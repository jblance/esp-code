#include <TM1638lite.h>

// I/O pins on the Arduino connected to strobe, clock, data
// (power should go to 5v and GND)
TM1638lite tm(4, 7, 8);
uint8_t state = 0;
uint8_t desired = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  tm.reset();
  tm.displayText("Eh");
  //tm.setLED(0, 1);
  
  delay(2000);

  tm.displayASCII(6, 'u');
  tm.displayASCII(7, 'p');
  //tm.setLED(7, 1);

  delay(2000);

  tm.displayHex(0, 8);
  tm.displayHex(1, 9);
  tm.displayHex(2, 10);
  tm.displayHex(3, 11);
  tm.displayHex(4, 12);
  tm.displayHex(5, 13);
  tm.displayHex(6, 14);
  tm.displayHex(7, 15);

  delay(2000);

  
  tm.displayText("buttons");
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(200);  
  uint8_t buttons = tm.readButtons();
  delay(100);
  uint8_t buttons2 = tm.readButtons();
  // debounce buttons
  while (buttons == buttons2) {
    delay(100);
    buttons2 = tm.readButtons();
  }
  desired = state ^ buttons;
  doLEDs(desired);
  state = desired;
}

// scans the individual bits of value
void doLEDs(uint8_t value) {
  for (uint8_t position = 0; position < 8; position++) {
    tm.setLED(position, value & 1);
    value = value >> 1;
  }
}
