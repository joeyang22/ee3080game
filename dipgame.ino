#include <LiquidCrystal.h>

// LCD Display constants
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int laserPin = 10;

const int correctPin = 6;
const int incorrectPin = 7;
const int speakerPin = 8;


enum state {
  asking,
  waiting,
  displaying,
  finished
};

// Game states
String questions[] = {"1+1 = 2?", "asdfasf"};
const int totalQuestions = 2;
int questionNum = 0;

const int timePerRound = 10;
const float timerInterval = 1000;
int currentTime = 10;

state currentState = asking;

int player1 = A5;
int player2 = A4;

void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);

  pinMode(speakerPin, OUTPUT);
  pinMode(correctPin, OUTPUT);
  pinMode(incorrectPin, OUTPUT);
  lcd.begin(20, 4);
  lcd.clear();
  lcd.display();
}

boolean sameAnswer() {
  boolean player1Input = analogRead(player1) > 500;
  boolean player2Input = analogRead(player2) > 500;
  Serial.println(player1Input);
  return (player1Input && player2Input) || (not player1Input && not player2Input);
}

void playSuccess() {
  digitalWrite(correctPin, HIGH);
  for (int i = 0; i < 3; i+=1) {
    digitalWrite(speakerPin, HIGH);
    delay(100);
    digitalWrite(speakerPin, LOW);
    delay(500);
  }
  digitalWrite(correctPin, LOW);
}

void playFailure() {
  digitalWrite(incorrectPin, HIGH);
  digitalWrite(speakerPin, HIGH);
  delay(100);
  digitalWrite(speakerPin, LOW);
  delay(400);
  digitalWrite(incorrectPin, LOW);
}

void loop() {
  if (currentState == asking) {
    Serial.println("Asking");
    lcd.clear();
    String question = questions[questionNum];
    lcd.setCursor(0, 0);
    lcd.print(question);
    currentState = waiting;
    currentTime = timePerRound;
  }
  else if (currentState == waiting) {
    lcd.setCursor(9, 2);
    currentTime = currentTime-1;
    if (currentTime == 0) {
      currentState = displaying;
    }
    lcd.print(currentTime); 
  }
  else if (currentState == finished) {
    Serial.println("YAY");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finished!");
  }
  else if (currentState = displaying) {
    Serial.println("Displaying");
    boolean isSame = sameAnswer();
    lcd.clear();
    lcd.setCursor(0,0);
    currentState = asking;
    if (isSame) {
      lcd.print("Correct!");
      questionNum+=1;
      playSuccess();
      Serial.println(questionNum);
      Serial.println(totalQuestions);
      if (questionNum >= totalQuestions) {
        Serial.println("ASDF");
        currentState = finished;
      }

    } else {
      lcd.print("Incorrect!");
      questionNum = 0;
      playFailure();
    }

    delay(2000);
  }
  Serial.print("state: ");
  Serial.println(currentState);
  delay(1000);
}

