## nhdlcd
  An Arduino library to interface with the NHD-0216K3Z-FL-GBW-V3.

  The library is implemented to use SPI as the serial interface, although I2C
  would likely be a better choice if my understanding is correct.

  By the nature of using a software emulated SPI (bit banging) any pins can be
  used to communicate with the LCD. However, hardware SPI uses pins 13, 12 and
  11. Thus I recommend those pins.

Installation:

  Change directory (cd) into

  (OS X) `~/Documents/Arduino/libraries`

  (Linux) `~/Arduino/libraries`

  (Windows) `C:\Users\%USERNAME%\My Documents\Arduino\libraries`

  Then do

  `git clone https://github.com/S-NA/nhdlcd`

Usage:
  ```cpp
  #include <nhdlcd.hpp>
  const byte dataPin = 13;
  const byte clockPin = 12;
  const byte slaveSelectPin = 11;
  nhdlcd lcd(dataPin, clockPin, slaveSelectPin);
  void setup() { }
  void loop() {
    lcd.write(Command::ClearScreen);
    lcd.write("Hello World");
    delay(100);
  }
  ```
