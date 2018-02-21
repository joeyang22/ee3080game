#include <LiquidCrystal.h>

// LCD Display constants
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize pin numbers
const int laserPin = 10;
const int correctPin = 6;
const int incorrectPin = 7;
const int speakerPin = 8;

// Enum to keep track of states (game operates as a FSM)
enum state {
  asking,
  waiting,
  displaying,
  finished,
  orderQuestion,
  correct,
  incorrect
};

// Game states
String questions[] = {"1+1 = 2?", "asdfasf"};
const int totalQuestions = 2;
int questionNum = 0;

// Question answers
bool question1[] = {true, true, true, true};
bool question2[] = {true, false, true, false};
bool question3[] = {false, true, true, true};


// Constants for time
const int timePerRound = 10;
const float timerInterval = 1000;
int currentTime = 10;

// Initialize current state
state currentState = orderQuestion;

// Initialize player 1
int player1 = A5;
int player2 = A3;
int player3 = A4;
int player4 = A2;

// User input for FSM type games
int userInput[4];
int registered = 0;
int order1[] = {1,2,3,4};
bool needsReset = false;

bool sameAnswer() {
  bool player1Input = analogRead(player1) > 500;
  bool player2Input = analogRead(player2) > 500;
  return (player1Input && player2Input) || (not player1Input && not player2Input);
}

bool verifyPlayer(bool playersToVerify[]) {
  // Check which players have inputted their answer
  bool player1Input = analogRead(player1) > 700;
  bool player2Input = analogRead(player2) > 700;
  bool player3Input = analogRead(player3) > 700;
  bool player4Input = analogRead(player4) > 700;
  bool inputs[] = {player1Input, player2Input, player3Input, player4Input};

  for (int i = 0; i < 4; i++) {
    if (inputs[i] != playersToVerify[i]) {
      return false;
    }
  }
  return true;   
}

bool compareArrays(int first[], int second[]) {
  for (int i = 0; i < sizeof(first); i++){
    Serial.println(first[i]);
    Serial.println(second[i]);
    if (first[i]!=second[i]){
      return false;
    }
  }
  return true;
}

int getPlayerInput(){
  bool player1Input = analogRead(player1) > 700;
  bool player2Input = analogRead(player2) > 700;
  bool player3Input = analogRead(player3) > 700;
  bool player4Input = analogRead(player4) > 700;
  if (player1Input && not (player2Input || player3Input || player4Input)) {
    return 1;
  } else if (player2Input && not (player1Input || player3Input || player4Input)) {
    return 2;
  } else if (player3Input && not (player1Input || player2Input || player4Input)) {
    return 3;
  } else if (player4Input && not (player1Input || player2Input || player3Input)) {
    return 4;
  } else if (not(player1Input || player2Input || player3Input || player4Input)) {
    return 5;
  }
  return -1;
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

void setup() {
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);

  pinMode(speakerPin, OUTPUT);
  pinMode(correctPin, OUTPUT);
  pinMode(incorrectPin, OUTPUT);
  lcd.begin(20, 4);
  lcd.clear();
  lcd.display();
  lcd.print("test");
  currentState = orderQuestion;
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
    delay(1000);
  }
  else if (currentState == finished) {
    Serial.println("YAY");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finished!");
    delay(2000);
  }
  else if(currentState == correct){
    lcd.clear();
    lcd.print("Correct!");
    questionNum+=1;
    playSuccess();
    if (questionNum >= totalQuestions) {
      currentState = finished;
    }
    delay(2000);
    currentState = orderQuestion;
  }
  else if(currentState == incorrect){
    lcd.clear();
    lcd.print("Incorrect!");
    questionNum = 0;
    playFailure();
    delay(2000);
    currentState = orderQuestion;
  }
  else if(currentState == orderQuestion){
    // Checks if the sequence has been pressed
    if (registered == 4){
      bool isSame = compareArrays(userInput, order1);
      Serial.println(isSame);
      if (isSame){
        currentState = correct;
      } else {
        currentState = incorrect;
      }
      registered = 0;
    } else 
    if (needsReset){
      if (getPlayerInput() == 5){
        needsReset = false;
      }
    } else {
      int input = getPlayerInput();
      if (input >0 && input <=4){
        userInput[registered] = input;
        lcd.setCursor(registered, 3);
        lcd.print(input);
        registered+=1;
        needsReset = true;
      }
    }
    delay(100);
  }
  else if (currentState == displaying) {
    Serial.println("Displaying");
    boolean isSame = verifyPlayer(question2);
    Serial.println(isSame);
    lcd.clear();
    lcd.setCursor(0,0);
    currentState = asking;
    if (isSame) {
      currentState = correct;
    } else {
      currentState = incorrect;
    }
    delay(50);
  }
}

