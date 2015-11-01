[appendix]
== Updating Device Firmware

	TeleMega, TeleMetrum v2, EasyMega, EasyMini and TeleDongle v3
	are all programmed directly over their USB connectors (self
	programming). TeleMetrum v1, TeleMini and TeleDongle v0.2 are
	all programmed by using another device as a programmer (pair
	programming). It's important to recognize which kind of devices
	you have before trying to reprogram them.

	You may wish to begin by ensuring you have current firmware
	images.  These are distributed as part of the AltOS software
	bundle that also includes the AltosUI ground station program.
	Newer ground station versions typically work fine with older
	firmware versions, so you don't need to update your devices
	just to try out new software features.  You can always
	download the most recent version from
	http://www.altusmetrum.org/AltOS/

	=== Updating TeleMega, TeleMetrum v2, EasyMega, EasyMini or TeleDongle v3 Firmware

		Self-programmable devices (TeleMega, TeleMetrum v2,
		EasyMega and EasyMini) are reprogrammed by connecting
		them to your computer over USB

		. Attach a battery if necessary and power switch to
		  the target device. Power up the device.

		. Using a Micro USB cable, connect the target device to your
		  computer's USB socket.

		. Run AltosUI, and select 'Flash Image' from the File menu.

		. Select the target device in the Device Selection dialog.

		. Select the image you want to flash to the device,
		  which should have a name in the form
		  <product>-v<product-version>-<software-version>.ihx,
		  such as TeleMega-v1.0-1.3.0.ihx.

		. Make sure the configuration parameters are
		  reasonable looking. If the serial number and/or RF
		  configuration values aren't right, you'll need to
		  change them.

		. Hit the 'OK' button and the software should proceed
		  to flash the device with new firmware, showing a
		  progress bar.

		. Verify that the device is working by using the
		  'Configure Altimeter' or 'Configure Groundstation'
		  item to check over the configuration.

		==== Recovering From Self-Flashing Failure

			If the firmware loading fails, it can leave the device
			unable to boot. Not to worry, you can force the device to
			start the boot loader instead, which will let you try to
			flash the device again.

			On each device, connecting two pins from one of the exposed
			connectors will force the boot loader to start, even if the
			regular operating system has been corrupted in some way.

			TeleMega::

			Connect pin 6 and pin 1 of the companion
			connector. Pin 1 can be identified by the square pad
			around it, and then the pins could sequentially across
			the board. Be very careful to *not* short pin 8 to
			anything as that is connected directly to the
			battery. Pin 7 carries 3.3V and the board will crash
			if that is connected to pin 1, but shouldn't damage
			the board.

			EasyMega::

			Connect pin 6 and pin 1 of the companion
			connector. Pin 1 can be identified by the square pad
			around it, and then the pins could sequentially across
			the board. Be very careful to *not* short pin 8 to
			anything as that is connected directly to the
			battery. Pin 7 carries 3.3V and the board will crash
			if that is connected to pin 1, but shouldn't damage
			the board.

			TeleMetrum v2::

			Connect pin 6 and pin 1 of the companion
			connector. Pin 1 can be identified by the square pad
			around it, and then the pins could sequentially across
			the board. Be very careful to *not* short pin 8 to
			anything as that is connected directly to the
			battery. Pin 7 carries 3.3V and the board will crash
			if that is connected to pin 1, but shouldn't damage
			the board.

			EasyMini::

			Connect pin 6 and pin 1 of the debug connector, which
			is the six holes next to the beeper. Pin 1 can be
			identified by the square pad around it, and then the
			pins could sequentially across the board, making Pin 6
			the one on the other end of the row.

			TeleDongle v3::

			Connect pin 32 on the CPU to ground. Pin 32 is closest
			to the USB wires on the row of pins towards the center
			of the board. Ground is available on the capacitor
			next to it, on the end towards the USB wires.

			Once you've located the right pins:

			. Turn the altimeter power off.

			. Connect a battery.

			. Connect the indicated terminals together with a
			  short piece of wire. Take care not to accidentally
			  connect anything else.

			. Connect USB

			. Turn the board power on.

			The board should now be visible over USB as
			'AltosFlash' and be ready to receive firmware.  Once
			the board has been powered up, you can remove the
			piece of wire.

	=== Pair Programming

		The big concept to understand is that you have to use
		a TeleMetrum v1.0, TeleBT v1.0 or TeleDongle v0.2 as a
		programmer to update a pair programmed device. Due to
		limited memory resources in the cc1111, we don't
		support programming directly over USB for these
		devices.

		If you need to update the firmware on a TeleDongle
		v0.2, we recommend updating the altimeter first,
		before updating TeleDongle.  However, note that
		TeleDongle rarely need to be updated.  Any firmware
		version 1.0.1 or later will work, version 1.2.1 may
		have improved receiver performance slightly.

		==== Updating TeleMetrum v1.x Firmware

			. Find the 'programming cable' that you got as
			  part of the starter kit, that has a red
			  8-pin MicroMaTch connector on one end and a
			  red 4-pin MicroMaTch connector on the other
			  end.

			. Take the 2 screws out of the TeleDongle v0.2
			  or TeleBT v1.0 case to get access to the
			  circuit board.

			. Plug the 8-pin end of the programming cable
			  to the matching connector on the TeleDongle
			  v0.2 or TeleBT v1.0, and the 4-pin end to
			  the matching connector on the TeleMetrum.
			  Note that each MicroMaTch connector has an
			  alignment pin that goes through a hole in
			  the PC board when you have the cable
			  oriented correctly.

			. Attach a battery to the TeleMetrum board.

			. Plug the TeleDongle v0.2 or TeleBT v1.0 into
			  your computer's USB port, and power up the
			  TeleMetrum.

			. Run AltosUI, and select 'Flash Image' from
			  the File menu.

			. Pick the TeleDongle v0.2 or TeleBT v1.0
			  device from the list, identifying it as the
			  programming device.

			. Select the image you want put on the
			  TeleMetrum, which should have a name in the
			  form telemetrum-v1.2-1.0.0.ihx.  It should
			  be visible in the default directory, if not
			  you may have to poke around your system to
			  find it.

			. Make sure the configuration parameters are
			  reasonable looking. If the serial number
			  and/or RF configuration values aren't right,
			  you'll need to change them.

			. Hit the 'OK' button and the software should
			  proceed to flash the TeleMetrum with new
			  firmware, showing a progress bar.

			. Confirm that the TeleMetrum board seems to
			  have updated OK, which you can do by
			  plugging in to it over USB and using a
			  terminal program to connect to the board and
			  issue the 'v' command to check the version,
			  etc.

			If something goes wrong, give it another try.

		==== Updating TeleMini Firmware

			You'll need a special 'programming cable' to
			reprogram the TeleMini.  You can make your own
			using an 8-pin MicroMaTch connector on one end
			and a set of four pins on the other.

			. Take the 2 screws out of the TeleDongle v0.2
			  or TeleBT v1.0 case to get access to the
			  circuit board.

			. Plug the 8-pin end of the programming cable
			  to the matching connector on the TeleDongle
			  v0.2 or TeleBT v1.0, and the 4-pins into the
			  holes in the TeleMini circuit board.  Note
			  that the MicroMaTch connector has an
			  alignment pin that goes through a hole in
			  the PC board when you have the cable
			  oriented correctly, and that pin 1 on the
			  TeleMini board is marked with a square pad
			  while the other pins have round pads.

			. Attach a battery to the TeleMini board.

			. Plug the TeleDongle v0.2 or TeleBT v1.0 into
			  your computer's USB port, and power up the
			  TeleMini

			. Run AltosUI, and select 'Flash Image' from
			  the File menu.

			. Pick the TeleDongle v0.2 or TeleBT v1.0
			  device from the list, identifying it as the
			  programming device.

			. Select the image you want put on the
			  TeleMini, which should have a name in the
			  form telemini-v1.0-1.0.0.ihx.  It should be
			  visible in the default directory, if not you
			  may have to poke around your system to find
			  it.

			. Make sure the configuration parameters are
			  reasonable looking. If the serial number
			  and/or RF configuration values aren't right,
			  you'll need to change them.

			. Hit the 'OK' button and the software should
			  proceed to flash the TeleMini with new
			  firmware, showing a progress bar.

			. Confirm that the TeleMini board seems to
			  have updated OK, which you can do by
			  configuring it over the radio link through
			  the TeleDongle, or letting it come up in
			  “flight” mode and listening for telemetry.

			If something goes wrong, give it another try.

		==== Updating TeleDongle v0.2 Firmware

		Updating TeleDongle v0.2 firmware is just like
		updating TeleMetrum v1.x or TeleMini firmware, but you
		use either a TeleMetrum v1.x, TeleDongle v0.2 or
		TeleBT v1.0 as the programmer.

		. Find the 'programming cable' that you got as part of
		  the starter kit, that has a red 8-pin MicroMaTch
		  connector on one end and a red 4-pin MicroMaTch
		  connector on the other end.

		. Find the USB cable that you got as part of the
		  starter kit, and plug the “mini” end in to the
		  mating connector on TeleMetrum v1.x, TeleDongle v0.2
		  or TeleBT v1.0.

		. Take the 2 screws out of the TeleDongle v0.2 or
		  TeleBT v1.0 case to get access to the circuit board.

		. Plug the 8-pin end of the programming cable to the
		  matching connector on the programmer, and the 4-pin
		  end to the matching connector on the TeleDongle
		  v0.2.  Note that each MicroMaTch connector has an
		  alignment pin that goes through a hole in the PC
		  board when you have the cable oriented correctly.

		. Attach a battery to the TeleMetrum v1.x board if
		  you're using one.

		. Plug both the programmer and the TeleDongle into
		  your computer's USB ports, and power up the
		  programmer.

		. Run AltosUI, and select 'Flash Image' from the File
		  menu.

		. Pick the programmer device from the list,
		  identifying it as the programming device.


		. Select the image you want put on the TeleDongle
		  v0.2, which should have a name in the form
		  teledongle-v0.2-1.0.0.ihx.  It should be visible in
		  the default directory, if not you may have to poke
		  around your system to find it.

		. Make sure the configuration parameters are
		  reasonable looking. If the serial number and/or RF
		  configuration values aren't right, you'll need to
		  change them.  The TeleDongle v0.2 serial number is
		  on the “bottom” of the circuit board, and can
		  usually be read through the translucent blue plastic
		  case without needing to remove the board from the
		  case.

		. Hit the 'OK' button and the software should proceed
		  to flash the TeleDongle v0.2 with new firmware,
		  showing a progress bar.

		. Confirm that the TeleDongle v0.2 board seems to have
		  updated OK, which you can do by plugging in to it
		  over USB and using a terminal program to connect to
		  the board and issue the 'v' command to check the
		  version, etc.  Once you're happy, remove the
		  programming cable and put the cover back on the
		  TeleDongle v0.2.

		If something goes wrong, give it another try.

		Be careful removing the programming cable from the
		locking 8-pin connector on TeleMetrum.  You'll need a
		fingernail or perhaps a thin screwdriver or knife
		blade to gently pry the locking ears out slightly to
		extract the connector.  We used a locking connector on
		TeleMetrum to help ensure that the cabling to
		companion boards used in a rocket don't ever come
		loose accidentally in flight.