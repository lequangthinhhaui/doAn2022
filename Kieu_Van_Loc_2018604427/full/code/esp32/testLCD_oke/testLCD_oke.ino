
#include <Arduino.h>

//u8g2
#include <U8g2lib.h>
#include <SPI.h>

//pzem
#include <PZEM004Tv30.h>

//mqtt
#include <WiFi.h>
#include <PubSubClient.h>

//function define 
void IRAM_ATTR onTimer();
void setup_wifi();
void mqttHandle();
void u8g2_prepare(void);
void updatePzem();
void converToStr();

//define wifi infor
#define ssid "Zoro"
#define password "minhdien04"

//define broker infor
#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 
String mqtt_topic_pub = "hungthinhhaui/maylanh"; 

//init wifi and mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//init pzem
PZEM004Tv30 pzem(Serial2, 16, 17);
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 23, /* CS=*/ 5, /* reset=*/ 22); // ESP32

//init timer
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//user variable
long lastMsg = 0;

String dataMQTT = "";

//pzem measurement infor
float fVoltage = 0;
float fAmp = 0;
float fPow = 0;
float fEng = 0;
float fFre = 0;
float fPf = 0;

//convert pzem measurement to str
char chVol[20];
char chAmp[20];
char chPow[20];
char chEng[20];
char chFre[20];
char chPf[20];

//merge the pzem measurement to out
char chVolOut[50];
char chAmpOut[50];
char chPowOut[50];
char chEngOut[50];
char chFreOut[50];
char chPfOut[50];

void enableTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}


void mqttHandle()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  dataMQTT = "";
  dataMQTT = "{\"Vol\":\"" + String(fVoltage) + "\"," +
                     "\"Amp\":\"" + String(fAmp) + "\"," +
                     "\"Pow\":\"" + String(fPow) + "\"," +
                     "\"Eng\":\"" + String(fEng) + "\"," +
                     "\"Fre\":\"" + String(fFre) + "\"," +
                     "\"Pf\":\"" + String(fPf) + "\"}";

  Serial.print("dataMQTT:");
  Serial.println(dataMQTT);
  
  client.publish(mqtt_topic_pub.c_str(), dataMQTT.c_str());

}

void setup(void) {
  u8g2.begin();
  enableTimer();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2_prepare();
  mqttHandle(); 
  updatePzem();
  converToStr();
  
  u8g2.drawStr( 0, 2, chVolOut);
  u8g2.drawStr( 0, 13, chAmpOut);
  u8g2.drawStr( 0, 23, chPowOut);
  u8g2.drawStr( 0, 33, chEngOut);
  u8g2.drawStr( 0, 43, chFreOut);
  u8g2.drawStr( 0, 53, chPfOut);
  u8g2.sendBuffer();
  delay(3000);
}


//timer function
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);

  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    String clientId = String(random(0xffff), HEX); 
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd))
    {
      Serial.println("Connected MQTT ngoinhaiot.com");
      digitalWrite(16, HIGH);
    }
    else
    {
      Serial.println("Không thể kết nối MQTT ngoinhaiot.com");
      digitalWrite(16, LOW);
      delay(5000);
    }
  }
}

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void updatePzem()
{
  fVoltage = pzem.voltage();
  fAmp = pzem.current();
  fPow = pzem.power();
  fEng = pzem.energy();
  fFre = pzem.frequency();
  fPf = pzem.pf();
  
  if(isnan(fVoltage))
      fVoltage = 0;
  if (isnan(fAmp))
      fAmp = 0;
  if (isnan(fPow))
      fPow = 0;
  if (isnan(fEng))
      fEng = 0;
  if (isnan(fFre))
      fFre = 0;
  if (isnan(fPf))
      fPf = 0; 
}


void converToStr()
{
  dtostrf(fVoltage, 6, 2, chVol);
  dtostrf(fAmp, 6, 2, chAmp);
  dtostrf(fPow, 6, 2, chPow);
  dtostrf(fEng, 6, 2, chEng);
  dtostrf(fFre, 6, 2, chFre);
  dtostrf(fPf, 6, 2, chPf);

  sprintf(chVolOut, "Voltage(V):%s", chVol);
  sprintf(chAmpOut, "Current(A):%s", chAmp);  
  sprintf(chPowOut, "Power(W):%s", chPow);  
  sprintf(chEngOut, "Energy(KWh):%s", chEng);  
  sprintf(chFreOut, "Frequency(HZ):%s", chFre);  
  sprintf(chPfOut, "Power Factory:%s", chPf);    
}
