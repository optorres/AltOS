
// Copyright (c) 2010 Anthony Towns
// GPL v2 or later

package altosui;

interface AltosDataPoint {
    int version();
    int serial();
    int flight();
    String callsign();
    double time();
    double rssi();

    int state();
    String state_name();

    double acceleration();
    double height();
    double speed();
    double temperature();
    double battery_voltage();
    double drogue_voltage();
    double main_voltage();
    boolean has_accel();
}

