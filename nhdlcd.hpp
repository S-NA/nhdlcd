#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

enum Level : unsigned char {
  OFF,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3,
  LEVEL_4,
  LEVEL_5,
  LEVEL_6,
  LEVEL_7,
  MAX
};

enum class Command : unsigned char {
  DisplayOn,
  DisplayOff,
  SetCursor /* takes param */
  ,
  CursorHome,
  UnderlineCursorOn,
  UnderlineCursorOff,
  MoveCursorLeftOnePlace,
  MoveCursorRightOnePlace,
  BlinkingCursorOn,
  BlinkingCursorOff,
  Backspace,
  ClearScreen,
  SetContrast /* takes param */
  ,
  SetBacklightBrightness /* takes param */
  ,
  LoadCustomCharacter /* takes params */
  ,
  MoveDisplayLeftOnePlace,
  MoveDisplayRightOnePlace,
  ChangeRS232BaudRate,
  ChangeI2CAddress,
  DisplayFirmwareVersionNumber,
  DisplayRS232BaudRate,
  DisplayI2CAddress
};

class nhdlcd {
  typedef const unsigned char pin_t;

private:
  pin_t data;
  pin_t clock;
  pin_t slaveSelect;
  unsigned charCount;

public:
  /**
   * Create a LCD serial communication object on specified pins.
   * Automatically calls setup() for you.
   * @param data pin
   * @param clock pin
   * @param slave select pin
   */
  nhdlcd(pin_t, pin_t, pin_t);
  /**
   * Explicitly called this function to set the pin modes.
   */
  void setup();
  /**
   * Write a character to the LCD, using the specified bit order.
   * MSB first by default.
   * @param character to write to LCD
   * @param bit order to use when writing
   */
  void write(unsigned char, int = MSBFIRST);
  /**
   * Write command to the LCD.
   * It is the programmer's responsibility that the command works without an
   * argument.
   * @param command to send to LCD
   */
  void write(Command);
  /**
   * Write command to the LCD.
   * Command will always write at least one argument to the LCD.
   * @param command to send to LCD
   * @param arguments for the command
   */
  void write(Command, int...);
  /**
   * Write string to the LCD.
   * @param c-style string to write to LCD
   */
  void write(const char *);

  /**
   * Scroll string from left to right, and right to left.
   */
  void hMarquee();
  /**
   * Blink the LCD.
   * @param how many times to blink the LCD
   * @param the level to blink to from LOW
   */
  void blink(unsigned char = 10, Level = MAX);
};
