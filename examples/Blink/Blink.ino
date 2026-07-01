/*
 * Blink — Arc32 (THEJAS32) example
 * Blinks the on-board User LED (D20 / GPIO22 / blue LED, D3 on schematic)
 */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
