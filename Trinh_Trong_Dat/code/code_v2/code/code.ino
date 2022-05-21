#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

int inputVal  = 0;

const int Lamp1 = 8;
const int Lamp2 = 9;
const int Lamp3 = 10;
const int Lamp4 = 11;
const int Lamp5 = 12;
const int ADC_pin = A2;

int temp,inc,hours1,minut,add=11;
int HOUR,MINUT,SECOND;

//Button
const int Mode = 3;
int next = 4; //Chinh gio, phut    
int INC = 5; // Chinh trang thai
int set_mo = 6;
int set_tat = 7;

//LCD: SDA: A4, SCL: A5
LiquidCrystal_I2C lcd(0x27,16,2);

//DS1307: SDA: A4, SCL: A5
byte degree[8] ={
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};
RTC_DS1307 RTC;

void setup() {
  pinMode(INC, INPUT);
  pinMode(next, INPUT);
  pinMode(set_mo, INPUT);
  pinMode(set_tat, INPUT);
 
  pinMode(ADC_pin, INPUT);
  pinMode(Mode, INPUT);
  pinMode(Lamp1, OUTPUT);
  pinMode(Lamp2, OUTPUT);
  pinMode(Lamp3, OUTPUT);
  pinMode(Lamp4, OUTPUT);
  pinMode(Lamp5, OUTPUT);

  digitalWrite(next, HIGH);
  digitalWrite(set_mo, HIGH);
  digitalWrite(INC, HIGH);
  digitalWrite(set_tat, HIGH);
  digitalWrite(Mode, LOW);
 
  Serial.begin(9600);
  Wire.begin();
  
  lcd.backlight();
  lcd.begin(16,2);
  lcd.init();

  lcd.setCursor(0,0);
  lcd.print("Real Time Clock");
  lcd.setCursor(0,1);
  lcd.print("Circuit Digest ");
   if (! RTC.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
//  if(!RTC.isrunning()){
//   RTC.adjust(DateTime(__DATE__,__TIME__));
//  }
//  RTC.adjust(DateTime(__DATE__,__TIME__));
}

void loop() {
  DateTime now = RTC.now();
  if(digitalRead(Mode) == 0){ //Mode Auto
    lcd.setCursor(0,0);
    lcd.print("   Mode Auto    ");
    
    //Serial.println("Mode Auto");
    inputVal = analogRead (ADC_pin);
    Serial.println(inputVal);
    if(inputVal <= 400 && inputVal > 200){
       digitalWrite(Lamp1, LOW); //2
       digitalWrite(Lamp3, HIGH); //3
       digitalWrite(Lamp5, HIGH);
       digitalWrite(Lamp2, HIGH); //1
       digitalWrite(Lamp4, LOW);
       lcd.setCursor(0,1);
       lcd.print("   Status: On 3  ");
    }else if(inputVal <= 200){
       digitalWrite(Lamp1, HIGH);
       digitalWrite(Lamp2, HIGH);
       digitalWrite(Lamp3, HIGH);
       digitalWrite(Lamp4, HIGH);
       digitalWrite(Lamp5, HIGH);
       lcd.setCursor(0,1);
       lcd.print("   Status: On 5  ");
    }else{
       digitalWrite(Lamp1, LOW);
       digitalWrite(Lamp2, LOW);
       digitalWrite(Lamp3, LOW);
       digitalWrite(Lamp4, LOW);
       digitalWrite(Lamp5, LOW);
       lcd.setCursor(0,1);
       lcd.print("   Status: Off  ");
    }
    //Serial.print("Analog value: ");
    //Serial.println(inputVal);

  }
  else{//Mode Menu
    int temp=0,val=1,temp4;
    if(digitalRead(set_mo) == 0){      //set Alarm time
        lcd.setCursor(0,0);
        lcd.print("     Set ON     ");
        delay(2000);
        defualt();
        time();
        delay(1000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  Alarm time ");
        lcd.setCursor(0,1);
        lcd.print(" has been set  ");
        delay(2000);
     }
    if(digitalRead(set_tat) == 0){     //set Alarm time
        lcd.setCursor(0,0);
        lcd.print("    Set OFF     ");
        delay(2000);
        defualt();
        time();
        delay(1000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  Alarm time ");
        lcd.setCursor(0,1);
        lcd.print(" has been set  ");
        delay(2000);
     }
     
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Time:");
     lcd.setCursor(6,0);
     lcd.print(HOUR=now.hour(),DEC); 
     lcd.print(":");
     lcd.print(MINUT=now.minute(),DEC);
     lcd.print(":");
     lcd.print(SECOND=now.second(),DEC);
     lcd.setCursor(0,1);
     lcd.print("Date: ");
     lcd.print(now.day(),DEC);
     lcd.print("/");
     lcd.print(now.month(),DEC);
     lcd.print("/");
     lcd.print(now.year(),DEC);
     match();
     match1();
     delay(500);
  }
}

void defualt(){
  lcd.setCursor(0,1);
  lcd.print(HOUR);
  lcd.print(":");
  lcd.print(MINUT);
  lcd.print(":");
  lcd.print(SECOND);
}

void time()                             {
  int temp=1,minuts=0,hours=0,seconds=0;
  while(temp==1){
    if(digitalRead(INC)==0){
      HOUR++;
      if(HOUR==24){
        HOUR=0;
      }
      while(digitalRead(INC)==0);
     }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Alarm Time ");

    lcd.setCursor(0,1);
    lcd.print(HOUR);
    lcd.print(":");
    lcd.print(MINUT);
    lcd.print(":");
    lcd.print(SECOND);
    delay(100);
    if(digitalRead(next)==0){
      hours1=HOUR;
      EEPROM.write(add++,hours1);
      temp=2;
      while(digitalRead(next)==0);
    }
  }
    
  while(temp==2){
    if(digitalRead(INC)==0){
      MINUT++;
      if(MINUT==60){
        MINUT=0;
      }
      while(digitalRead(INC)==0);
     }
    lcd.setCursor(0,1);
    lcd.print(HOUR);
    lcd.print(":");
    lcd.print(MINUT);
    lcd.print(":");
    lcd.print(SECOND);
    delay(100);
      if(digitalRead(next)==0){
       minut=MINUT;
       EEPROM.write(add++, minut);
       temp=0;
       while(digitalRead(next)==0);
      }
    }
    delay(1000);
}

void match(){
  int tem[23];
  for(int i=11;i<23;i++){
    tem[i]=EEPROM.read(i);
  }
  if(HOUR == tem[11] && MINUT == tem[12]) {
    digitalWrite(Lamp1, HIGH);
    digitalWrite(Lamp2, HIGH);
    digitalWrite(Lamp3, HIGH);
    digitalWrite(Lamp4, HIGH);
    digitalWrite(Lamp5, HIGH);
  }
}
void match1(){
  int tem1[23];
  for(int j=11;j<23;j++){
    tem1[j]=EEPROM.read(j);
  }
  if(HOUR == tem1[13] && MINUT == tem1[14]) {
    digitalWrite(Lamp1, LOW);
    digitalWrite(Lamp2, LOW);
    digitalWrite(Lamp3, LOW);
    digitalWrite(Lamp4, LOW);
    digitalWrite(Lamp5, LOW);
  }
}
