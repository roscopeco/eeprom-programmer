#define CLOCK_PIN D1
#define DATA_PIN D0
#define WE_PIN D2
#define A10_PIN D3
#define A11_PIN D4
#define A12_PIN D5

/**
 * Shift out nBits from the given value in LSB-first order.
 */
void nShiftOut(uint8_t dataPin, uint8_t clockPin, uint32_t val, uint8_t nBits) {
  int i;
  
  for (i = 0; i < nBits; i++)  {
    digitalWrite(dataPin, !!(val & (1 << i)));
    
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);            
  }
}

inline uint32_t encode(uint8_t data, uint16_t address) {
  return data | ((address & 0x3FF) << 8);
}

void writeByte(uint16_t addr, uint8_t data) {
  // Pull W/E high (deassert)
  digitalWrite(WE_PIN, HIGH);
  
  // Shift out 18 bits to the shift register. This is 8 bits of data and 10 bits of address.
  nShiftOut(DATA_PIN, CLOCK_PIN, encode(data, addr), 18);

  // Set up the remaining address pins that don't fit in the register
  digitalWrite(A10_PIN, !!(addr & 0x400));
  digitalWrite(A11_PIN, !!(addr & 0x800));
  digitalWrite(A12_PIN, !!(addr & 0x1000));
  
  digitalWrite(WE_PIN, LOW);
  digitalWrite(WE_PIN, HIGH);

  // Wait for write cycle to complete; This is a long delay, but shorter ones resulted in inconsistent writes.
  //
  // Unfortunately, can't do the polling the datasheet suggests, because there's no disable line on the 74LS174s...
  delay(10);
}

void setup() {
  // W/E is active low
  digitalWrite(WE_PIN, HIGH);
  pinMode(WE_PIN, OUTPUT);

  pinMode(A10_PIN, OUTPUT);
  pinMode(A11_PIN, OUTPUT);
  pinMode(A12_PIN, OUTPUT);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  Serial.begin(115200);

  // Probably not necessary, but be sure the chip power-on write timeout has expired
  delay(5);
}

void loop() {
  static int addr;
  static byte val;

  if (addr < 8192) {
    Serial.printf("%04x: %d\n", addr, addr & 0xFF);
    writeByte(addr, 0x99/*addr & 0xFF*/);
    addr += 1;
  } else {
    Serial.println("Complete");
  }
}
