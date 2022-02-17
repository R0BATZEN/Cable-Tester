#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long interval = 10000;           // interval at which to test wiggle mode (milliseconds)
unsigned long previousMillis = 0;      // will store last time LED was updated

const int OUT1 = 6; // TIP / 2 / 1+
const int OUT2 = 5; // RING / 3 / 1-
const int OUT3 = 4; // SLEEVE / 1 / 2+
const int OUT4 = 3; // 2-

const int IN1 = 7; // TIP / 2 / 1+
const int IN2 = 8; // RING / 3 / 1-
const int IN3 = 9; // SLEEVE / 1 / 2+
const int IN4 = 10; // 2-

const int START_BUTTON = 2;

const int WIGGLE_SWITCH = A6;
const int NORMAL_SWITCH = A7;

const int MONO_SWITCH = A0;
//const int SPEAKON_SWITCH = A1;
const int NL2_SWITCH = A2;
const int NL4_SWITCH = A1;

const int XLR[4][4] = { {0, 1, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1} , {1, 1, 1, 1 } };
const int MONO[4][4] = { {0, 1, 1, 1}, {1, 0, 0, 1}, {1, 0, 0, 1}, {1, 1, 1, 1} };
const int NL2[4][4] = { {0, 1, 1, 1}, {1, 0, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1} };
const int NL4[4][4] = { {0, 1, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 0} };
//const int SPEAKON[4][4] = { {0, 1, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 0} };

int operatingMode; // 0 = wiggle, 1 = normal
int cableType;    // 3 = XLR/Stereo, 2 = Mono, 4 = NL4, 5 = NL2

int inputs[] = { IN1, IN2, IN3, IN4, WIGGLE_SWITCH, NORMAL_SWITCH, MONO_SWITCH, NL4_SWITCH, NL2_SWITCH, START_BUTTON};
int outputs[] = {OUT1, OUT2, OUT3, OUT4};

int pass = 1;

//========================================================================================================
//=== GET OPERATING MODE (WIGGLE OR NORMAL) ==============================================================
//========================================================================================================
int getOperatingMode() {

    int mode = 1;

    if (!digitalRead(WIGGLE_SWITCH)) {
        mode = 0;
    } else if (!digitalRead(NORMAL_SWITCH)) {
        mode = 1;
    } else {
    }
    return mode;
}

//========================================================================================================
//=== GET CABLE TYPE (MONO, XLR/STEREO, NL2, NL4) =========================================================
//========================================================================================================
int getCableType() {

    int cable;

    if (!digitalRead(MONO_SWITCH)) { //Mono
        cable = 2;
    } else if (!digitalRead(NL4_SWITCH)) { //NL4
        cable = 4;
    } else if (!digitalRead(NL2_SWITCH)){ //NL2
        cable = 5;
    }else {   //XLR
        cable = 3;
    }
    return cable;
}



//====================================================================================
//=== SETUP ==========================================================================
//====================================================================================
void setup() {

    Serial.begin(9600);
    Serial.println("Starting System");
    Serial.println("---------------------");

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextWrap(false);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.display();

    //=== set the pins as Inputs =======================================================
    for (int i = 0; i < (sizeof(inputs) / sizeof(inputs[0])); i++) {
        pinMode(inputs[i], INPUT_PULLUP);
    }

    //=== set the pins as output: =======================================================
    for (int i = 0; i < (sizeof(outputs) / sizeof(outputs[0])); i++) {
        pinMode(outputs[i], OUTPUT);
    }

    //=== get operating Mode ============================================================
    operatingMode = getOperatingMode();
}

void loop() {

    cableType = getCableType();

    display.clearDisplay();
    display.setTextSize(3);

    if (cableType == 2) {
        display.setCursor(28, 10);
        display.println("Mono");
        display.setTextSize(2);
        display.setCursor(27, 47);
        display.println("Klinke");
    } else if ( cableType == 3) {
        display.setCursor(11, 10);
        display.println("Stereo");
        display.setTextSize(2);
        display.setCursor(4, 47);
        display.println("XLR/Klinke");
    } else if (cableType == 4) {
        display.setCursor(1, 10);
        display.println("SpeakOn");
        display.setTextSize(2);
        display.setCursor(45, 47);
        display.println("NL4");
    } else if (cableType == 5) {
        display.setCursor(1, 10);
        display.println("SpeakOn");
        display.setTextSize(2);
        display.setCursor(45, 47);
        display.println("NL2");
    }

    display.display();

    if (!digitalRead(START_BUTTON)) {
        startTest();
        delay(2000);
    }
}

void startTest() {
    if (operatingMode == 0) {
        testWiggle();
    } else if (operatingMode == 1) {
        testNormal();
    }
    delay(500);
}

void testNormal() {

    int testResults[4][4];

    //=== set all outputs to high to prepare testing ============================
    for (int x = 0; x < 4; x++) {
        digitalWrite(outputs[x], HIGH);
    }

    //=== test all pins =========================================================
    for (int i = 0; i < 4; i++) {
        digitalWrite(outputs[i], LOW);
        delay(10);
        for (int j = 0; j < 4; j++) {
            testResults[i][j] = digitalRead(inputs[j]);
        }
        digitalWrite(outputs[i], HIGH);
    }

    //=== set all outputs to low to end testing =================================
    for (int x = 0; x < 4; x++) {
        digitalWrite(outputs[x], LOW);
    }

    //=== hand off results to be processed ======================================
    int result = checkResults(testResults);
    Serial.println(result);
    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(28, 21);
    if (result) {
        display.println("PASS");
    } else {
        display.println("FAIL");
    }
    display.display();
}

void testWiggle() {
    unsigned long startMillis = millis();

    int pass = 1;

    display.clearDisplay();
    display.setCursor(0, 15);
    display.setTextSize(3);
    display.println("WIGGLE");
    display.display();

    while (millis() - startMillis < interval && pass == 1) {

        int testResults[4][4];

        //=== set all outputs to high to prepare testing ============================
        for (int x = 0; x < 4; x++) {
            digitalWrite(outputs[x], HIGH);
        }

        //=== test all pins =========================================================
        for (int i = 0; i < 4; i++) {
            digitalWrite(outputs[i], LOW);
            delay(10);
            for (int j = 0; j < 4; j++) {
                testResults[i][j] = digitalRead(inputs[j]);
            }
            digitalWrite(outputs[i], HIGH);
        }

        //=== set all outputs to low to end testing =================================
        for (int x = 0; x < 4; x++) {
            digitalWrite(outputs[x], LOW);
        }

        pass = checkResults(testResults);
    }

    Serial.println(pass);
    display.clearDisplay();
    display.setCursor(0, 21);
    if (pass) {
        display.println("PASS");
    } else {
        display.println("FAIL");
    }
    display.display();
}

int checkResults(int pResults[4][4]) {
    pass = 1;
    cableType = getCableType();

    if (cableType == 2) { // Mono Cable
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pResults[i][j] != MONO[i][j]) {
                    pass = 0;
                }
            }
        }
    } else  if (cableType == 3) { // XLR/Stereo Cable
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pResults[i][j] != XLR[i][j]) {
                    pass = 0;
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
            }
        }
    } else  if (cableType == 4) { // SpeakOn NL4 Cable
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pResults[i][j] != NL4[i][j]) {
                    pass = 0;
                }
            }
        }
    }else  if (cableType == 5) { // SpeakOn NL2 Cable
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pResults[i][j] != NL2[i][j]) {
                    pass = 0;
                }
            }
        }
    }
    return pass;
}