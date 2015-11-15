# AT28HC64B-EEPROM-Arduino-Uno
Arduino Uno Programmer for Atmel AT28HC64B 8K Bytes EEPROM 

Use your Arduino Uno to write and read to/from an Atmel AT28HC64B 8K bytes EEPROM
This program uses two 74HC595 shift register chips to handle the 13 bits address

EEPROM datasheet: http://www.atmel.com/images/doc0274.pdf
  
Based almost entirely on the work for the lower capacity AT28HC16
by Chris Torrence, here: http://forum.6502.org/viewtopic.php?f=4&t=3156

Connections:

The chips runs on 5V
The first 74HC595 stores the LSB and the second the MSB. 
Q7' from the first connects (chains) to DS on the second.

Pin A1 -> SH_CP on both 74HC595
Pin A2 -> ST_CP on both 74HC595
Pin A4 -> DS (data) on first (least significant byte) 74HC595

Pins 2 through 9 are the Data I/O and are connected on IO0..IO7 in the AT28
Pin 10 -> CE (chip enable)
Pin 11 -> WE (write enable)
Pin 12 -> OE (output enable)

Outputs of shift registers connects to A0..A12 in the AT28:
A0..A7 from the first (LSB) 74HC595 and
A8..12 from the second (MSB) 74HC595 

That's it.

