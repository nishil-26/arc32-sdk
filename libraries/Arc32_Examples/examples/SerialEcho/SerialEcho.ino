/*
 * SerialEcho — Arc32 (THEJAS32) example
 * Echoes back anything typed in Serial Monitor over UART0 (USB/CP2102N)
 */

void setup() {
  Serial.begin(115200);
  Serial.println("Arc32 (THEJAS32 RISC-V) ready.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    Serial.print("Echo: ");
    Serial.println(c);
  }
}
