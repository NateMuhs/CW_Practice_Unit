#include <LiquidCrystal.h>

#define KEY 8  // Morse key from pin 7 to GND
#define BUZZER 9   // Buzzer key from pin 8 to GND
#define BUTTON_UP 10
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 13

// Length and width of the LCD
#define NUM_COLUMN 16
#define NUM_ROW 2

// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  pinMode(KEY, INPUT_PULLUP); // Set Morse key to input
  pinMode(BUZZER, OUTPUT); // Set Buzzer to output
  Serial.begin(115200); // Sets the speed of comunications with serial
  lcd.begin(NUM_COLUMN, NUM_ROW);   // Sets the size of the LCD being used

  //Buttons
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
}

//Values to be eddited later in development
int buzzerVol = 10; // The volumte of buzzer used to produce sound 0-255

float dashDuration = 200.0;  // The duration of one dash in morse code
boolean previousInput = false;  // The previous input that was run through the loop
long startTimeRunning, startTimePaused;  // Used to record the start and end times of the botton
boolean input;
String car = ""; // Not to sure yet
void loop() {
  input = !digitalRead(KEY); // Set input to high if pushed - low if unpushed

  if (input) {
    if (input != previousInput) {
      startTimeRunning = millis();  // Records the time of when to button was pressed
      DecodeerPauze(startTimePaused); // pass the time
    }
    analogWrite(BUZZER, buzzerVol);
  }
  else {
    if (input != previousInput) {
      startTimePaused = millis(); // Start the paused timer
      Decodeer(startTimeRunning);
    }
    digitalWrite(BUZZER, LOW);
  }

  if (abs(millis() - startTimePaused) > dashDuration * 10) { // Start a new word line
    DecodeerPauze(startTimePaused);
  }

  previousInput = input;
}

void Decodeer(long startTime) {
  char Symbol = '?';
  long time = abs(millis() - startTime); // Duration of the transmitted symbol (a dot or a dash)"
  float dotDuration = dashDuration / 3.0;

  if (time <= 2) return; // Debouncing

  if (time <= dotDuration) Symbol = '.';
  else if (time > dashDuration) Symbol = '-';
  else if ((time > (dashDuration + dotDuration) / 1.9) && time <= dashDuration) Symbol = '-';
  else Symbol = '.';

  if (Symbol == '-') {
    if (time > dashDuration) dashDuration++;
    if (time < dashDuration) dashDuration--;
  }
  else if (Symbol == '.') {
    if (time > dashDuration / 3.0) dashDuration++;
    if (time < dashDuration / 3.0) dashDuration--;
  }
  car += Symbol;
  //Serial.println(Symbol);
}

void DecodeerPauze(long startTime) {
  if (car == "") return;

  char Symbol = '?';
  long time = abs(millis() - startTime);
  if (time > dashDuration - dashDuration / 40) {
    Decodeercar();
    //Serial.print();
  }
  if (time > dashDuration * 2) {
    Decodeercar();
    Print(" "); Serial.print(" ");
  }
  //else Decodeercar();
  //Serial.println(Symbol);
}

void Decodeercar() {
  static String letters[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
    ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "E"
  };
  int i = 0;
  while (letters[i] != "E") {
    if (letters[i] == car) {
      Print((char)('A' + i));
      Serial.print((char)('A' + i));
      break;
    }
    i++;
  }
  if (letters[i] == "E") {
    Print(car);
    Serial.print(car);
  }
  car = "";
}

int y = 0, x = 0;
void Print(String & s) {
  for (int i = 0; i < s.length(); i++) {
    upd(); lcd.print(s[i]);
  }
}
void Print(char s) {
  upd();
  lcd.print(s);
}
void Print(char * s) {
  for (int i = 0; i < strlen(s); i++) {
    upd();
    lcd.print(s[i]);
  }
}

void upd() {
  lcd.setCursor(x, y);
  x++; if (x >= NUM_COLUMN) {
    x = 0;
    y++;
  }
  if (y >= NUM_ROW) {
    lcd.clear();
    x=y = 0;    
  }
}

