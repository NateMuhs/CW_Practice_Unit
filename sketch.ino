#include <LiquidCrystal.h>

const int KEY = 8;         // Morse key input (normally HIGH, pressed = LOW)
const int BUZZER = 9;      // Buzzer output
const int AUX_TIP = 10;    // AUX trip
const int AUX_RING = 11;   // AUX input ring

// LCD pins: RS, E, D4, D5, D6, D7
const int LCD_RS = 2, LCD_E = 3, LCD_D4 = 4, LCD_D5 = 5, LCD_D6 = 6, LCD_D7 = 7;
const int NUM_COLUMN = 16;
const int NUM_ROW = 2;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Constants
const int buzzerVol = 10;  // Buzzer volume (0-255) for PWM (if needed later)

// --- State ---
int part = 0;
bool prompted = false;
bool notDisplayingText = true;

void setup() {
  Serial.begin(115200);
  
  // Set pin modes
  pinMode(BUZZER, OUTPUT);
  pinMode(KEY, INPUT_PULLUP);
  pinMode(AUX_TIP, INPUT_PULLUP);
  pinMode(AUX_RING, INPUT_PULLUP);
  
  // Initialize LCD
  lcd.begin(NUM_COLUMN, NUM_ROW);
}

void loop() {
  switch (part % 5) {
    case 0: testLCD(); break;
    case 1: testBuzzer(); break;
    case 2: testInput("Button", KEY); break;
    case 3: testInput("Dit trigger (left on the paddle)", AUX_TIP); break;
    case 4: testInput("Dah trigger (right on the paddle)", AUX_RING); break;
  }

  readSerial();
}

// --- Test modules ---
void testLCD() {
  if (!prompted) {
    lcd.print("Hello world!");
    promptResponse("LCD");
  }
}

void testBuzzer() {
  if (!prompted) {
    promptResponse("Buzzer");
  }

  if (millis() % 2000 < 1000) {
    analogWrite(BUZZER, buzzerVol);
  } 
  else {
    analogWrite(BUZZER, 0);
  }
}

void testInput(const String& label, int pin) {
  if (!prompted) {
    promptResponse(label);
  }

  if (digitalRead(pin) == LOW && notDisplayingText) {
    lcd.print("It worked!");
    notDisplayingText = false;
  }
}

// --- Prompt handler ---
void promptResponse(const String& component) {
  if (part % 5 == 0) {
    Serial.println("Does the LCD display \"Hello world!\"? (yes/no)");
  } else if (part % 5 == 1) {
    Serial.println("Does the buzzer beep for about a second then turn off? (yes/no)");
  } else {
    Serial.println("Does the LCD display \"It worked!\" when you push the " + component + "? (yes/no)");
  }
  prompted = true;
}

// --- Serial interaction ---
void readSerial() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    Serial.print("User: ");
    Serial.println(input);

    if (input.equalsIgnoreCase("yes")) {
      Serial.println("Great! Moving to next test.\n");
      resetBoard();
    } else if (input.equalsIgnoreCase("no")) {
      Serial.println("Please check your wiring and test the " + String(part % 5) + " component again.\n");
    } else {
      Serial.println("Please enter 'yes' or 'no'.\n");
    }
  }
}

// --- State reset between parts ---
void resetBoard() {
  part++;
  prompted = false;
  notDisplayingText = true;
  lcd.clear();
  analogWrite(BUZZER, 0);
}