<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V4.5//EN"
"/usr/share/xml/docbook/schema/dtd/4.5/docbookx.dtd">

<article>
  <para>
    Version 1.6 includes support for our updated TeleDongle v3.0
    product and bug fixes in in the flight software for all our boards
    and ground station interfaces.
  </para>
  <para>
    AltOS New Features
    <itemizedlist>
      <listitem>
	<para>
	  Add support for TeleDongle v3.0 boards.
	</para>
      </listitem>
    </itemizedlist>
  </para>
  <para>
    AltOS Fixes
    <itemizedlist>
      <listitem>
	<para>
	  Don't beep out the continuity twice by accident in idle mode.
	  If the battery voltage report takes longer than the initialiation
	  sequence, the igniter continuity would get reported twice.
	</para>
      </listitem>
      <listitem>
	<para>
	  Record all 32 bits of gyro calibration data in TeleMega and
	  EasyMega log files. This fixes computation of the gyro rates
	  in AltosUI.
	</para>
      </listitem>
      <listitem>
	<para>
	  Change TeleDongle LED usage. Green LED flashes when valid
	  packet is received. Red LED flashes when invalid packet is
	  received.
	</para>
      </listitem>
    </itemizedlist>
  </para>
  <para>
    AltosUI and TeleGPS New Features
    <itemizedlist>
      <listitem>
	<para>
	  Compute tilt angle from TeleMega and EasyMega log
	  files. This duplicates the quaternion-based angle tracking
	  code from the flight firmware inside the ground station
	  software so that post-flight analysis can include evaluation
	  of the tilt angle.
	</para>
      </listitem>
      <listitem>
	<para>
	  Shows the tool button window when starting with a data file
	  specified. This means that opening a data file from the file
	  manager will now bring up the main window to let you operate
	  the whole application.
	</para>
      </listitem>
    </itemizedlist>
  </para>
  <para>
    AltosUI Fixes
    <itemizedlist>
      <listitem>
	<para>
	  Show the 'Connecting' dialog when using Monitor Idle. Lets
	  you cancel the Monitor Idle startup when connecting over the
	  radio link.
	</para>
      </listitem>
    </itemizedlist>
  </para>
</article>