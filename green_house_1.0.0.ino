#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>                                   //DHT11 Sensor
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temp;                                        //Temp & humidity variables
float humid;
float tempThreshold = 30;

int HRS;                                           //Timer variables
int MIN;
int SEC;

int defHRS;
int defMIN;
int defSEC;

int modeButtonPin = 11;                            //Pins mapping
int upButtonPin = 10;
int downButtonPin = 9;
int pumpPin = 6;
int fanPin = 5;
int sparePin = 4;                                  //spare pin for additional control

int screenMode = 5;                                //Screen modes

int timerMode = 0;                                 //Timer modes

int modeButtonState;                               //Button debouncing
int lastModeButtonState = LOW;
unsigned long modeLastDebounceTime = 0;
unsigned long modeDebounceDelay = 50;

int upButtonState;
int lastUpButtonState = LOW;
unsigned long upLastDebounceTime = 0;
unsigned long upDebounceDelay = 50;

int downButtonState;
int lastDownButtonState = LOW;
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
}

void loop(){                                       //Main functions in loop
  startTimer();
  readSensor();
  fanAction();
  pumpAction();
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
      if (modeButtonState==HIGH) {
        screenMode++;
        if(screenMode>5){
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
      if (upButtonState==HIGH) {
        switch(screenMode){                          //The button function
          
          case 3:{
            defMIN++;
            if(defMIN>59){
              defMIN=0;
            }
          }
          break;
          
          case 4:{
            defHRS++;
            if(defHRS>24){
              defHRS=0;
            }
          }
          break;

          case 5:{
            tempThreshold++;
            if(tempThreshold > 45){
              tempThreshold = 45;
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
      if (downButtonState==HIGH) {
        switch(screenMode){                             //The Button Function
          
          case 3:{
            defMIN--;
            if(defMIN<0){
              defMIN=59;
            }
          }
          break;
          
          case 4:{
            defHRS--;
            if(defHRS<0){
              defHRS=24;
            }
          }
          break;

          case 5:{
            tempThreshold--;
            if(tempThreshold < 0){
              tempThreshold = 0;
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

void startTimer(){
  HRS = 1;
  MIN = 15;
  SEC = 0;
}

void fanAction(){
  if(temp >= tempThreshold){
    digitalWrite(fanPin, LOW);
  }else{
    digitalWrite(fanPin,HIGH);
  }return;
}

void pumpAction(){
  digitalWrite(pumpPin, LOW);
}

void thermoHygroDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMP  : ");
  lcd.print(temp,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("HUMID : ");
  lcd.print(humid,1);
  lcd.print(" %");
}

void systemStateDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  if(digitalRead(fanPin)==1){
    lcd.print("FAN STOPPED");
  }else{
    lcd.print("FAN RUNNING");
  }
  lcd.setCursor(0,1);
  if(digitalRead(pumpPin)==1){
    lcd.print("PUMP STOPPED");
  }else{
    lcd.print("PUMP RUNNING");
  }
}

void pumpTimerDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  if (digitalRead(pumpPin)==1){
    lcd.print("PUMP STOPPED,");
  }else{
    lcd.print("PUMP RUNNING...");
  }
  lcd.setCursor(0,1);
  if (digitalRead(pumpPin)==1){
    lcd.print("RUN IN ");
  }else{
    lcd.print("STOP IN ");
  }
  lcd.print(HRS);
  lcd.print(":");
  lcd.print(MIN);
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
        thermoHygroDisplay();
      }
      break;

      case 1:{
        systemStateDisplay();
      }
      break;

      case 2:{
        pumpTimerDisplay();
      }
      break;

      case 3:{
        setPumpTime();
      }
      break;

      case 4:{
        setIdleTime();
      }
      break;

      case 5:{
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
