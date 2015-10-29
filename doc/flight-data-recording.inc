[appendix]
== Flight Data Recording
      
	Each flight computer logs data at 100 samples per second
	during ascent and 10 samples per second during descent, except
	for TeleMini v1.0, which records ascent at 10 samples per
	second and descent at 1 sample per second. Data are logged to
	an on-board flash memory part, which can be partitioned into
	several equal-sized blocks, one for each flight.

	.Data Storage on Altus Metrum altimeters
	[options="header",cols="1,1,1,1"]
	|====
	|Device			|Bytes per Sample	|Total Storage	|Minutes at Full Rate
	|TeleMetrum v1.0	|8	|1MB	|20
	|TeleMetrum v1.1 v1.2	|8	|2MB	|40
	|TeleMetrum v2.0	|16	|8MB	|80
	|TeleMini v1.0		|2	|5kB	|4
	|TeleMini v2.0		|16	|1MB	|10
	|EasyMini		|16	|1MB	|10
	|TeleMega		|32	|8MB	|40
	|EasyMega		|32	|8MB	|40
	|====
      
	The on-board flash is partitioned into separate flight logs,
	each of a fixed maximum size. Increase the maximum size of
	each log and you reduce the number of flights that can be
	stored. Decrease the size and you can store more flights.
            
	Configuration data is also stored in the flash memory on
	TeleMetrum v1.x, TeleMini and EasyMini. This consumes 64kB
	of flash space.  This configuration space is not available
	for storing flight log data. TeleMetrum v2.0, TeleMega and EasyMega
	store configuration data in a bit of eeprom available within
	the processor chip, leaving that space available in flash for
	more flight data.
            
	To compute the amount of space needed for a single flight, you
	can multiply the expected ascent time (in seconds) by 100
	times bytes-per-sample, multiply the expected descent time (in
	seconds) by 10 times the bytes per sample and add the two
	together. That will slightly under-estimate the storage (in
	bytes) needed for the flight. For instance, a TeleMetrum v2.0 flight spending
	20 seconds in ascent and 150 seconds in descent will take
	about (20 * 1600) + (150 * 160) = 56000 bytes of storage. You
	could store dozens of these flights in the on-board flash.
            
	The default size allows for several flights on each flight
	computer, except for TeleMini v1.0, which only holds data for a
	single flight. You can adjust the size.
            
	Altus Metrum flight computers will not overwrite existing
	flight data, so be sure to download flight data and erase it
	from the flight computer before it fills up. The flight
	computer will still successfully control the flight even if it
	cannot log data, so the only thing you will lose is the data.