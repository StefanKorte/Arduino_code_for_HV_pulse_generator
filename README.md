# Arduino_code_for_HV_pulse_generator

Arduino nano controlled high voltage pulse generator

Outputs A4 and A5 are used to charge a coil.
The charge time will determine the pulse voltage.

An encoder is used to set a voltage.
The necessary charge time gets calculated.
Parameters here are:
	charging constant alpha=-L/R of the coil,
	maximum achievable output voltage.

The set voltage is displayed on a 8x2 LCD.

The output is set to HIGH for the calculated charge time.
When the output is switched back to LOW, the pulse is generated.

The coil charged by A4 will generate positive pulses,
the coil charged by A5 will generate negative pulses.
