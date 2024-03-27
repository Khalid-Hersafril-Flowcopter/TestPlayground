#include <TimerOne.h>
// This example uses the timer interrupt to blink an LED
// and also demonstrates how to share a variable between
// the interrupt and the main program.


const int led = LED_BUILTIN;  // the pin with a LED
const int freq_output = 2;

// The main reason why I need to half this is because when you're sending the 
// Digital Output signals, you need to make sure that the 1s are hold 50% of the 
// duration! So, if you want 1 Hz, then for 0.5s you hold 1s, and another 0.5s, you hold 0
unsigned long time_us = 2e6;

void setup(void)
{
  pinMode(led, OUTPUT);
  pinMode(freq_output, OUTPUT);
  Timer1.initialize(time_us);
  Timer1.attachInterrupt(blinkLED); // blinkLED to run every 0.15 seconds
  Serial.begin(115200);
  digitalWrite(led, LOW);
  digitalWrite(freq_output, LOW);
}


// The interrupt will blink the LED, and keep
// track of how many times it has blinked.
int ledState = LOW;
volatile int ledStateSignal = 0;
volatile unsigned long blinkCount = 0; // use volatile for shared variables

void blinkLED(void)
{
  if (ledState == LOW) {
    ledState = HIGH;
    ledStateSignal = 1;
    blinkCount = blinkCount + 1;  // increase when LED turns on
  } else {
    ledState = LOW;
    ledStateSignal = 0;
  }
  digitalWrite(led, ledState);
  digitalWrite(freq_output, ledState);
}


// The main program will print the blink count
// to the Arduino Serial Monitor
unsigned long time_ms = time_us / 1000;
unsigned long tSample_ms = time_ms / 10;

unsigned long lastUpdate_ms = millis();
unsigned long change_freq_ms = 1000;

void loop(void)
{
  unsigned long blinkCopy;  // holds a copy of the blinkCount
  int ledStateSignalCopy;

  // to read a variable which the interrupt code writes, we
  // must temporarily disable interrupts, to be sure it will
  // not change while we are reading.  To minimize the time
  // with interrupts off, just quickly make a copy, and then
  // use the copy while allowing the interrupt to keep working.
  noInterrupts();
  blinkCopy = blinkCount;
  ledStateSignalCopy = ledStateSignal;
  interrupts();

  if (millis() - lastUpdate_ms > change_freq_ms) {
    time_us = time_us / 2;
    time_ms = time_us / 1000;
    tSample_ms = time_ms / 10;
    lastUpdate_ms = millis();
    

    if (time_us < 50) {
      time_us = 2000000;
      time_ms = time_us / 1000;
      tSample_ms = time_ms / 10;
      lastUpdate_ms = millis();
    }

    Timer1.setPeriod(time_us);
  }

  Serial.print("blinkCount = ");
  // Serial.print(time_us);
  // Serial.print(" : ");
  Serial.println(ledStateSignalCopy);

  delay(tSample_ms);
}
