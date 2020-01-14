This repository is made for a custom-built keyboard array that plays audio when buttons are pressed. 
It is built with an arduino feather M0 proto board and musicmaker featherwing, both from Adafruit.
The button array is custom built for the particular purpose of this device.

This code basically does three things: plays a special audio track at boot, 
                                       reads keys if pressed
                                       plays an audio track to match the pressed key
                                       
This code assumes 2 arduino libraries are present:

Keypad, a library developed for matrix keypads. https://playground.arduino.cc/Code/Keypad/
Adafruit VS1053 library for Music Maker shields. https://github.com/adafruit/Adafruit_VS1053_Library

The other libraries in the code are typical Arduino communication libraries.
