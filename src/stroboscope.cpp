#include <Arduino.h>

#include "stroboscope.h"

#include <SoftwareSerial.h>

SoftwareSerial lcd(5,6);

#define LED 4
#define MIN_HERTZ 0
#define MAX_HERTZ 32

#define ENCODER_1 2
#define ENCODER_2 3

volatile int strobeHz = 1; // initially run at one pulse per second
volatile int strobeDuration = 250; // pulse the strobe for n milliseconds
volatile int strobeGap = 750; // gap between pulses in milliseconds

volatile int lastEncoded = 0;

void setup() {
  lcd.begin(9600);
  lcdClear();
  lcd.print("Stroboscope!");
  pinMode(LED, OUTPUT);

  pinMode(ENCODER_1, INPUT_PULLUP);
  pinMode(ENCODER_2, INPUT_PULLUP);

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  
  setStrobeHz(strobeHz);
  delay(1000);
}

void loop() {
  pulse(strobeDuration);
  delay(strobeGap);
}

void lcdClear() {
  lcd.write(0xFE);
  lcd.write(0x01);
}

void lcdSetCursor(byte position) {
  lcd.write(0xFE);
  lcd.write(0x80);
  lcd.write(position);
}

void pulse(int duration) {
  digitalWrite(LED, HIGH);
  delay(duration);
  digitalWrite(LED, LOW);
}

int strobePulseWidth(int hertz) {
  if (hertz <= MIN_HERTZ) {
    return MIN_HERTZ;
  } else {
    return (1000/hertz);
  }
}

int strobePulseDuration(int pulseWidth) {
  if (pulseWidth == 0) {
    return 0;
  } else {
    return pulseWidth/4;
  }
}

void setStrobeParams() {
  lcdSetCursor(16);
  lcd.print(strobeHz);
  lcd.print("Hz");
  strobeDuration = strobePulseDuration(strobePulseWidth(strobeHz));
  strobeGap = strobePulseWidth(strobeHz) - strobeDuration;
}


void setStrobeHz(int hertz) {
  if (hertz >= MIN_HERTZ && hertz <= MAX_HERTZ) {
    strobeHz = hertz;
    setStrobeParams();
  }
}

void updateEncoder(){
  int MSB = digitalRead(ENCODER_1); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_2); //LSB = least significant bit
  int current = strobeHz;
  
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) current++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) current--;
  
  setStrobeHz(current);
}
