
//1 Bass
//2 LT
//3 HT
//4 Snare
//8 Short cymbal

const byte clockIn = 2;
const byte GateKnob = A7;
const byte playLED = 5;
const byte reset = 3;
const byte dirSW1 = 6;
const byte dirSW2 = 7;

const byte Muxs[] = {11, 12, 13};


const byte Gates[] = {A0, A1, A2, A3, A4, A5, 8, 9};
byte Pattern = 0;
byte Patterns[2][16] = {{
    0x01
    , 0xC0
    , 0x08
    , 0x94

    , 0x01
    , 0xA0
    , 0x08
    , 0x84

    , 0x01
    , 0xc0
    , 0x08
    , 0x84

    , 0x01
    , 0xA0
    , 0x89
    , 0x92
  },
  { 0x01
    , 0x02
    , 0x04
    , 0x08

    , 0x10
    , 0x20
    , 0x40
    , 0x80

    , 0x01
    , 0x02
    , 0x04
    , 0x08

    , 0x10
    , 0x20
    , 0x40
    , 0x80
  }

};


/*
   {0x01
                ,0xC0
                ,0x08
                ,0x94

                ,0x01
                ,0xA0
                ,0x08
                ,0x84

                ,0x01
                ,0xc0
                ,0x08
                ,0x84

                ,0x01
                ,0xA0
                ,0x89
                ,0x92}
*/

int GateTime = 50; //How long the gate will be high.

int n = 0;

void clockChange() {
  WritePattern(n);   // turn the LED on (HIGH is the voltage level)
  n++;
  if (n >= 8)
    n = 0;

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

/***  Debug variables
 *   
 */

//int MuxCount=0;

void WriteMux(int n) {
//Debug
/*
clearMuxs();
digitalWrite(Muxs[MuxCount],HIGH);
MuxCount++;
if (MuxCount >=3)
   MuxCount=0;
//endDebug

*/
  for (int i = 0; i < 3; i++) {
    digitalWrite(Muxs[i], (n & 0x01));
    n >> 0x1;
  }
  
}




void readGateLength() {
  GateTime = (1024 - analogRead(GateKnob)) / 16; //0-> 125ms

}

void WritePattern(int n) {
  byte Beats = Patterns[Pattern][n];
  clearGates();
  digitalWrite(playLED, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(Gates[i], (Beats & 0x01));
    Beats = Beats >> 0x1;
  }
  WriteMux(n);
  delay(GateTime);// wait gatetime Ms (this will effect the max tempo
  clearGates();
  digitalWrite(playLED, LOW);
}

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

void getPatternNum() {
  if (digitalRead(dirSW2) == HIGH) {
    digitalWrite(Gates[7],HIGH);
    Pattern = 0;
  } else {
    Pattern = 1;
     digitalWrite(Gates[7],LOW);
  }
  if (digitalRead(dirSW1) == HIGH) {
    digitalWrite(Gates[0],HIGH);
    
  } else {
    
     digitalWrite(Gates[0],LOW);
  }
  
}

// the loop function runs over and over again forever

void loop() {
  // wait for a second
  readGateLength();
  getPatternNum();
}
