#include <LiquidCrystal.h>
LiquidCrystal lcd(12,A1,A2,A3,A4,A5);

const int switch1 = 7;
const int switch2 = 8;
const int switch3 = 9;
const int switch4 = 10;

const int led1 = 2;
const int led2 = 3;
const int led3 = 4;
const int led4 = 5;

const int sound = 6;

// difference between first led and first switch
const int diff = 5;

int state1;
int state2;
int state3;
int state4;

int led;
int averageTime;
int score;
int time1;
int gMode;
const int max_rounds=20;
int gameArray[max_rounds];
int numLights;
int notes[] = {262, 294, 330, 349};
bool silent;

void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT);
  
  lcd.print("Welcome!!");
  Serial.println("Welcome!!!! ");
  randomSeed(analogRead(A0));
  delay(3000);
  allowNoise();
}

void loop() {
  
  selectMode();
  delay(1000);
  if (gMode == 1){
    countdownLoop();
    reactionLoop();
  }else{
    memoryLoop();
  }
  endLoop();
  delay(2000);
}

void allowNoise(){
  lcd.setCursor(0,0);
  lcd.print("Press Blue");
  lcd.setCursor(2,1);
  lcd.print("For Sound");
  int push = getButtonPushed();
  digitalWrite(push-diff, HIGH);
  if (push == switch4){
    silent = false;
  }else{
    silent = true;
  }
  buttonUnpressed();
  digitalWrite(push-diff, LOW);
  delay(200);
}


void selectMode(){
  Serial.println("Select mode");
  Serial.println("Red: Reaction Speed");
  Serial.println("Yellow: Memory");
  lcd.clear();
  lcd.home();
  lcd.print("Red: Reaction");
  lcd.setCursor(0,1);
  lcd.print("Yellow: Memory");
  int push;
  push = getButtonPushed();
  digitalWrite(push-diff, HIGH);
  if (push == switch1){
    gMode = 1;
    buttonUnpressed();
    digitalWrite(push-diff, LOW);
    delay(1000);
    numLightsSelect();
  }else{
    gMode = 2;
    buttonUnpressed();
    digitalWrite(push-diff, LOW);
  }
  
}

void numLightsSelect(){
  Serial.println("\nSelect number of lights");
  Serial.println("Red: 1 light");
  Serial.println("Yellow: 2 lights");
  Serial.println("Green: 3 lights");
  Serial.println("Blue: 4 lights");

  lcd.clear();
  lcd.home();
  lcd.print("Number Lights");
  lcd.setCursor(0,1);
  lcd.print("Red:1 - Blue:4");
  
  int push = getButtonPushed();
  numLights = push - 1 - diff;
  digitalWrite(push-diff, HIGH);
  buttonUnpressed();
  digitalWrite(push-diff, LOW);
}

void countdownLoop(){
  Serial.print("Game mode ");
  Serial.print(gMode);
  Serial.println(" selected!");
  Serial.println("\nStarting in 3 seconds!\n");  
  lcd.clear();
  lcd.home();
  lcd.print("Reaction Mode");
  lcd.setCursor(0,1);
  lcd.print("Get Ready!!");
  setLights(HIGH, LOW, LOW, LOW);
  delay(1000);
  setLights(LOW, HIGH, LOW, LOW);
  delay(1000);
  setLights(LOW, LOW, HIGH, LOW);
  delay(1000);
  setLights(LOW, LOW, LOW, HIGH);
  delay(1000);
  setLights(LOW, LOW, LOW, LOW);
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("Begin!!");
}

void reactionLoop(){
  int time1;
  int ave = 0;
  int loops = 10;
  int upper = 2 + numLights;
  for (int i=0;i<loops;i++){
    int led = (int) random(2,upper);
    time1 = millis();
    digitalWrite(led, HIGH);
    while (digitalRead(led+5) != HIGH){}
    ave = ave + millis() - time1;
    digitalWrite(led, LOW);
    delay(2000 + random(-500,1000));
  }
  ave = (ave/loops);
  Serial.print("For ");
  Serial.print(gMode);
  Serial.print(" lights, your average is: ");
  Serial.print(ave);
  Serial.println(" milliseconds");
  lcd.clear();
  lcd.home();
  lcd.print("Your average is");
  lcd.setCursor(0,1);
  lcd.print(ave);
  lcd.print(" millis!");
}

void memoryLoop(){
  score = 0;
  bool lost = false;
  lcd.home();
  lcd.clear();
  lcd.print("Score: ");
  lcd.print(score);
  for (int i=0;i<max_rounds;i++){

    // Show the sequence
    gameArray[i] = random(2,6);
    for (int j=0;j<=i;j++){
      digitalWrite(gameArray[j], HIGH);
      if (! silent){
        tone(sound, notes[gameArray[j]-2]);  
        delay(500);   
        noTone(sound);
      }else{
        delay(500);
      }
      digitalWrite(gameArray[j], LOW);
      delay(50);
    }

    // Get the user to repeat the sequence
    for (int j=0;j<=i;j++){
      int pushed = getButtonPushed()-diff;
      if (! silent){
        tone(sound, notes[pushed-2]); 
      }
      digitalWrite(pushed, HIGH);
      buttonUnpressed();
      if (! silent){
        noTone(sound);
      }
      delay(50);
      digitalWrite(pushed, LOW);
      if (pushed != gameArray[j]){
        lcd.clear();
        lcd.home();
        lcd.print("You lost :(");
        lcd.setCursor(0,1);
        lcd.print("Final Score: ");
        lcd.print(score);
        Serial.println("You lost :(");
        Serial.print("Your score was: ");
        Serial.println(score);
        lost = true;
        break;
      }
    }
    if (lost){
      break;
    }
    score++;
    lcd.setCursor(7,0);
    lcd.print(score);
    delay(500);
  }
  if (! lost){
    Serial.println("You are a genius! Congratulations!!");
  }
  
}
void endLoop(){
  Serial.println("\nHold the red light button to restart");
  unsigned long t = millis();
  bool printed = true;
  while (digitalRead(switch1) != HIGH){
    if (millis()- t > 5000 && printed){
      lcd.clear();
      lcd.home();
      lcd.print("Hold red");
      lcd.setCursor(2,1);
      lcd.print("to restart!");
      printed = false;
    }
    setLights(HIGH,HIGH,HIGH,HIGH);
    delay(250);
    setLights(LOW,HIGH,HIGH,HIGH);
    delay(250);
    setLights(HIGH,LOW,HIGH,HIGH);
    delay(250);
    setLights(HIGH,HIGH,LOW,HIGH);
    delay(250);
    setLights(HIGH,HIGH,HIGH,LOW);
    delay(250);

  }
  setLights(LOW,LOW,LOW,LOW);
}


int getButtonPushed(){
  // make sure a buttonUnpressed method follows when this function is called. 
  do {
    state1 = digitalRead(switch1);
    state2 = digitalRead(switch2);
    state3 = digitalRead(switch3);
    state4 = digitalRead(switch4);
  } while(state1 != HIGH && state2 != HIGH && state3 != HIGH && state4 != HIGH);
  if (state1 == HIGH){
    return switch1;
  }
  if (state2 == HIGH){
    return switch2;
  }
  if (state3 == HIGH){
    return switch3;
  }
  if (state4 == HIGH){
    return switch4;
  }
}

void buttonUnpressed(){
  do {
    state1 = digitalRead(switch1);
    state2 = digitalRead(switch2);
    state3 = digitalRead(switch3);
    state4 = digitalRead(switch4);
  } while(state1 == HIGH || state2 == HIGH || state3 == HIGH || state4 == HIGH);
}
void setLights(int l1, int l2, int l3, int l4){
  digitalWrite(led1, l1);
  digitalWrite(led2, l2);
  digitalWrite(led3, l3);
  digitalWrite(led4, l4);
}
