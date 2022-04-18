

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>



U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 23, /* CS=*/ 5, /* reset=*/ 22); // ESP32

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

float fVoltage = 229.60;
float fAmp = 249.60;
float fPow = 259.60;
float fEng = 429.60;
float fFre = 529.60;
float fPf = 269.60;


char chVol[20];
char chAmp[20];
char chPow[20];
char chEng[20];
char chFre[20];
char chPf[20];

char chVolOut[50];
char chAmpOut[50];
char chPowOut[50];
char chEngOut[50];
char chFreOut[50];
char chPfOut[50];

void converToStr()
{
  dtostrf(fVoltage, 6, 2, chVol);
  dtostrf(fAmp, 6, 2, chAmp);
  dtostrf(fPow, 6, 2, chPow);
  dtostrf(fEng, 6, 2, chEng);
  dtostrf(fFre, 6, 2, chFre);
  dtostrf(fPf, 6, 2, chPf);

  sprintf(chVolOut, "Voltage(V:%s", chVol);
  sprintf(chAmpOut, "Current(A:%s", chAmp);  
  sprintf(chPowOut, "Power W:%s", chPow);  
  sprintf(chEngOut, "Energy(KWh:%s", chEng);  
  sprintf(chFreOut, "Frequency(HZ:%s", chFre);  
  sprintf(chPfOut, "Power Factory:%s", chPf);    
}

void setup(void) {
  u8g2.begin();
}

void loop(void) { 
  u8g2.clearBuffer();
  converToStr();
  u8g2_prepare();
  u8g2.drawStr( 0, 2, chVolOut);
  u8g2.drawStr( 0, 13, chAmpOut);
  u8g2.drawStr( 0, 23, chPowOut);
  u8g2.drawStr( 0, 33, chEngOut);
  u8g2.drawStr( 0, 43, chFreOut);
  u8g2.drawStr( 0, 53, chPfOut);
  u8g2.sendBuffer();
  delay(100);
}
