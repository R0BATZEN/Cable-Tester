//Musicians Cable Tester (SCT)  20210822 by Brodafin
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
int Pass = 0;
int sensTRS = 10;
int sensTS = 11;
int sensMode = 12;
int tip1 = 4;
int ring1 = 5;
int sleeve1 = 6;
int tip2 = 7;
int ring2 = 8;
int sleeve2 = 9;
int TRS = 0;
volatile int testPressed = 0;
int cableType = 0;
int muX = 0;
int TmaskXLRTRS[ 3 ][ 3 ] = { {0, 1, 1}, {1, 0, 1}, {1, 1, 0} } ;
int TmaskTS[3][3] = { {0, 1, 1}, {1, 0, 0}, {1, 0, 0} };
int Tread[ 3 ][ 3 ] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } ;
String openStatus = "Open: T,R,S";
String shortStatus = "Shrt:TRTSRS";
char iDent[7] = {'0', '2', '3', '1', 'T', 'R', 'S',};
unsigned long timeout = 10000;
unsigned long interVal = 0;
void setup()   {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.display();
  attachInterrupt(0, testButtonPressed, HIGH);
  pinMode(sensTRS, INPUT_PULLUP);
  pinMode(sensTS, INPUT_PULLUP);
  pinMode(sensMode, INPUT_PULLUP);
  pinMode(tip1, OUTPUT);
  pinMode(ring1, OUTPUT);
  pinMode(sleeve1, OUTPUT);
  pinMode(tip2, INPUT_PULLUP);
  pinMode(ring2, INPUT_PULLUP);
  pinMode(sleeve2, INPUT_PULLUP);
}
void loop() {
  display.setTextSize(2);
  if (digitalRead(sensTRS) == HIGH && digitalRead(sensTS) == HIGH) {
    cableType = 1; muX = 1;
    display.clearDisplay();
    display.setCursor(14, 10); display.println("XLR Cable");
    display.display();
  }
  if (digitalRead(sensTS) == LOW) {
    cableType = 2;  muX = 4;
    display.clearDisplay(); display.setCursor(8, 10);
    display.println("Mono Cable");
    display.display();
  }
  if (digitalRead(sensTRS) == LOW) {
    cableType = 3; muX = 4;
    display.clearDisplay(); display.setCursor(0, 10);
    display.println("StereoCable");
    display.display();
  }
  if (testPressed == 1) {
    delay(50); testPressed = 0;
    if (digitalRead(sensMode) == 1 ) {
      clearStatusVars(); doWiggleTest(); testPressed = 0; checkResults();
    } else {
      doNormalTest();
      testPressed = 0;
      checkResults();
    }
  }
}
void doNormalTest() {
  display.clearDisplay(); display.setTextSize(3);
  display.setCursor(0, 8); display.println("TESTING");
  display.display();
  delay(100);
  pinMode(sleeve1, OUTPUT);
  digitalWrite(tip1, LOW);
  digitalWrite(ring1, HIGH);
  digitalWrite(sleeve1, HIGH); delay(1);
  Tread[0][0] = digitalRead(tip2);
  Tread[0][1] = digitalRead(ring2);
  Tread[0][2] = digitalRead(sleeve2); digitalWrite(tip1, HIGH);
  digitalWrite(ring1, LOW); delay(1);
  Tread[1][0] = digitalRead(tip2);
  Tread[1][1] = digitalRead(ring2);
  Tread[1][2] = digitalRead(sleeve2); digitalWrite(ring1, HIGH);
  digitalWrite(sleeve1, LOW); delay(1);
  Tread[2][0] = digitalRead(tip2);
  Tread[2][1] = digitalRead(ring2);
  Tread[2][2] = digitalRead(sleeve2); digitalWrite(sleeve1, HIGH);
}
void checkResults() {
  Pass = 1;
  if (digitalRead(sensMode) != 1 ) {
    clearStatusVars();
  }
  if (cableType == 2) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (Tread[i][j] != TmaskTS[i][j]) {
          Pass = 0;
          if (Tread[i][j] > TmaskTS[i][j]) {
            openStatus.setCharAt(i + 1 + 5, iDent[i + muX]);
          }
        }
      }
    }
    if (Tread[0][2] == 0 && Tread[2][0] == 0) {
      shortStatus.setCharAt(7, iDent[muX]);
      shortStatus.setCharAt(8, iDent[muX + 2]);
    }
  }  else {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (Tread[i][j] != TmaskXLRTRS[i][j]) {
          Pass = 0;
          if (Tread[i][j] > TmaskXLRTRS[i][j]) {
            openStatus.setCharAt(i * 2 + 5, iDent[i + muX]);
          }
        }
      }
    }
    if (Tread[0][1] == 0 || Tread[1][0] == 0) {
      shortStatus.setCharAt(5, iDent[muX]);
      shortStatus.setCharAt(6, iDent[muX + 1]);
    }
    if (Tread[0][2] == 0 || Tread[2][0] == 0) {
      shortStatus.setCharAt(7, iDent[muX]);
      shortStatus.setCharAt(8, iDent[muX + 2]);
    }
    if (Tread[1][2] == 0 || Tread[2][1] == 0) {
      shortStatus.setCharAt(9, iDent[muX + 1]);
      shortStatus.setCharAt(10, iDent[muX + 2]);
    }
  }
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(30, 8);
  if (Pass == 1) {
    display.print("PASS");
    display.display();
    delay(1000);
  } else {
    display.print("FAIL");
    display.display();
    delay(1000);
    showResults();
  }
}
void doWiggleTest() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 8);
  display.println("WIGGLE.");
  display.display();
  Pass = 1; testPressed = 0;
  interVal = millis();
  while (Pass == 1 && (millis() - interVal) < timeout) {
    pinMode(sleeve1, OUTPUT);
    digitalWrite(tip1, LOW);
    digitalWrite(ring1, HIGH);
    digitalWrite(sleeve1, HIGH);
    delay(1);
    Tread[0][0] = digitalRead(tip2);
    Tread[0][1] = digitalRead(ring2);
    Tread[0][2] = digitalRead(sleeve2);
    digitalWrite(tip1, HIGH);
    digitalWrite(ring1, LOW);
    delay(1);
    Tread[1][0] = digitalRead(tip2);
    Tread[1][1] = digitalRead(ring2);
    Tread[1][2] = digitalRead(sleeve2);
    digitalWrite(ring1, HIGH);
    digitalWrite(sleeve1, LOW);
    delay(1);
    Tread[2][0] = digitalRead(tip2);
    Tread[2][1] = digitalRead(ring2);
    Tread[2][2] = digitalRead(sleeve2);
    digitalWrite(sleeve1, HIGH);

    if (cableType == 2) {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          if (Tread[i][j] != TmaskTS[i][j]) {
            Pass = 0;
          }
        }
      }
    }  else {
      for (int i = 0; i < 3; i++) {
        if (Pass == 0) {
          continue;
        }
        for (int j = 0; j < 3; j++) {
          if (Pass == 0) {
            continue;
          }
          if (Tread[i][j] != TmaskXLRTRS[i][j]) {
            Pass = 0;
          }
        }
      }
    }
  }
}
void clearStatusVars() {
  for (int i = 5; i < 11; i++) {
    openStatus.setCharAt(i, 32);
  }
  for (int i = 5; i < 11; i++) {
    shortStatus.setCharAt(i, 32);
  }
}
void showResults() {
  testPressed = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(openStatus);
  display.setCursor(0, 18);
  display.print(shortStatus);
  display.display();
  while (testPressed != 1) {
    delay(10);
  }
  display.clearDisplay();
  display.display();
  delay(200);
  testPressed = 0;
}
void testButtonPressed() {
  testPressed = 1;
  delay(500);
}
