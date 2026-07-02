/*
 * I2C_Scanner — Arc32 (THEJAS32) example
 * Scans I2C0 bus (J3 header: SCL/SDA) for connected devices.
 */

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(200);
  Serial.println("Arc32 I2C0 Scanner");
}

void loop() {
  byte count = 0;
  Serial.println("Scanning...");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte err = Wire.endTransmission();

    if (err == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, 16);
      count++;
    }
  }

  if (count == 0) Serial.println("No I2C devices found.");
  Serial.println("Done.\n");
  delay(3000);
}
