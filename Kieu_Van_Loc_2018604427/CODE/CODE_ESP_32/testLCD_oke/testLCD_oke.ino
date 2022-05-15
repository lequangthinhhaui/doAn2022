#include <Arduino.h>
#include <HardwareSerial.h>


#include <U8g2lib.h>
#include <SPI.h>


//ota update
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

//pzem
#include <PZEM004Tv30.h>

//mqtt
#include <WiFi.h>
#include <PubSubClient.h>

//function define 
void setup_wifi();
void mqttHandle();
void u8g2_prepare(void);
void updatePzem();
void converToStr();
void otaUpdate();

//define wifi infor
#define ssid "vanloc"
#define password "vanloc123"

//define broker infor
#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 
String mqtt_topic_pub = "kieuvanloc/data"; 

//init wifi and mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//init pzem
PZEM004Tv30 pzem(Serial2,16,17);

//init LCD screen
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 23, /* CS=*/ 5, /* reset=*/ 22); // ESP32


//inti ota update
AsyncWebServer server(80);

//user variable
long lastMsg = 0;

String dataMQTT = "";

//pzem measurement infor
float fVoltage = 0;
float fAmp = 0;

//convert pzem measurement to str
char chVol[20];
char chAmp[20];


//merge the pzem measurement to out
char chVolOut[50];
char chAmpOut[50];

void mqttHandle()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  dataMQTT = "";
  dataMQTT = "{\"Vol\":\"" + String(fVoltage) + "\"," +
                     "\"Amp\":\"" + String(fAmp) + "\"}";

//  Serial.print("dataMQTT:");
//  Serial.println(dataMQTT);
  
  client.publish(mqtt_topic_pub.c_str(), dataMQTT.c_str());

}

void setup(void) {
  u8g2.begin();
  Serial.begin(115200);
  setup_wifi();
  otaUpdate();

  client.setServer(mqtt_server, mqtt_port); 
}

void loop(void) {
  u8g2.clearBuffer();
  u8g2_prepare();
  updatePzem();
  converToStr();

  u8g2.drawStr( 0, 2, "     Kieu Van Loc");
  u8g2.drawStr( 0, 13, "      2018604427");
  u8g2.drawStr( 0, 25, " Thong So Cua Dong Co");
  u8g2.drawStr( 0, 40, chVolOut);
  u8g2.drawStr( 0, 53, chAmpOut);
  u8g2.sendBuffer();
  mqttHandle(); 
  delay(3000);
}


//timer function


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
      Serial.println("Connected to Broker");
      digitalWrite(16, HIGH);
    }
    else
    {
      Serial.println("Không thể kết nối tới MQTT Broker");
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
  
//  if(isnan(fVoltage))
//      fVoltage = 0;
//  if(isnan(fAmp))
//      fAmp = 0;
}


void converToStr()
{
  dtostrf(fVoltage, 6, 2, chVol);
  dtostrf(fAmp, 6, 2, chAmp);
  sprintf(chVolOut, "Dien Ap(V):%s", chVol);
  sprintf(chAmpOut, "Dong Dien(A):%s", chAmp);  
}


void otaUpdate()
{
  AsyncElegantOTA.begin(&server);
  server.begin();
}
