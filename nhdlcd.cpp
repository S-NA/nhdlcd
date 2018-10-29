#include "nhdlcd.hpp"

unsigned char get_cmd_value(Command);
unsigned get_cmd_delay(Command);

nhdlcd::nhdlcd(pin_t datPin, pin_t clkPin, pin_t slsPin)
    : data{datPin}, clock{clkPin}, slaveSelect{slsPin}, charCount{0} {
  setup();
}

void nhdlcd::setup() {
  pinMode(data, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(slaveSelect, OUTPUT);
}

void nhdlcd::write(unsigned char ch, int bitOrder) {
  /* bitOrder uses MSBFIRST and LSBFIRST macros, of which are 1 and 0 */
  unsigned char compareValue = bitOrder ? 0x80U : 0x01U;

  /**
   * To all who read this, the comments below are needlessly detailed.
   * Why you ask, for myself when I forget what the fsck is SPI.
   */
  /* Before sending data, we set slave select line low which activates it. */
  digitalWrite(slaveSelect, LOW);

  // shift out data
  for (uint8_t i = 0U; i < 8U; ++i) {
    // send bit to data pin
    digitalWrite(data, (ch & compareValue) ? HIGH : LOW);

    /* Shift compare value, again this relies on the fact that the macros
     * MSBFIRST and LSBFIRST are defined to 1 and 0 respectively.
     */
    compareValue = bitOrder ? compareValue >> 1 : compareValue << 1;

    // trigger clk rising edge(toggle low then high)
    digitalWrite(clock, LOW);
    // wait before trigger
    delayMicroseconds(4U);
    digitalWrite(clock, HIGH);
    /* Due to the LCD we are using specifying a max 100KHz for SPI
     * wait some time so we can bit bang at such a frequency.
     */
    delayMicroseconds(14U);
  }
  /* Finished sending data, set slave select line high, disabling it. */
  digitalWrite(slaveSelect, HIGH);
  delayMicroseconds(110U); /* TODO: is this what the datasheet means? */
}

void nhdlcd::write(Command cmd) {
  unsigned delayTime = get_cmd_delay(cmd); /* in μs */
  if (cmd == Command::ClearScreen) charCount = 0;
  write(0xFEU);
  write(get_cmd_value(cmd));
  delayMicroseconds(delayTime);
}

void nhdlcd::write(Command cmd, int argv...) {
  unsigned delayTime = get_cmd_delay(cmd); /* in μs */
  va_list args;
  va_start(args, argv);
  
  uint8_t count = 1;
  if (cmd == Command::LoadCustomCharacter)
    count = 9;

  write(0xFE);
  write(get_cmd_value(cmd));
  for (uint8_t i = 0; i < count; ++i) {
    unsigned char ch = (count == 1) ? argv : va_arg(args, int);
    Serial.println(ch);
    write(ch);
  }
  va_end(args);

  delayMicroseconds(delayTime);
}

void nhdlcd::write(const char *str) {
  uint8_t i = 0;
  while (str[i]) {
    write(str[i++]);
  }
  ++charCount;
}

void nhdlcd::hMarquee() {
  for (unsigned i = 0; i < max(charCount, 16); ++i) {
    write(Command::MoveDisplayRightOnePlace);
    delay(50);
  }
  delay(100);
  for (unsigned i = 0; i < max(charCount, 16); ++i) {
    write(Command::MoveDisplayLeftOnePlace);
    delay(50);
  }
  delay(100);
}

void nhdlcd::blink(unsigned char reps, Level restore) {
  for (int i = 0; i < reps; ++i) {
    write(Command::SetBacklightBrightness, 1);
    delay(100);
    write(Command::SetBacklightBrightness, restore);
    delay(100);
  }
}

/**
 * Internal helper functions for the command interface.
 */
unsigned char get_cmd_value(Command cmd) {
  /* NHD-0216K3Z-FL-GBW-V3.pdf page 7 */
  static const unsigned char lookup[22] = {
      0x41, 0x42, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4E,
      0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x61, 0x62, 0x70, 0x71, 0x72};
  return lookup[static_cast<uint8_t>(cmd)];
}

unsigned get_cmd_delay(Command cmd) {
  switch (cmd) {
  case Command::CursorHome:
  case Command::UnderlineCursorOn:
  case Command::UnderlineCursorOff:
  case Command::ClearScreen:
    return 2000;
  case Command::SetContrast:
    return 501;
  case Command::LoadCustomCharacter:
    return 201;
  case Command::ChangeRS232BaudRate:
  case Command::ChangeI2CAddress:
    return 3500;
  case Command::DisplayFirmwareVersionNumber:
  case Command::DisplayI2CAddress:
    return 4500;
  case Command::DisplayRS232BaudRate:
    return 10500;
  default:
    return 110;
  }
}
