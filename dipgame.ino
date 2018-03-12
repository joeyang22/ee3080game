/*
 * DIP AY2017/2018 Sem 2 E004 Subgroup game - 'Question Game'
 */

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
  verifying,
  finished,
  orderQuestion,
  correct,
  incorrect
};

// Question answers
String questions[][2] = {{"(85-5-2)/6 = ___?", "Express in binary."}, {"NTUDIP = UTIPND", "123432 = ________?"}};
bool coverAnswers[] = {true, true, false, true};
int totalQuestions = 2;
int coverQuestions = 1;
int questionNum = 0;


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
int userInput[6];
int registered = 0;
int orderAnswers[] = {3,2,3,2,1,4};
bool needsReset = false;

/*
 * This function verifies whether or not the sensors being triggered match an expected array of inputs. 
 * For example, if the desired output is for player 1 and 3 to cover their sensors, but only player 1 is covering their sensor, the function will return False.
 * 
 * @param bool playersToVerify[] An array of booleans to signify which sensors should be covered in order from player 1 to player 4
 * 
 * @return True if the correct players are covering their sensors, and false otherwise
 */
bool verifyPlayer(bool playersToVerify[]) {
  // Check which players have inputted their answer
  bool player1Input = analogRead(player1) > 700;
  bool player2Input = analogRead(player2) > 700;
  bool player3Input = analogRead(player3) > 700;
  bool player4Input = analogRead(player4) > 700;
  bool inputs[] = {player1Input, player2Input, player3Input, player4Input};

  // Compares the input array to the verification set
  return compareArrays(inputs, playersToVerify);   
}

/*
 * Compares two arrays for element-wise equality.
 * 
 * @param int first[]  First array to compare.
 * @param int second[] Second array to compare.
 * 
 * @return True if the arrays have the same values, false otherwise.
 */
bool compareArrays(int first[], int second[]) {
  for (int i = 0; i < sizeof(first); i++){
    if (first[i]!=second[i]){
      return false;
    }
  }
  return true;
}

/*
 * Determines which single player is inputting a value by checking if one sensor is covered and the rest are not.
 * 
 * @return -2 if none of the inputs are being covered, -1 if an invalid value is returned, 
 *         and a number from {1,2,3,4} otherwise signifying which user is providing input.
 */
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
    return -2;
  }
  return -1;
}

/*
 * Plays a sound for success as well as lighting up the pin with the correct LED.
 */
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


/*
 * Plays a sound for failure as well as lighting up the pin with the correct LED.
 */
void playFailure() {
  digitalWrite(incorrectPin, HIGH);
  digitalWrite(speakerPin, HIGH);
  delay(100);
  digitalWrite(speakerPin, LOW);
  delay(400);
  digitalWrite(incorrectPin, LOW);
}

/*
 * Prints multiple lines on the LCD screen.
 * 
 * @param lines[] a 2D string array of the lines to print.
 */
void lcdPrintLines(String lines[]){
  for (int i =0; i < sizeof(lines); i++) {
    lcd.setCursor(0, i);
    lcd.print(lines[i]);
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize pins for output
  pinMode(laserPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(correctPin, OUTPUT);
  pinMode(incorrectPin, OUTPUT);
  //Initialize LCD
  lcd.begin(20, 4);
  lcd.clear();
  lcd.display();
  lcd.print("test");
  currentState = asking;
}

/*
 * Main loop of the game. The game functions as an FSM with 8 states
 */
void loop() {
  if (currentState == asking) {
    Serial.println("Asking");
    lcd.clear();
    String question[2] = questions[questionNum];
    lcdPrintLines(question);
    if (questionNum >= totalQuestions) {
      currentState = finished;
    } else if (questionNum < coverQuestions) {
       currentState = waiting; 
    } else {
      currentState = orderQuestion;
    }
    currentTime = timePerRound; 
  }
  else if (currentState == waiting) {
    lcd.setCursor(9, 4);
    currentTime = currentTime-1;
    if (currentTime == 0) {
      currentState = verifying;
    }
    lcd.print(currentTime); 
    delay(1000);
  }
  else if (currentState == finished) {
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
    currentState = asking;
  }
  else if(currentState == incorrect){
    lcd.clear();
    lcd.print("Incorrect!");
    playFailure();
    delay(2000);
    currentState = asking;
  }
  else if(currentState == orderQuestion){
    // Checks if the sequence has been pressed
    if (registered == 6){
      bool isSame = compareArrays(userInput, orderAnswers);
      Serial.println(isSame);
      if (isSame){
        currentState = correct;
      } else {
        currentState = incorrect;
      }
      registered = 0;
    } else 
    if (needsReset){
      if (getPlayerInput() == -2){
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
  else if (currentState == verifying) {
    Serial.println("verifying");
    boolean isSame = verifyPlayer(coverAnswers);
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

