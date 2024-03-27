unsigned long startTime_us = micros();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

int pinState = LOW;
float freq = 0;

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long lastTime_us = micros();

  unsigned long hold_us = lastTime_us - startTime_us;


  if (hold_us >= 0.01e6 && pinState == LOW) {
    startTime_us = micros();
    pinState = HIGH;
    freq = 1 / (hold_us * 1e-6);
  } else if (hold_us >= 0.01e6 && pinState == HIGH) {
    startTime_us = micros();
    pinState = LOW;
    freq = 1 / (hold_us * 1e-6);
  }

  analogWrite(3, pinState);

  Serial.print("Frequency: ");
  Serial.print(hold_us);
  Serial.print(" ");
  Serial.println(freq);
}
