
//1 Bass
//2 LT
//3 HT
//4 Snare
//8 Short cymbal

#include "inc/BeatPatterns.h"

const byte clockIn = 2;
const byte GateKnob = A7;
const byte playLED = 5;
const byte playBut = 4;
const byte reset = 3;
const byte dirSW1 = 6;
const byte dirSW2 = 7;



const byte Muxs[] = {11, 12, 13};
const byte Running = 1;
const byte Stopped = 2;

byte State = Running;

const byte Gates[] = {A0, A1, A2, A3, A4, A5, 8, 9};
byte Pattern = 0;
byte SeqCount = 0;
byte PatternMap[] = {1, 0, 2}; //Used to convert Switch positions to pattern number (center is 0 on the switch)
byte rPattern = 0;


byte States[] = {false, false, false, false, false, false, false, false};
// the setup function runs once when you press reset or power the board
void setup() {
   
  int i = 0;
  for (i = 0; i < 8; i++) {
    pinMode(Gates[i], OUTPUT);
  }
  pinMode(reset, INPUT);
  pinMode(GateKnob, INPUT);
  pinMode(playLED, OUTPUT);
  pinMode(dirSW1, INPUT);
  pinMode(dirSW2, INPUT);
  pinMode(playBut, INPUT);
  pinMode(reset, INPUT);

  //flash(15);
  
  for (int i = 0; i < 8; i++) {
    States[i] = !States[i];
    digitalWrite(Gates[i], States[i]);
    States[i] = !States[i];
    delay(200);
    digitalWrite(Gates[i], States[i]);
    delay(200);
  }
  //Serial.begin(9600);
  pinMode(clockIn, INPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(Muxs[i], OUTPUT);
  }

  attachInterrupt(digitalPinToInterrupt(clockIn), clockChange, RISING);
}


void intDelay(unsigned long Time){
  interrupts();
  unsigned long startTime=micros();
  while(micros()-startTime<Time*1000);
}

void flash (int n) {
  
  
  for (int i = 0; i < n; i++) {
    digitalWrite(playLED, HIGH);
    intDelay(200);
    digitalWrite(playLED, LOW);
    intDelay(200);
  }
  
}

int GateTime = 50; //How long the gate will be high.

int n = 0;

int getPatternNum() {
  byte SW1 = digitalRead(dirSW1);
  byte SW2 = digitalRead(dirSW2);


  return (SW1 + 2 * SW2);

}
void CheckNextPattern() {
  int rPattern = getPatternNum();
  if (rPattern > 0) {
    Pattern = rPattern;
  } else {
    Pattern = Seq[SeqCount];
  }

  
}

void clockChange() {
  //flash(Pattern+1);
  if (State == Running) {
    WritePattern(n);
    n++;
    if (n >= 16){
       n = 0;

       SeqCount++;
       if (SeqCount > 3)
         SeqCount = 0;
       CheckNextPattern();
    }
  }
  if (digitalRead(reset) == LOW) {
    n = 0;
    CheckNextPattern();
    SeqCount = 0;
  }
}

void clearGates() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(Gates[i], LOW);

  }
}

void clearMuxs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(Muxs[i], LOW);

  }
}


void WriteMux(int n) {

  for (int i = 0; i < 3; i++) {
    digitalWrite(Muxs[i], (n & 0x01));
    n >> 0x1;
  }

}


void readGateLength() {
  GateTime = (1024 - analogRead(GateKnob));

}

int lastReading = LOW;
void playButton() {
  int reading = digitalRead(playBut);
  if (reading != lastReading) {
    if (reading == HIGH) {
      if (State == Running) {
        State = Stopped;
      } else {
        State = Running;
      }

    }
  }
  lastReading = reading;
}

void WritePattern(int n) {
  byte Beats = Patterns[Pattern][n];
  clearGates();
  digitalWrite(playLED, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(Gates[i], (Beats & 0x01));
    Beats = Beats >> 0x1;
  }
  WriteMux(n & 0x07);
  interrupts(); //the delay function won't work in an interrupt unless interrupts are enabled.
  delay(GateTime);// wait gatetime Ms (this will effect the max tempo
  noInterrupts();
  clearGates();
  digitalWrite(playLED, LOW);
}


// the loop function runs over and over again forever

void loop() {
  // wait for a second
  readGateLength();

  playButton();
}
