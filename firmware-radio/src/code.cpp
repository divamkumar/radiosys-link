#define TX_PIN 17
#define RX_PIN 16
#define ALERT_PIN 5
#define BTN_PIN 4

void setup() {
  Serial.begin(115200);           // Debug via USB
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // UART to Pi
  
  pinMode(ALERT_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP); // Or use your RC filter logic
  digitalWrite(ALERT_PIN, LOW);
}

void loop() {
  // 1. Send Heartbeat Protocol: [0xAA, CMD, LEN, PAYLOAD, CRC]
  byte packet[] = {0xAA, 0x01, 0x01, 0x00, 0xFF};
  Serial2.write(packet, 5);
  
  // 2. Check for Critical Alert
  if (digitalRead(BTN_PIN) == LOW) {
    digitalWrite(ALERT_PIN, HIGH);
    delay(50); // 50ms Pulse
    digitalWrite(ALERT_PIN, LOW);
    delay(500); // Debounce
  }
  
  delay(1000);
}

