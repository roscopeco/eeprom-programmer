#define CLOCK_PIN D1
#define DATA_PIN D0

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

// address must be < 1024 (10 bits)!
inline uint32_t encode(uint8_t data, uint16_t address) {
  return data | (address << 8);
}


void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  static int i;
  static byte val;

  i = i + 1;

  if (i > 20000) {
    i = 0;
    nShiftOut(DATA_PIN, CLOCK_PIN, encode(val, 0x2AA), 18);
    Serial.printf("Outputting: %d\n", val++);
  }
}
