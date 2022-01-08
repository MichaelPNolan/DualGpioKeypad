# DualMC17Keypad
A solution to using multiple MCP23017 with Keypad Lib in progress

When I made another project I used this repository https://github.com/joeyoung/arduino_keypads
Rather than fork it, or a small part I'm just copying the code and the hacked version I made to give a quick and dirty 2nd instance.
Why? Well I wired up 2 keyboard keypad circuitboards to one MC23017 GPIO expander so its all controllable over a single pair of I2C wires to 
an Arduino Uno. 
Update: Same day - After I finally got it working i went back and used 2 instances of the one library and it worked so it was more of a malfunction it seems. Isn't that the case
when you can't figure it out you think there is a different reason but its just conjecture. At least i figured it out before i spent too much time "fixing". So onto future goals already I guess.

Its a messy project involving fixing very old corroded away traces and carbon pads where conductive rubber makes connections to facilitate a musical keyboard.
The library by Joe Young is overrides the standard keypad library with calls to the GPIO expander for PinMode, ReadPin, WritePin within a keypad instance. However, it
seemed that it was reading from the last initialization even with separate objects due to static variables that aren't allocated. Seems so - so I renamed the object and some
variables that were conflicting and I include 2 libraryies Keypad_MC17 and the hacked 2nd instance of the library just to document. 

Future Goals - to perhaps fork or module-ize the library into this so that it has an additional variable to reference which keypad or fix the variable settings so
multiple instances of the keypad work without issue.  To add Midi functionality and wire up a midi-output to the physical keyboard.
