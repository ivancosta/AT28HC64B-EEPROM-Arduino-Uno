/** 
 *  Arduino Uno Programmer for Atmel AT28HC64B 8K Bytes EEPROM 
 *  Datasheet: http://www.atmel.com/images/doc0274.pdf
 *  
 *  
 *  This program uses two 74HC595 shift register chips to handle the 13 bits address
 *  
 *  Based almost entirely on the work for the lower capacity AT28HC16
 *  by Chris Torrence, here: http://forum.6502.org/viewtopic.php?f=4&t=3156
 *
 */

#define SER_Pin   A4  // 75HC595 pin 14, DS serial input
#define STCP_Pin  A2  // 75HC595 pin 12, STCP storage register clock
#define SHCP_Pin  A1  // 75HC595 pin 11, SHCP shift register clock

#define AT28_CE_Pin  10  // AT28HC64 WE, write enable
#define AT28_WE_Pin  11  // AT28HC64 WE, write enable
#define AT28_OE_Pin  12  // AT28HC64 OE, output enable

#define numOfAddressPins 13   // A0 -> A12 = 2^13 = 8192 address locations on the AT28HC64B
#define ADDRESS_MAX (1 << numOfAddressPins)

// Fast way of setting/reading/writing pins on the Arduino
// http://masteringarduino.blogspot.com/2013_10_01_archive.html
#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P) ((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)  ((uint8_t)(1<<pinIndex(P)))
#define digitalLow(P)  *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P) ((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)  ((*(pinOfPin(P))& pinMask(P))==0)


// Data to be written to the AT28.
// Use PROGMEM to put into Arduino flash memory
const byte values[2048] PROGMEM = {
  28, 34, 42, 42, 44, 32, 30,  0,  8, 20, 34, 34, 62, 34, 34,  0, 60, 34, 34, 60, 34, 34, 60,  0, 28, 34, 32, 32, 32, 34, 28,  0,
  60, 34, 34, 34, 34, 34, 60,  0, 62, 32, 32, 60, 32, 32, 62,  0, 62, 32, 32, 60, 32, 32, 32,  0, 30, 32, 32, 38, 34, 34, 30,  0,
  34, 34, 34, 62, 34, 34, 34,  0, 28,  8,  8,  8,  8,  8, 28,  0,  2,  2,  2,  2,  2, 34, 28,  0, 34, 36, 40, 48, 40, 36, 34,  0,
  32, 32, 32, 32, 32, 32, 62,  0, 34, 54, 42, 42, 34, 34, 34,  0, 34, 34, 50, 42, 38, 34, 34,  0, 28, 34, 34, 34, 34, 34, 28,  0,
  60, 34, 34, 60, 32, 32, 32,  0, 28, 34, 34, 34, 42, 36, 26,  0, 60, 34, 34, 60, 40, 36, 34,  0, 28, 34, 32, 28,  2, 34, 28,  0,
  62,  8,  8,  8,  8,  8,  8,  0, 34, 34, 34, 34, 34, 34, 28,  0, 34, 34, 34, 34, 34, 20,  8,  0, 34, 34, 34, 42, 42, 54, 34,  0,
  34, 34, 20,  8, 20, 34, 34,  0, 34, 34, 20,  8,  8,  8,  8,  0, 62,  2,  4,  8, 16, 32, 62,  0, 62, 48, 48, 48, 48, 48, 62,  0,
  0, 32, 16,  8,  4,  2,  0,  0, 62,  6,  6,  6,  6,  6, 62,  0,  8, 20, 34,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  0,  8,  0, 20, 20, 20,  0,  0,  0,  0,  0, 20, 20, 62, 20, 62, 20, 20,  0,
  8, 30, 40, 28, 10, 60,  8,  0, 50, 50,  4,  8, 16, 38, 38,  0, 24, 36, 40, 16, 42, 36, 26,  0,  8,  8,  8,  0,  0,  0,  0,  0,
  8, 16, 32, 32, 32, 16,  8,  0,  8,  4,  2,  2,  2,  4,  8,  0,  8, 42, 28,  8, 28, 42,  8,  0,  0,  8,  8, 62,  8,  8,  0,  0,
  0,  0,  0,  0,  0, 24,  8, 16,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24, 24,  0,  0,  2,  4,  8, 16, 32,  0,  0,
  28, 34, 38, 42, 50, 34, 28,  0,  8, 24,  8,  8,  8,  8, 28,  0, 28, 34,  2, 12, 16, 32, 62,  0, 62,  2,  4, 12,  2, 34, 28,  0,
  4, 12, 20, 36, 62,  4,  4,  0, 62, 32, 60,  2,  2, 34, 28,  0, 14, 16, 32, 60, 34, 34, 28,  0, 62,  2,  4,  8, 16, 16, 16,  0,
  28, 34, 34, 28, 34, 34, 28,  0, 28, 34, 34, 30,  2,  4, 56,  0,  0,  0, 24, 24,  0, 24, 24,  0,  0,  0, 24, 24,  0, 24,  8, 16,
  4,  8, 16, 32, 16,  8,  4,  0,  0,  0, 60,  0, 60,  0,  0,  0, 16,  8,  4,  2,  4,  8, 16,  0, 28, 34,  4,  8,  8,  0,  8,  0,
  156, 162, 170, 170, 172, 160, 158, 128, 136, 148, 162, 162, 190, 162, 162, 128, 188, 162, 162, 188, 162, 162, 188, 128, 156, 162, 160, 160, 160, 162, 156, 128,
  188, 162, 162, 162, 162, 162, 188, 128, 190, 160, 160, 188, 160, 160, 190, 128, 190, 160, 160, 188, 160, 160, 160, 128, 158, 160, 160, 166, 162, 162, 158, 128,
  162, 162, 162, 190, 162, 162, 162, 128, 156, 136, 136, 136, 136, 136, 156, 128, 130, 130, 130, 130, 130, 162, 156, 128, 162, 164, 168, 176, 168, 164, 162, 128,
  160, 160, 160, 160, 160, 160, 190, 128, 162, 182, 170, 170, 162, 162, 162, 128, 162, 162, 178, 170, 166, 162, 162, 128, 156, 162, 162, 162, 162, 162, 156, 128,
  188, 162, 162, 188, 160, 160, 160, 128, 156, 162, 162, 162, 170, 164, 154, 128, 188, 162, 162, 188, 168, 164, 162, 128, 156, 162, 160, 156, 130, 162, 156, 128,
  190, 136, 136, 136, 136, 136, 136, 128, 162, 162, 162, 162, 162, 162, 156, 128, 162, 162, 162, 162, 162, 148, 136, 128, 162, 162, 162, 170, 170, 182, 162, 128,
  162, 162, 148, 136, 148, 162, 162, 128, 162, 162, 148, 136, 136, 136, 136, 128, 190, 130, 132, 136, 144, 160, 190, 128, 190, 176, 176, 176, 176, 176, 190, 128,
  128, 160, 144, 136, 132, 130, 128, 128, 190, 134, 134, 134, 134, 134, 190, 128, 136, 148, 162, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 190, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 136, 136, 136, 136, 136, 128, 136, 128, 148, 148, 148, 128, 128, 128, 128, 128, 148, 148, 190, 148, 190, 148, 148, 128,
  136, 158, 168, 156, 138, 188, 136, 128, 178, 178, 132, 136, 144, 166, 166, 128, 152, 164, 168, 144, 170, 164, 154, 128, 136, 136, 136, 128, 128, 128, 128, 128,
  136, 144, 160, 160, 160, 144, 136, 128, 136, 132, 130, 130, 130, 132, 136, 128, 136, 170, 156, 136, 156, 170, 136, 128, 128, 136, 136, 190, 136, 136, 128, 128,
  128, 128, 128, 128, 128, 152, 136, 144, 128, 128, 128, 156, 128, 128, 128, 128, 128, 128, 128, 128, 128, 152, 152, 128, 128, 130, 132, 136, 144, 160, 128, 128,
  156, 162, 166, 170, 178, 162, 156, 128, 136, 152, 136, 136, 136, 136, 156, 128, 156, 162, 130, 140, 144, 160, 190, 128, 190, 130, 132, 140, 130, 162, 156, 128,
  132, 140, 148, 164, 190, 132, 132, 128, 190, 160, 188, 130, 130, 162, 156, 128, 142, 144, 160, 188, 162, 162, 156, 128, 190, 130, 132, 136, 144, 144, 144, 128,
  156, 162, 162, 156, 162, 162, 156, 128, 156, 162, 162, 158, 130, 132, 184, 128, 128, 128, 152, 152, 128, 152, 152, 128, 128, 128, 152, 152, 128, 152, 136, 144,
  132, 136, 144, 160, 144, 136, 132, 128, 128, 128, 188, 128, 188, 128, 128, 128, 144, 136, 132, 130, 132, 136, 144, 128, 156, 162, 132, 136, 136, 128, 136, 128,
  28, 34, 42, 42, 44, 32, 30,  0,  8, 20, 34, 34, 62, 34, 34,  0, 60, 34, 34, 60, 34, 34, 60,  0, 28, 34, 32, 32, 32, 34, 28,  0,
  60, 34, 34, 34, 34, 34, 60,  0, 62, 32, 32, 60, 32, 32, 62,  0, 62, 32, 32, 60, 32, 32, 32,  0, 30, 32, 32, 38, 34, 34, 30,  0,
  34, 34, 34, 62, 34, 34, 34,  0, 28,  8,  8,  8,  8,  8, 28,  0,  2,  2,  2,  2,  2, 34, 28,  0, 34, 36, 40, 48, 40, 36, 34,  0,
  32, 32, 32, 32, 32, 32, 62,  0, 34, 54, 42, 42, 34, 34, 34,  0, 34, 34, 50, 42, 38, 34, 34,  0, 28, 34, 34, 34, 34, 34, 28,  0,
  60, 34, 34, 60, 32, 32, 32,  0, 28, 34, 34, 34, 42, 36, 26,  0, 60, 34, 34, 60, 40, 36, 34,  0, 28, 34, 32, 28,  2, 34, 28,  0,
  62,  8,  8,  8,  8,  8,  8,  0, 34, 34, 34, 34, 34, 34, 28,  0, 34, 34, 34, 34, 34, 20,  8,  0, 34, 34, 34, 42, 42, 54, 34,  0,
  34, 34, 20,  8, 20, 34, 34,  0, 34, 34, 20,  8,  8,  8,  8,  0, 62,  2,  4,  8, 16, 32, 62,  0, 62, 48, 48, 48, 48, 48, 62,  0,
  0, 32, 16,  8,  4,  2,  0,  0, 62,  6,  6,  6,  6,  6, 62,  0,  8, 20, 34,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  0,  8,  0, 20, 20, 20,  0,  0,  0,  0,  0, 20, 20, 62, 20, 62, 20, 20,  0,
  8, 30, 40, 28, 10, 60,  8,  0, 50, 50,  4,  8, 16, 38, 38,  0, 24, 36, 40, 16, 42, 36, 26,  0,  8,  8,  8,  0,  0,  0,  0,  0,
  8, 16, 32, 32, 32, 16,  8,  0,  8,  4,  2,  2,  2,  4,  8,  0,  8, 42, 28,  8, 28, 42,  8,  0,  0,  8,  8, 62,  8,  8,  0,  0,
  0,  0,  0,  0,  0, 24,  8, 16,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0, 24, 24,  0,  0,  2,  4,  8, 16, 32,  0,  0,
  28, 34, 38, 42, 50, 34, 28,  0,  8, 24,  8,  8,  8,  8, 28,  0, 28, 34,  2, 12, 16, 32, 62,  0, 62,  2,  4, 12,  2, 34, 28,  0,
  4, 12, 20, 36, 62,  4,  4,  0, 62, 32, 60,  2,  2, 34, 28,  0, 14, 16, 32, 60, 34, 34, 28,  0, 62,  2,  4,  8, 16, 16, 16,  0,
  28, 34, 34, 28, 34, 34, 28,  0, 28, 34, 34, 30,  2,  4, 56,  0,  0,  0, 24, 24,  0, 24, 24,  0,  0,  0, 24, 24,  0, 24,  8, 16,
  4,  8, 16, 32, 16,  8,  4,  0,  0,  0, 60,  0, 60,  0,  0,  0, 16,  8,  4,  2,  4,  8, 16,  0, 28, 34,  4,  8,  8,  0,  8,  0,
  28, 34, 42, 42, 44, 32, 30,  0,  8, 20, 34, 34, 62, 34, 34,  0, 60, 34, 34, 60, 34, 34, 60,  0, 28, 34, 32, 32, 32, 34, 28,  0,
  60, 34, 34, 34, 34, 34, 60,  0, 62, 32, 32, 60, 32, 32, 62,  0, 62, 32, 32, 60, 32, 32, 32,  0, 30, 32, 32, 38, 34, 34, 30,  0,
  34, 34, 34, 62, 34, 34, 34,  0, 28,  8,  8,  8,  8,  8, 28,  0,  2,  2,  2,  2,  2, 34, 28,  0, 34, 36, 40, 48, 40, 36, 34,  0,
  32, 32, 32, 32, 32, 32, 62,  0, 34, 54, 42, 42, 34, 34, 34,  0, 34, 34, 50, 42, 38, 34, 34,  0, 28, 34, 34, 34, 34, 34, 28,  0,
  60, 34, 34, 60, 32, 32, 32,  0, 28, 34, 34, 34, 42, 36, 26,  0, 60, 34, 34, 60, 40, 36, 34,  0, 28, 34, 32, 28,  2, 34, 28,  0,
  62,  8,  8,  8,  8,  8,  8,  0, 34, 34, 34, 34, 34, 34, 28,  0, 34, 34, 34, 34, 34, 20,  8,  0, 34, 34, 34, 42, 42, 54, 34,  0,
  34, 34, 20,  8, 20, 34, 34,  0, 34, 34, 20,  8,  8,  8,  8,  0, 62,  2,  4,  8, 16, 32, 62,  0, 62, 48, 48, 48, 48, 48, 62,  0,
  0, 32, 16,  8,  4,  2,  0,  0, 62,  6,  6,  6,  6,  6, 62,  0,  8, 20, 34,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,
  32, 16,  8,  0,  0,  0,  0,  0,  0,  0, 28,  2, 30, 34, 30,  0, 32, 32, 44, 50, 34, 50, 44,  0,  0,  0, 30, 32, 32, 32, 30,  0,
  2,  2, 26, 38, 34, 38, 26,  0,  0,  0, 28, 34, 62, 32, 28,  0, 12, 18, 16, 56, 16, 16, 16,  0,  0,  2, 28, 34, 34, 30,  2, 28,
  32, 32, 44, 50, 34, 34, 34,  0,  8,  0, 24,  8,  8,  8, 28,  0,  4,  0, 12,  4,  4,  4, 36, 24, 32, 32, 34, 36, 56, 36, 34,  0,
  24,  8,  8,  8,  8,  8, 28,  0,  0,  0, 52, 42, 42, 42, 42,  0,  0,  0, 44, 50, 34, 34, 34,  0,  0,  0, 28, 34, 34, 34, 28,  0,
  0,  0, 44, 50, 50, 44, 32, 32,  0,  0, 26, 38, 38, 26,  2,  2,  0,  0, 44, 48, 32, 32, 32,  0,  0,  0, 30, 32, 28,  2, 60,  0,
  16, 16, 56, 16, 16, 18, 12,  0,  0,  0, 34, 34, 34, 38, 26,  0,  0,  0, 34, 34, 34, 20,  8,  0,  0,  0, 34, 42, 42, 42, 20,  0,
  0,  0, 34, 20,  8, 20, 34,  0,  0,  0, 34, 34, 38, 26,  2, 28,  0,  0, 62,  4,  8, 16, 62,  0,  6,  8,  8, 48,  8,  8,  6,  0,
  8,  8,  8,  0,  8,  8,  8,  0, 48,  8,  8,  6,  8,  8, 48,  0,  0,  0, 16, 42,  4,  0,  0,  0, 42, 20, 42, 20, 42, 20, 42,  0
};


// Procedure to set and display registers:
//   STCP low
//   For each address bit (high to low): SHCP low, write bit, SHCP high
//   STCP high to transfer to the storage register (and the output)
//
void writeAddresToShiftRegister(long val)
{
  digitalWrite(STCP_Pin, LOW);

  // From highest bit to lowest bit
  for (int i = 15; i >=  0; i--)
  {
    digitalWrite(SHCP_Pin, LOW);
    digitalWrite(SER_Pin, (val >> i) & 1);  // Write the next highest bit
    digitalWrite(SHCP_Pin, HIGH);  // Left shift the register 1 bit

  }
  digitalWrite(STCP_Pin, HIGH);
}


void printValue(int address, byte value)
{
  if (address % 32 == 0)
  {
    Serial.println("");
    Serial.print("$");
    if (address < 256) Serial.print("0");
    if (address < 16) Serial.print("0");
    Serial.print(address, HEX);
    Serial.print(":");
  }

  Serial.print(" ");
  if (value < 16) Serial.print("0");
  Serial.print(value, HEX);
}


// Procedure to write to the AT28 from data stored PROGMEM
//   Set OE and WE high
//   For each address, write the address and data, then pulse WE low
void writeEEPROM()
{
  Serial.println("");
  Serial.println("***** WRITE *****");
  AT28_enter_write_mode();

  // Set the data pins for output
  for (int i = 2; i <= 9; i++) pinMode(i, OUTPUT);

  for (int address = 0; address < ADDRESS_MAX; address++)
  {
    byte value = pgm_read_byte_near(values + address);
    printValue(address, value);

    writeAddresToShiftRegister(address);

    // set de data in data i/o pins
    // the (i + 2) translates from bit # to pin #
    for (int i = 0; i <= 7; i++) {
      digitalWrite(i + 2, (value & (1 << i)) ? HIGH : LOW);
    }

    // Send a pulse to the AT28 to write the data
    AT28_pulse_to_Write();
  }
}

void AT28_enter_write_mode() {
  digitalWrite(AT28_CE_Pin, LOW);
  digitalWrite(AT28_OE_Pin, HIGH);
}

void AT28_pulse_to_Write() {
  // Send a pulse to the AT28 to write the data
  digitalLow(AT28_WE_Pin);
  digitalHigh(AT28_WE_Pin);
  delay(10);
}

void writeEEPROM(int address, byte data) {
  Serial.println("");
  Serial.println("***** WRITE *****");

  writeAddresToShiftRegister(address);

  // set the data in I/O pins
  // the (i + 2) translates from bit # to pin #
  for (int i = 0; i <= 7; i++) {
    digitalWrite(i + 2, (data & (1 << i)) ? HIGH : LOW);
  }
  AT28_pulse_to_Write();
}

void AT28_pulse_to_Read() {
  // Set the AT28 for "READ", WE high, OE low
  // Set the data pins for input
  digitalWrite(AT28_CE_Pin, LOW);
  digitalWrite(AT28_WE_Pin, HIGH);
  digitalWrite(AT28_OE_Pin, LOW);
}

// Procedure to read from the AT28:
//   Set OE low and WE high
//   For each address, write the address to shift registers, then read the data and print
void readEEPROM()
{
  Serial.println("");
  Serial.println("***** READ *****");
  // Now read each address value and print out.
  
  AT28_pulse_to_Read();
  
  for (int i = 2; i <= 9; i++) pinMode(i, INPUT);

  for (int address = 0; address < ADDRESS_MAX; address++)
  {
    writeAddresToShiftRegister(address);
    int value = 0;
    // the (i + 2) translates from bit # to pin #
    for (int i = 0; i <= 7; i++) {
      if (digitalRead(i + 2)) value += (1 << i);
    }

    printValue(address, value);
  }

  Serial.println("");
}

byte readEEPROM(int address) {
  Serial.println("");
  Serial.println("***** READ *****");
  // Now read each address value and print out.
  
  AT28_pulse_to_Read();
  
  for (int i = 2; i <= 9; i++) pinMode(i, INPUT);

  writeAddresToShiftRegister(address);
  int value = 0;
  // the (i + 2) translates from bit # to pin #
  for (int i = 0; i <= 7; i++) {
    if (digitalRead(i + 2)) value += (1 << i);
  }
  return value;
}


void setup()
{
  Serial.begin(115200);

  // enabling output for shift registers
  pinMode(SER_Pin, OUTPUT);
  pinMode(STCP_Pin, OUTPUT);
  pinMode(SHCP_Pin, OUTPUT);

  pinMode(AT28_CE_Pin, OUTPUT);
  pinMode(AT28_WE_Pin, OUTPUT);
  pinMode(AT28_OE_Pin, OUTPUT);

  // Disable the AT28 for now
  digitalWrite(AT28_CE_Pin, HIGH);
  digitalWrite(AT28_WE_Pin, HIGH);
  digitalWrite(AT28_OE_Pin, HIGH);

  for (int i = 2; i <= 9; i++) pinMode(i, INPUT);

  // Wait until user hits return.
  Serial.println("***** Hit <RETURN> to start WRITE/READ *****");
  while (Serial.available() == 0) {};
  Serial.read();

  writeEEPROM();
  readEEPROM();
}


void loop()
{
  // Do nothing
}
