#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>                                   //DHT11 Sensor
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temp;                                        //Temp & humidity variables
float humid;
float tempThreshold = 35;

int HRS;                                           //Timer variables
int MIN;
int SEC;
int defHRS=1;
int defMIN=10;
int defSEC;
int pumpState = HIGH;  
unsigned long timerPreviousMillis = 0;
const long timerInterval = 1000;                               



int modeButtonPin = 11;                            //Pins mapping
int upButtonPin = 10;
int downButtonPin = 9;
int pumpPin = 6;
int fanPin = 5;
int sprayPin = 4;                                  

int screenMode = 0;                                //Screen modes

int modeButtonState;                               //Button debouncing
int lastModeButtonState = HIGH;
unsigned long modeLastDebounceTime = 0;
unsigned long modeDebounceDelay = 50;

int upButtonState;
int lastUpButtonState = HIGH;
unsigned long upLastDebounceTime = 0;
unsigned long upDebounceDelay = 50;

int downButtonState;
int lastDownButtonState = HIGH;
unsigned long downLastDebounceTime = 0;
unsigned long downDebounceDelay = 50;

unsigned long dispUpdatePreviousMillis = 0;        //Screen refresh interval
const long dispUpdateInterval = 10000;

void setup() {                                     //Components initialization
  dht.begin();
  
  lcd.backlight();
  lcd.begin();

  pinMode(modeButtonPin,INPUT_PULLUP);             //Pins initialization
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  
  pinMode(pumpPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(sprayPin,OUTPUT);
  
  digitalWrite(pumpPin,HIGH);
  digitalWrite(fanPin,HIGH);
  digitalWrite(sprayPin,HIGH);

  readSensor();
  mainDisplay();
}

void loop(){                                       //Main functions inside loop
  pumpTimer();
  readSensor();
  fanAction();
  buttonCheck();
  updateDisplay();

}

void buttonCheck(){
  modeButtonCheck();
  upButtonCheck();
  downButtonCheck();
}

void modeButtonCheck(){
  int modeButtonReading = digitalRead(modeButtonPin);
  if(modeButtonReading != lastModeButtonState) {
    modeLastDebounceTime = millis();
  }
  if ((millis() - modeLastDebounceTime) > modeDebounceDelay) {
    if (modeButtonReading != modeButtonState) {
      modeButtonState = modeButtonReading;
      if (modeButtonState==LOW) {
        screenMode++;
        if(screenMode>4){
          screenMode=0;
        }
        mainDisplay();
      }
    }
  }
  lastModeButtonState = modeButtonReading;
}

void upButtonCheck(){
 int upButtonReading = digitalRead(upButtonPin);
  if(upButtonReading != lastUpButtonState) {
    upLastDebounceTime = millis();
  }
  if ((millis() - upLastDebounceTime) > upDebounceDelay) {
    if (upButtonReading != upButtonState) {
      upButtonState = upButtonReading;
      if (upButtonState==LOW) {
        switch(screenMode){                          //The button function
          
          case 2:{
            defMIN++;
            if(defMIN>59){
              defMIN=0;
            }
          }
          break;
          
          case 3:{
            defHRS++;
            if(defHRS>24){
              defHRS=1;
            }
          }
          break;

          case 4:{
            tempThreshold++;
            if(tempThreshold > 43){
              tempThreshold = 43;
            }
          }
          break;
        }
        mainDisplay();                                //refresh main display after button press
      }
    }
  }
  lastUpButtonState = upButtonReading;
}

void downButtonCheck(){
 int downButtonReading = digitalRead(downButtonPin);
  if(downButtonReading != lastDownButtonState) {
    downLastDebounceTime = millis();
  }
  if ((millis() - downLastDebounceTime) > downDebounceDelay) {
    if (downButtonReading != downButtonState) {
      downButtonState = downButtonReading;
      if (downButtonState==LOW) {
        switch(screenMode){                             //The Button Function
          
          case 2:{
            defMIN--;
            if(defMIN<0){
              defMIN=59;
            }
          }
          break;
          
          case 3:{
            defHRS--;
            if(defHRS<1){
              defHRS=24;
            }
          }
          break;

          case 4:{
            tempThreshold--;
            if(tempThreshold < 16){
              tempThreshold = 16;
            }
          }
          break;
        }
        mainDisplay();                              //refresh main display after button press
      }
    }
  }
  lastDownButtonState = downButtonReading;
}

void readSensor(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
}

void pumpTimer(){ 
unsigned long timerCurrentMillis=millis();
  if(timerCurrentMillis-timerPreviousMillis>=timerInterval){
    timerPreviousMillis=timerCurrentMillis;
    SEC--;
    if(SEC<0){
      SEC=59;
      MIN--;
      if(MIN<0){
        MIN=59;
        HRS--;
        if(HRS<0){
          if(pumpState==LOW){
            pumpState=HIGH;
            HRS=defHRS;
            MIN=0;
            SEC=0;
          }else{
            pumpState=LOW;
            HRS=0;
            MIN=defMIN;
            SEC=0;
          }
        }
      } 
    }
    digitalWrite(pumpPin,pumpState);
  }
}

void fanAction(){
  if(temp >= tempThreshold){
    digitalWrite(fanPin, LOW);
  }else{
    digitalWrite(fanPin,HIGH);
  }return;
}


void statusDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(temp,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print(humid,0);
  lcd.print(" %");
  if(digitalRead(fanPin)==1){
    lcd.setCursor(9,0);
    lcd.print("FAN OFF");
  }else{
    lcd.setCursor(10,0);
    lcd.print("FAN ON");
  }
  if(pumpState=HIGH){
    lcd.setCursor(8,1);
    lcd.print("PUMP OFF");
  }else{
    lcd.setCursor(9,1);
    lcd.print("PUMP ON");
  }
}


void pumpTimerDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  if (pumpState==HIGH){
    lcd.print("PUMP OFF, TURN ");
  }else{
    lcd.print("PUMP ON, TURN ");
  }
  lcd.setCursor(0,1);
  if (pumpState==HIGH){
    lcd.print("ON IN ");
  }else{
    lcd.print("OFF IN ");
  }
  if(MIN>=59){
    lcd.print(HRS+1);
    lcd.print(":");
    lcd.print("0");
    lcd.print(MIN-59);
  }else{
    lcd.print(HRS);
    lcd.print(":");
    if(MIN<9){
      lcd.print("0");
    }
    lcd.print(MIN+1);
  }
}

void setPumpTime(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PUMP TIME(MIN):");
  lcd.setCursor(0,1);
  lcd.print(defMIN);
}

void setIdleTime(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IDLE TIME(HRS):");
  lcd.setCursor(0,1);
  lcd.print(defHRS);
}

void setThresholdTemp(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FAN RUN TEMP(");
  lcd.print((char)223);
  lcd.print("C)");
  lcd.setCursor(0,1);
  lcd.print(tempThreshold,0);
}

void mainDisplay(){
     switch(screenMode){
      case 0:{
        statusDisplay();
      }
      break;

      case 1:{
        pumpTimerDisplay();
      }
      break;

      case 2:{
        setPumpTime();
      }
      break;

      case 3:{
        setIdleTime();
      }
      break;

      case 4:{
        setThresholdTemp();
      }
    }
}

void updateDisplay(){
  unsigned long dispUpdateCurrentMillis = millis();
  if(dispUpdateCurrentMillis - dispUpdatePreviousMillis >= dispUpdateInterval){
    dispUpdatePreviousMillis = dispUpdateCurrentMillis;
    mainDisplay();
  }
}
