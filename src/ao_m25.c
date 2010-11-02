/*
 * Copyright © 2010 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include "ao.h"

/*
 * Each flash chip is arranged in 64kB sectors; the
 * chip cannot erase in units smaller than that.
 *
 * Writing happens in units of 256 byte pages and
 * can only change bits from 1 to 0. So, you can rewrite
 * the same contents, or append to an existing page easily enough
 */

#define M25_WREN	0x06	/* Write Enable */
#define M25_WRDI	0x04	/* Write Disable */
#define M25_RDID	0x9f	/* Read Identification */
#define M25_RDSR	0x05	/* Read Status Register */
#define M25_WRSR	0x01	/* Write Status Register */
#define M25_READ	0x03	/* Read Data Bytes */
#define M25_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define M25_PP		0x02	/* Page Program */
#define M25_SE		0xd8	/* Sector Erase */
#define M25_BE		0xc7	/* Bulk Erase */
#define M25_DP		0xb9	/* Deep Power-down */

/* RDID response */
#define M25_MANUF_OFFSET	0
#define M25_MEMORY_TYPE_OFFSET	1
#define M25_CAPACITY_OFFSET	2
#define M25_UID_OFFSET		3
#define M25_CFI_OFFSET		4
#define M25_RDID_LEN		4	/* that's all we need */

#define M25_CAPACITY_128KB	0x11
#define M25_CAPACITY_256KB	0x12
#define M25_CAPACITY_512KB	0x13
#define M25_CAPACITY_1MB	0x14
#define M25_CAPACITY_2MB	0x15

/*
 * Status register bits
 */

#define M25_STATUS_SRWD		(1 << 7)	/* Status register write disable */
#define M25_STATUS_BP_MASK	(7 << 2)	/* Block protect bits */
#define M25_STATUS_BP_SHIFT	(2)
#define M25_STATUS_WEL		(1 << 1)	/* Write enable latch */
#define M25_STATUS_WIP		(1 << 0)	/* Write in progress */

/*
 * On teleterra, the m25 chip select pins are
 * wired on P0_0 through P0_3.
 */

#define AO_M25_MAX_CHIPS	4

static uint8_t ao_m25_size[AO_M25_MAX_CHIPS];	/* number of sectors in each chip */
static uint8_t ao_m25_pin[AO_M25_MAX_CHIPS];	/* chip select pin for each chip */
static uint8_t ao_m25_numchips;			/* number of chips detected */
static uint8_t ao_m25_total;			/* total sectors available */
static uint8_t ao_m25_wip;			/* write in progress */

static __xdata uint8_t ao_m25_mutex;

/*
 * This little array is abused to send and receive data. A particular
 * caution -- the read and write addresses are written into the last
 * three bytes of the array by ao_m25_set_page_address and then the
 * first byte is used by ao_m25_wait_wip and ao_m25_write_enable, neither
 * of which touch those last three bytes.
 */

static __xdata uint8_t	ao_m25_instruction[4];

#define AO_M25_SELECT(cs)		(SPI_CS_PORT &= ~(cs))
#define AO_M25_DESELECT(cs)		(SPI_CS_PORT |= (cs))
#define AO_M25_PAGE_TO_SECTOR(page)	((page) >> 8)
#define AO_M25_SECTOR_TO_PAGE(sector)	(((uint16_t) (sector)) << 8)

/*
 * Block until the specified chip is done writing
 */
static void
ao_m25_wait_wip(uint8_t cs)
{
	if (ao_m25_wip & cs) {
		AO_M25_SELECT(cs);
		ao_m25_instruction[0] = M25_RDSR;
		ao_spi_send(ao_m25_instruction, 1);
		do {
			ao_spi_recv(ao_m25_instruction, 1);
		} while (ao_m25_instruction[0] & M25_STATUS_WIP);
		AO_M25_DESELECT(cs);
		ao_m25_wip &= ~cs;
	}
}

/*
 * Set the write enable latch so that page program and sector
 * erase commands will work. Also mark the chip as busy writing
 * so that future operations will block until the WIP bit goes off
 */
static void
ao_m25_write_enable(uint8_t cs)
{
	AO_M25_SELECT(cs);
	ao_m25_instruction[0] = M25_WREN;
	ao_spi_send(&ao_m25_instruction, 1);
	AO_M25_DESELECT(cs);
	ao_m25_wip |= cs;
}


/*
 * Returns the number of 64kB sectors
 */
static uint8_t
ao_m25_read_capacity(uint8_t cs)
{
	uint8_t	capacity;
	AO_M25_SELECT(cs);
	ao_m25_instruction[0] = M25_RDID;
	ao_spi_send(ao_m25_instruction, 1);
	ao_spi_recv(ao_m25_instruction, M25_RDID_LEN);
	AO_M25_DESELECT(cs);

	/* Check to see if the chip is present */
	if (ao_m25_instruction[0] == 0xff)
		return 0;
	capacity = ao_m25_instruction[M25_CAPACITY_OFFSET];

	/* Sanity check capacity number */
	if (capacity < 0x11 || 0x1f < capacity)
		return 0;
	return 1 << (capacity - 0x10);
}

static uint8_t
ao_m25_set_page_address(uint16_t page)
{
	uint8_t	chip, size;

	for (chip = 0; chip < ao_m25_numchips; chip++) {
		size = ao_m25_size[chip];
		if (AO_M25_PAGE_TO_SECTOR(page) < size)
			break;
		page -= AO_M25_SECTOR_TO_PAGE(size);
	}
	if (chip == ao_m25_numchips)
		ao_panic(AO_PANIC_EE);

	chip = ao_m25_pin[chip];
	ao_m25_wait_wip(chip);

	ao_m25_instruction[1] = page >> 8;
	ao_m25_instruction[2] = page;
	ao_m25_instruction[3] = 0;
	return chip;
}

/*
 * Scan the possible chip select lines
 * to see which flash chips are connected
 */
static void
ao_m25_scan(void)
{
	uint8_t	pin, size;

	if (ao_m25_total)
		return;

	ao_m25_numchips = 0;
	for (pin = 1; pin != 0; pin <<= 1) {
		if (M25_CS_MASK & pin) {
			size = ao_m25_read_capacity(pin);
			if (size != 0) {
				ao_m25_size[ao_m25_numchips] = size;
				ao_m25_pin[ao_m25_numchips] = pin;
				ao_m25_total += size;
				ao_m25_numchips++;
			}
		}
	}
}

/*
 * Erase the specified sector
 */
void
ao_flash_erase_sector(uint8_t sector) __reentrant
{
	uint8_t	cs;
	uint16_t page = AO_M25_SECTOR_TO_PAGE(sector);

	ao_mutex_get(&ao_m25_mutex);
	ao_m25_scan();

	cs = ao_m25_set_page_address(page);
	ao_m25_wait_wip(cs);
	ao_m25_write_enable(cs);

	ao_m25_instruction[0] = M25_SE;
	AO_M25_SELECT(cs);
	ao_spi_send(ao_m25_instruction, 4);
	AO_M25_DESELECT(cs);
	ao_m25_wip |= cs;

	ao_mutex_put(&ao_m25_mutex);
}

/*
 * Write one page
 */
void
ao_flash_write_page(uint16_t page, uint8_t __xdata *d) __reentrant
{
	uint8_t	cs;

	ao_mutex_get(&ao_m25_mutex);
	ao_m25_scan();

	cs = ao_m25_set_page_address(page);
	ao_m25_write_enable(cs);

	ao_m25_instruction[0] = M25_PP;
	AO_M25_SELECT(cs);
	ao_spi_send(ao_m25_instruction, 4);
	ao_spi_send(d, 256);
	AO_M25_DESELECT(cs);

	ao_mutex_put(&ao_m25_mutex);
}

/*
 * Read one page
 */
void
ao_flash_read_page(uint16_t page, __xdata uint8_t *d) __reentrant
{
	uint8_t	cs;

	ao_mutex_get(&ao_m25_mutex);
	ao_m25_scan();

	cs = ao_m25_set_page_address(page);

	/* No need to use the FAST_READ as we're running at only 8MHz */
	ao_m25_instruction[0] = M25_READ;
	AO_M25_SELECT(cs);
	ao_spi_send(ao_m25_instruction, 4);
	ao_spi_recv(d, 256);
	AO_M25_DESELECT(cs);

	ao_mutex_put(&ao_m25_mutex);
}

static __xdata uint8_t ao_flash_block[256];

static void
ao_flash_dump(void) __reentrant
{
	uint8_t i;

	ao_cmd_hex();
	if (ao_cmd_status != ao_cmd_success)
		return;
	ao_flash_read_page(ao_cmd_lex_i, ao_flash_block);
	i = 0;
	do {
		if ((i & 7) == 0) {
			if (i)
				putchar('\n');
			ao_cmd_put16((uint16_t) i);
		}
		putchar(' ');
		ao_cmd_put8(ao_flash_block[i]);
		++i;
	} while (i != 0);
	putchar('\n');
}

static void
ao_flash_store(void) __reentrant
{
	uint16_t block;
	uint8_t i;
	uint16_t len;
	uint8_t b;

	ao_cmd_hex();
	block = ao_cmd_lex_i;
	ao_cmd_hex();
	i = ao_cmd_lex_i;
	ao_cmd_hex();
	len = ao_cmd_lex_i;
	if (ao_cmd_status != ao_cmd_success)
		return;
	ao_flash_read_page(block, ao_flash_block);
	while (len--) {
		ao_cmd_hex();
		if (ao_cmd_status != ao_cmd_success)
			return;
		b = ao_cmd_lex_i;
		ao_flash_block[i] = ao_cmd_lex_i;
		i++;
	}
	ao_flash_write_page(block, ao_flash_block);
}

static void
ao_flash_info(void) __reentrant
{
	uint8_t	chip, cs;

	ao_mutex_get(&ao_m25_mutex);
	ao_m25_scan();
	ao_mutex_put(&ao_m25_mutex);

	printf ("Detected chips %d size %d\n", ao_m25_numchips, ao_m25_total);
	for (chip = 0; chip < ao_m25_numchips; chip++)
		printf ("Flash chip %d select %02x size %d manf %02x type %02x cap %02x uid %02x\n",
			chip, ao_m25_pin[chip], ao_m25_size[chip]);

	printf ("Available chips:\n");
	for (cs = 1; cs != 0; cs <<= 1) {
		if ((M25_CS_MASK & cs) == 0)
			continue;

		ao_mutex_get(&ao_m25_mutex);
		AO_M25_SELECT(cs);
		ao_m25_instruction[0] = M25_RDID;
		ao_spi_send(ao_m25_instruction, 1);
		ao_spi_recv(ao_m25_instruction, M25_RDID_LEN);
		AO_M25_DESELECT(cs);

		printf ("Select %02x manf %02x type %02x cap %02x uid %02x\n",
			cs,
			ao_m25_instruction[M25_MANUF_OFFSET],
			ao_m25_instruction[M25_MEMORY_TYPE_OFFSET],
			ao_m25_instruction[M25_CAPACITY_OFFSET],
			ao_m25_instruction[M25_UID_OFFSET]);
		ao_mutex_put(&ao_m25_mutex);
	}
}

__code struct ao_cmds ao_flash_cmds[] = {
	{ 'e', ao_flash_dump, 	"e <block>                          Dump a block of EEPROM data" },
	{ 'w', ao_flash_store,	"w <block> <start> <len> <data> ... Write data to EEPROM" },
	{ 'F', ao_flash_info,   "F                                  Display flash info" },
	{ 0,   ao_flash_store, NULL },
};

void
ao_flash_init(void)
{
	/* Set up chip select wires */
	SPI_CS_PORT |= M25_CS_MASK;	/* raise all CS pins */
	SPI_CS_DIR |= M25_CS_MASK;	/* set CS pins as outputs */
	SPI_CS_SEL &= ~M25_CS_MASK;	/* set CS pins as GPIO */
	ao_spi_init();

	ao_cmd_register(&ao_flash_cmds[0]);
}
