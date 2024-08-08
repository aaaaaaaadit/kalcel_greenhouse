#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float temp;
float humid;
bool pumpState = false;
bool fanState = false;
int modeButtonPin = 11;
int upButton = 10;
int downButton = 9;
int screenMode = 0;

int modeButtonState;
int lastModeButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  dht.begin();
  
  lcd.backlight();
  lcd.begin();

  pinMode(modeButtonPin,INPUT_PULLUP);
}

void loop(){
  buttonCheck();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(screenMode);
}

void buttonCheck(){
  modeButtonCheck();
  
}

void modeButtonCheck(){
  int modeButtonReading = digitalRead(modeButtonPin);
  if(modeButtonReading != lastModeButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (modeButtonReading != modeButtonState) {
      modeButtonState = modeButtonReading;
      if (modeButtonState==LOW) {
        screenMode++;
      }
    }
  }
}


void ThermoHygroDisplay(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
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
  if (fanState==false){
    lcd.print("FAN OFF");
  } else {
    lcd.print("FAN RUNNING");
  }
  lcd.setCursor(0,1);
  if(pumpState==false){
    lcd.print("PUMP OFF");
  }else{
    lcd.print("PUMP RUNNING");
  }
}

void pumpTimerDisplay(){
  lcd.clear();
  lcd.setCursor(0,0);
  if (pumpState==false){
    lcd.print("IDLE.");
  }else{
    lcd.print("PUMPING...");
  }
  lcd.setCursor(0,1);
  if (pumpState==false){
    lcd.print("PUMP IN ");
  }else{
    lcd.print("STOP IN ");
  }
  lcd.print("00:00:00");
}
