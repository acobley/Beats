
//1 Bass
//2 LT
//3 HT
//4 Snare
//8 Short cymbal

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
byte Patterns[3][16] = {{
    0x01  //P0
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
  { 0xA3   //p1
    , 0x80
    , 0x80
    , 0x85

    , 0x03
    , 0x80
    , 0x88
    , 0x85

    , 0x83
    , 0x80
    , 0x88
    , 0x84

    , 0x81
    , 0x81
    , 0x89
    , 0x83
  },
  { 0x83 //P2
    , 0x00
    , 0x80
    , 0x00

    , 0x88
    , 0x00
    , 0x80
    , 0x00

    , 0x83
    , 0x00
    , 0x80
    , 0x00

    , 0x0C
    , 0x82
    , 0x04
    , 0x82
  }

};


/*

*/

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

int GateTime = 50; //How long the gate will be high.

int n = 0;

void clockChange() {
  if (State == Running) {
    WritePattern(n);   // turn the LED on (HIGH is the voltage level)
    n++;
    if (n >= 8)
      n = 0;
  }
  if (digitalRead(reset)==LOW){
    n=0;
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
  WriteMux(n);
  delay(GateTime);// wait gatetime Ms (this will effect the max tempo
  clearGates();
  digitalWrite(playLED, LOW);
}



byte PatternMap[] = {1, 0, 2}; //Used to convert Switch positions to pattern number (center is 0 on the switch)

void getPatternNum() {
  byte SW1 = digitalRead(dirSW1);
  byte SW2 = digitalRead(dirSW2);
  byte rPattern = SW1 + 2 * SW2;
  Pattern = PatternMap[rPattern];
}

// the loop function runs over and over again forever

void loop() {
  // wait for a second
  readGateLength();
  getPatternNum();
  playButton();
}
