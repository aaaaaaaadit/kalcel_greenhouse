#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temp;
float humid;

int modeButtonPin = 11;
int upButtonPin = 10;
int downButtonPin = 9;
int pumpPin = 12;
int fanPin = 13;

int screenMode = 0;

int modeButtonState;
int lastModeButtonState = LOW;
unsigned long modeLastDebounceTime = 0;
unsigned long modeDebounceDelay = 50;

unsigned long dispUpdatePreviousMillis = 0;
const long dispUpdateInterval = 10000;

void setup() {
  dht.begin();
  
  lcd.backlight();
  lcd.begin();

  pinMode(modeButtonPin,INPUT_PULLUP);
  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
}

void loop(){
  readSensor();
  fanAction();
  pumpAction();
  buttonCheck();
  updateDisplay();

}

void buttonCheck(){
  modeButtonCheck();
  //upButtonCheck();
  //downButtonCheck();
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
        if(screenMode>2){
          screenMode=0;
        }
        switch(screenMode){
          case 0:{
            pumpTimerDisplay();
          }
          break;

          case 1:{
            thermoHygroDisplay();
          }
          break;

          case 2:{
            systemStateDisplay();
          }
          break;
        }
      }
    }
  }
  lastModeButtonState = modeButtonReading;
}

void readSensor(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
}

void fanAction(){
  if(temp >= 35){
    digitalWrite(fanPin, HIGH);
  }else{
    digitalWrite(fanPin,LOW);
  }return;
}

void pumpAction(){
  digitalWrite(pumpPin, HIGH);
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
  if(digitalRead(fanPin)==0){
    lcd.print("FAN OFF");
  }else{
    lcd.print("FAN RUNNING");
  }
  lcd.setCursor(0,1);
  if(digitalRead(pumpPin)==0){
    lcd.print("PUMP OFF");
  }else{
    lcd.print("PUMP RUNNING");
  }
}

void pumpTimerDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  if (digitalRead(pumpPin)==0){
    lcd.print("IDLE.");
  }else{
    lcd.print("PUMPING...");
  }
  lcd.setCursor(0,1);
  if (digitalRead(pumpPin)==0){
    lcd.print("PUMP IN ");
  }else{
    lcd.print("STOP IN ");
  }
  lcd.print("00:00:00");
}

void updateDisplay(){
  unsigned long dispUpdateCurrentMillis = millis();
  if(dispUpdateCurrentMillis - dispUpdatePreviousMillis >= dispUpdateInterval){
    dispUpdatePreviousMillis = dispUpdateCurrentMillis;
    switch(screenMode){
      case 0:{
        pumpTimerDisplay();
      }
      break;

      case 1:{
        thermoHygroDisplay();
      }
      break;

      case 2:{
        systemStateDisplay();
      }
      break;
    }
  }
}
