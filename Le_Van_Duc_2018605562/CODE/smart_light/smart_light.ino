
#include <Button.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <EEPROM.h>



//user button
Button btnMenu(2); 
Button btnUp(3); 
Button btnDown(4); 
Button btnLeft(5); 
Button btnRight(6); 

//user lamp
const int lamp1 = 12;   
const int lamp2 = 11; 
const int lamp3 = 10; 
const int lamp4 = 9;   
const int lamp5 =  8; 
const int lamp6 =  7; 

//light sensor pin
const int lightSensorPin = A2;


//user variable

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//lcd
LiquidCrystal_I2C lcd(0x27,16,2);

//rtc
RTC_DS1307 rtc;


//user function define
void lampAll();
void LampAlternate(unsigned int currentTime, unsigned int startTime, unsigned int endTime);
void LampAlternate(unsigned int currentTime, unsigned int startTime, unsigned int endTime);
int menuOfState();
void handleMenuState(int &menuState);
void rtcInit();
void rtcExe();
void displayTime();
void settingHourMode1(int &stHour);
void settingMinuteMode1(int &stMinute);
void settingSecondMode1(int &stSecond);
void configPin()
{
  //input pin
  btnMenu.begin();
  btnUp.begin(); 
  btnDown.begin(); 
  btnLeft.begin(); 
  btnRight.begin(); 
  
  //output pin
  pinMode(lamp1, OUTPUT);
  pinMode(lamp2, OUTPUT);
  pinMode(lamp3, OUTPUT);
  pinMode(lamp4, OUTPUT);
  pinMode(lamp5, OUTPUT);
  pinMode(lamp6, OUTPUT);
}


int buttonState = 0;         // variable for reading the pushbutton status
DateTime now;
void setup() {
  configPin();
  lcd.init();   
  Serial.begin(9600);
  lcd.backlight();
  rtcInit();
}


int menuState = 0;
int modeState = 0;
int swModeState = 0;
unsigned int timeSecond = 720000;
bool flagMode2 = false;

//start time lamp all
int stHourAll = EEPROM.read(0);
int stMinuteAll = EEPROM.read(1);
int stSecondAll = EEPROM.read(2);
//end time lamp all
int edHourAll = EEPROM.read(3);
int edMinuteAll = EEPROM.read(4);
int edSecondAll = EEPROM.read(5);

//start time lamp sole
int stHourSole = EEPROM.read(6);
int stMinuteSole = EEPROM.read(7);
int stSecondSole = EEPROM.read(8);
//start time lamp sole
int edHourSole = EEPROM.read(9);
int edMinuteSole = EEPROM.read(10);
int edSecondSole = EEPROM.read(11);

//demo 
unsigned long previousMillis = 0;        // will store last time LED was updated

const long interval = 1000;

bool flagLampModeAll = false;
bool flagLampModeSole = false;


void loop() {
  now = rtc.now();
  menuState = menuOfState();
  if(modeState == 0)
  {
    if (analogRead(lightSensorPin) > 200)
    {
     for(int i = 7; i<=12; i++)
      {
        digitalWrite(i, LOW);
      }
    }
    else
    {
     for(int i = 7; i<=12; i++)
      {
        digitalWrite(i, HIGH);
      }      
    }
  }
  else
  {
    if(flagLampModeSole == false)
      lampAll();
    if(flagLampModeAll == false) 
      lampSole();
  }
  handleMenuState(menuState);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println("*****ALL*****");
    Serial.println(stHourAll);
    Serial.println(stMinuteAll);
    Serial.println(stSecondAll);
    Serial.println(edHourAll);
    Serial.println(edMinuteAll);
    Serial.println(edSecondAll);
    Serial.println("*****ALL*****");
    
    Serial.println("*****SOLE*****");
    Serial.println(stHourSole);
    Serial.println(stMinuteSole);
    Serial.println(stSecondSole);
    Serial.println(edHourSole);
    Serial.println(edMinuteSole);
    Serial.println(edSecondSole);
    Serial.println("*****SOLE*****");
  }
}

void turnOnLamp()
{
  for(int i = 7; i<=12; i ++)
  {
    digitalWrite(i, HIGH);
  }    
}
void turnOnSoleLamp()
{
  for(int i = 7; i<=12; i = i+2)
  {
    digitalWrite(i, HIGH);
  }    
}
void turnOffLamp()
{
  for(int i = 7; i<=12; i ++)
  {
    digitalWrite(i, LOW);
  }    
}

void lampAll()
{
  unsigned int currentTimeSec = now.hour() * 3600 + now.minute() * 60 + now.second();
  unsigned int setAllTimeSecStart = stHourAll * 3600 + stMinuteAll * 60 + stSecondAll;
  unsigned int setAllTimeSecEnd = edHourAll * 3600 + edMinuteAll * 60 + edSecondAll;
  
  if(currentTimeSec >= setAllTimeSecStart && currentTimeSec <= setAllTimeSecEnd)
  {
    turnOnLamp();
    flagLampModeAll = true;     
  }
  else
  {
    turnOffLamp();
    flagLampModeAll = false;
  }  
}

void lampSole()
{
  unsigned int currentTimeSec = now.hour() * 3600 + now.minute() * 60 + now.second();
  unsigned int setSoleTimeSecStart = stHourSole * 3600 + stMinuteSole * 60 + stSecondSole;
  unsigned int setSoleTimeSecEnd = edHourSole * 3600 + edMinuteSole * 60 + edSecondSole;
  
  if(currentTimeSec >= setSoleTimeSecStart && currentTimeSec <= setSoleTimeSecEnd)
  {
     flagLampModeSole = true;
     turnOnSoleLamp();
       
  }
  else
  {
    flagLampModeSole = false;
    turnOffLamp();
  }
}


int menuOfState()
{
  if (btnMenu.toggled())
    if (btnMenu.read() == Button::PRESSED)
    {
        menuState ++;
        if(menuState > 2 )
        {
          menuState = 0;     
        }
        Serial.println(menuState);
    }
    return menuState;  
}

int modeOfState()
{
  if (btnLeft.toggled())
    if (btnLeft.read() == Button::PRESSED)
    {
        modeState ++;
        if(modeState > 1)
        {
          modeState = 0; 
        }
        Serial.println(modeState);
    }

  if (btnRight.toggled())
    if (btnRight.read() == Button::PRESSED)
    {
        modeState --;
        if(modeState < 0)
        {
          modeState = 1; 
        }
        Serial.println(modeState);
    }
    return modeState;  
}

int switchModeState()
{
  if (btnLeft.toggled())
    if (btnLeft.read() == Button::PRESSED)
    {
        swModeState ++;
        if(swModeState > 11)
        {
          swModeState = 0; 
        }
        Serial.println(swModeState);
    }

  if (btnRight.toggled())
    if (btnRight.read() == Button::PRESSED)
    {
        swModeState --;
        if(swModeState < 0)
        {
          swModeState = 11; 
        }
        Serial.println(swModeState);
    }
    return swModeState;  
}


//menu state flag
bool flagMenuState0 = false;
bool flagMenuState1 = false;
bool flagMenuState2 = false;

//switch mode state flag
bool flagSwModeState0 = false;
bool flagSwModeState1 = false;
bool flagSwModeState2 = false;
bool flagSwModeState3 = false;
bool flagSwModeState4 = false;
bool flagSwModeState5 = false;
bool flagSwModeState6 = false;
bool flagSwModeState7 = false;
bool flagSwModeState8 = false;
bool flagSwModeState9 = false;
bool flagSwModeState10 = false;
bool flagSwModeState11 = false;

void handleMenuState(int &menuState)
{
  if(menuState == 0)
  {
    if(flagMenuState0 == false)
    {
      lcd.clear();
      flagMenuState0 = true;
      flagMenuState1 = false;
      flagMenuState2 = false;
      EEPROM.write(0, stHourAll);
      EEPROM.write(1, stMinuteAll);
      EEPROM.write(2, stSecondAll);
      EEPROM.write(3, edHourAll);
      EEPROM.write(4, edMinuteAll);
      EEPROM.write(5, edSecondAll);
      EEPROM.write(6, stHourSole);
      EEPROM.write(7, stMinuteSole);
      EEPROM.write(8, stSecondSole);
      EEPROM.write(9, edHourSole);
      EEPROM.write(10, edMinuteSole);
      EEPROM.write(11, edSecondSole);
    }

    displayTime();
  }
  else if(menuState == 1)
  {
    if(flagMenuState1 == false)
    {
      lcd.clear();
      flagMenuState0 = false;
      flagMenuState1 = true;
      flagMenuState2 = false;
    }
    modeState = modeOfState();  
    lcd.setCursor(0, 0);
    lcd.print("Choose Mode!");
    lcd.setCursor(0, 1);
    lcd.print("Mode:");
    lcd.setCursor(6, 1);
    lcd.print(modeState);
  }
  else if(menuState == 2)
  {
    if(flagMenuState2 == false)
    {
      lcd.clear();
      flagMenuState0 = false;
      flagMenuState1 = false;
      flagMenuState2 = true;
    }
     if(modeState == 0)
     {
      menuState = 0;
     }
     else
     {
///////////////////////////setting mode 1 handling
      swModeState = switchModeState();
      if(swModeState == 0)
      {
        if(flagSwModeState0 == false)
        {
          lcd.clear();
          flagSwModeState0 = true;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: All ");
        lcd.setCursor(0,1);
        lcd.print("Start Hour:");
        lcd.setCursor(12,1);
        settingHourMode1(stHourAll);
        lcd.print(stHourAll);
        lcd.print("                ");   
      }
      else if(swModeState == 1)
      {
        if(flagSwModeState1 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = true;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: All ");
        lcd.setCursor(0,1);
        lcd.print("Start Minute:");
        lcd.setCursor(14,1);
        settingMinuteMode1(stMinuteAll);
        lcd.print(stMinuteAll);
        lcd.print("                ");       
      }
      else if(swModeState == 2)
      {
        if(flagSwModeState2 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = true;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: All ");
        lcd.setCursor(0,1);
        lcd.print("Start Second:");
        lcd.setCursor(14,1);
        settingSecondMode1(stSecondAll);
        lcd.print(stSecondAll);
        lcd.print("                ");          
      }
      else if(swModeState == 3)
      {
        if(flagSwModeState3 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = true;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: ALL");
        lcd.setCursor(0,1);
        lcd.print("End Hour:");
        lcd.setCursor(10,1);
        settingHourMode1(edHourAll);
        lcd.print(edHourAll);
        lcd.print("                ");          
      } 
      else if(swModeState == 4)
      {
        if(flagSwModeState4 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = true;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: All");
        lcd.setCursor(0,1);
        lcd.print("End Minute:");
        lcd.setCursor(12,1);
        settingMinuteMode1(edMinuteAll);
        lcd.print(edMinuteAll);
        lcd.print("                ");          
      }
      else if(swModeState == 5)
      {
        if(flagSwModeState5 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = true;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: All");
        lcd.setCursor(0,1);
        lcd.print("End Second:");
        lcd.setCursor(12,1);
        settingSecondMode1(edSecondAll);
        lcd.print(edSecondAll);
        lcd.print("                ");          
      }

//////////
      else if(swModeState == 6)
      {
        if(flagSwModeState6 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = true;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole ");
        lcd.setCursor(0,1);
        lcd.print("Start Hour:");
        lcd.setCursor(12,1);
        settingHourMode1(stHourSole);
        lcd.print(stHourSole);
        lcd.print("                ");     
      }
      else if(swModeState == 7)
      {
        if(flagSwModeState7 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = true;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole ");
        lcd.setCursor(0,1);
        lcd.print("Start Minute:");
        lcd.setCursor(14,1);
        settingMinuteMode1(stMinuteSole);
        lcd.print(stMinuteSole);
        lcd.print("                ");          
      }
      else if(swModeState == 8)
      {
        if(flagSwModeState8 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = true;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole ");
        lcd.setCursor(0,1);
        lcd.print("Start Second:");
        lcd.setCursor(14,1);
        settingSecondMode1(stSecondSole);
        lcd.print(stSecondSole);
        lcd.print("                ");          
      }
      else if(swModeState == 9)
      {
        if(flagSwModeState9 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = true;
          flagSwModeState10 = false;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole");
        lcd.setCursor(0,1);
        lcd.print("End Hour:");
        lcd.setCursor(10,1);
        settingHourMode1(edHourSole);
        lcd.print(edHourSole);
        lcd.print("                ");          
      } 
      else if(swModeState == 10)
      {
        if(flagSwModeState10 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = true;
          flagSwModeState11 = false;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole");
        lcd.setCursor(0,1);
        lcd.print("End Minute:");
        lcd.setCursor(12,1);
        settingMinuteMode1(edMinuteSole);
        lcd.print(edMinuteSole);
        lcd.print("                ");          
      }
      else if(swModeState == 11)
      {
        if(flagSwModeState11 == false)
        {
          lcd.clear();
          flagSwModeState0 = false;
          flagSwModeState1 = false;
          flagSwModeState2 = false;
          flagSwModeState3 = false;
          flagSwModeState4 = false;
          flagSwModeState5 = false;
          flagSwModeState6 = false;
          flagSwModeState7 = false;
          flagSwModeState8 = false;
          flagSwModeState9 = false;
          flagSwModeState10 = false;
          flagSwModeState11 = true;
        }
        lcd.setCursor(0,0);
        lcd.print("Mode: Sole");
        lcd.setCursor(0,1);
        lcd.print("End Second:");
        lcd.setCursor(12,1);
        settingSecondMode1(edSecondSole);
        lcd.print(edSecondSole);
        lcd.print("                ");          
      }       
     }
  }
}


void rtcInit()
{
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  
}



void displayTime()
{
   lcd.setCursor(0,0);
   lcd.print("Time:");
   lcd.setCursor(6,0);
   lcd.print(now.hour(),DEC); 
   lcd.print(":");
   lcd.print(now.minute(),DEC);
   lcd.print(":");
   if(now.second() < 10)
   {
    lcd.print(now.second(),DEC);
    lcd.print("                      ");
   }
   else
    lcd.print(now.second(),DEC);
   lcd.setCursor(0,1);
   lcd.print("Mode:");
   lcd.setCursor(6,1);
   lcd.print(modeState);
}

void settingHourMode1(int &stHour)
{
    if (btnDown.toggled())
    {
      if (btnDown.read() == Button::PRESSED)
      {
          stHour --;
          if(stHour < 0)
          {
            stHour = 23;
             
          }
          Serial.println(stHour);
      }
    }
    if (btnUp.toggled())
    {
      if (btnUp.read() == Button::PRESSED)
      {
          stHour ++;
          if(stHour > 23)
          {
            stHour = 0;
            lcd.clear(); 
          }
          Serial.println(stHour);
      }
    }
}


void settingMinuteMode1(int &stMinute)
{
    if (btnDown.toggled())
    {
      if (btnDown.read() == Button::PRESSED)
      {
          stMinute --;
          if(stMinute < 0)
          {
            stMinute = 59; 
          }
          Serial.println(stMinute);
      }
    }
    if (btnUp.toggled())
    {
      if (btnUp.read() == Button::PRESSED)
      {
          stMinute ++;
          if(stMinute > 59)
          {
            stMinute = 0;
            lcd.clear(); 
          }
          Serial.println(stMinute);
      }
    }
}

void settingSecondMode1(int &stSecond)
{
    if (btnDown.toggled())
    {
      if (btnDown.read() == Button::PRESSED)
      {
          stSecond --;
          if(stSecond < 0)
          {
            stSecond = 59;
          }
          Serial.println(stSecond);
      }
    }
    if (btnUp.toggled())
    {
      if (btnUp.read() == Button::PRESSED)
      {
          stSecond ++;
          if(stSecond > 59)
          {
            stSecond = 0;
            lcd.clear(); 
          }
          Serial.println(stSecond);
      }
    }
}
