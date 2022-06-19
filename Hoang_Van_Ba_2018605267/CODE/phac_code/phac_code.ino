#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
//#include <AsyncElegantOTA.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

//ota update
#include <WiFi.h>
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>
#include <PubSubClient.h>
//


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 


const char* ssid = "HoangBa";
const char* password = "12345678";

//AsyncWebServer server(80);


//dht pin define
#define DHTPIN 13    
#define DHTTYPE DHT22  
int doAmDatPin = 36;
int doSangPin = 39;
//cac thong so cua moi truong
int doAmDat = 0;
int doSang = 0;
float doAmMoiTruong = 0.0;
float nhietDoMoiTruong = 0.0;

int nguongDoSang = 80; //pham tram
float nguongNhietDoMoiTruong = 32; //float 
int nguongDoAmDat = 0;

//khai bao data tu mqtt
int dataInt = 0;

//khai bao dht
DHT dht(DHTPIN, DHTTYPE); 

//khai bao lcd i2c
LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd i2c 20x04

//khai bao esp client mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//khai bao nut nhan su dung

int setMode = 15 ; 
int dieuKhienBom1 = 4; 
int dieuKhienBom2 = 23; 

//khai bao, khoi tao chan output

int bom1 = 14;
int bom2 = 27;

//khai bao topic
String mqtt_topic_infor_MT = "hoangBa/inforMoitruong";//publish

String mqtt_topic_nguong_MT = "hoangBa/nguongMoiTruong";//subcribe

String mqtt_topic_infor_TB = "hoangBa/inforThietBi";//publish  

String mqtt_topic_control_TB = "hoangBa/controlThietBi";//subcribe



//khai bao button press counter
int buttonModePressCount = 0;
int buttonBom1PressCount = 0;
int buttonBom2PressCount = 0;

//define function
void pinInit();
void sensorInit();
void lcdInit();
void setUpWifi();
void setUpOTA();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

//rtos
void taskInit();

//handle button
void handleButtonBomTask();

void readTemHumi(float &temp, float &humi);
void setup() {
  Serial.begin(115200);
  sensorInit();
  pinInit();
  lcdInit();
  setUpWifi();
  taskInit();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
}

void loop() {

  //các thông số về môi trường
//  readTemHumi(nhietDoMoiTruong, doAmMoiTruong);
  doAmDat = 100 - (analogRead(doAmDatPin) * 100) / 4095;
  doSang = (analogRead(doSangPin) * 100) / 4095;

  //chế độ điều khiển tự động
  if(buttonModePressCount == 1)
  {
    if(doAmDat < nguongDoAmDat)
    {
      digitalWrite(bom1, HIGH);
    }
    else
      digitalWrite(bom1, LOW);
      
    if(nhietDoMoiTruong > nguongNhietDoMoiTruong)
    {
      digitalWrite(bom2, HIGH);
    }
    else
      digitalWrite(bom2, LOW);    
  }
  vTaskDelay(3000 / portTICK_PERIOD_MS);
}

void pinInit()
{
  //pin input
  pinMode(setMode, INPUT); 
  pinMode(dieuKhienBom1, INPUT); 
  pinMode(dieuKhienBom2, INPUT); 

  //pin output
  pinMode(bom1, OUTPUT);
  pinMode(bom2, OUTPUT);
}

void lcdInit()
{
  lcd.init();                   
  lcd.backlight();  
}

void sensorInit()
{
  dht.begin();  
}

void readTemHumi(float &temp, float &humi)
{
  temp = dht.readTemperature();
  humi = dht.readHumidity();  
}

void setUpWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}

void taskInit()
{

  //
  sendMqttTask();
  sendValueTask();
  mqttLoopTask();

  //handle button task
  handleButtonbom1Task();
  handleButtonbom2Task();
  handleButtonModeTask();
}

void mqttLoopTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    mqttLoop,  // Function to be called
    "mqttLoop",   // Name of task
    4096,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void mqttLoop(void *parameter) {
   vTaskDelay(200 / portTICK_PERIOD_MS); 
  while (1) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  vTaskDelay(200 / portTICK_PERIOD_MS); 
  } 
}

void sendMqttTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendMqtt,  // Function to be called
    "sendMqtt",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void sendMqtt(void *parameter) {
  while (1) {
  sendDataMQTT();
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}


//button mode handle
void handleButtonModeTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonMode,  // Function to be called
    "handleButtonMode",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButtonMode(void *parameter) {
  while (1) {
    while (digitalRead(setMode) == HIGH);
    while (digitalRead(setMode) == LOW);
    buttonModePressCount ++;
    if (buttonModePressCount == 2)
    {
      buttonModePressCount = 0;
    }
    if (buttonModePressCount == 1)
    {
      client.publish(mqtt_topic_infor_TB.c_str(), "11");
    }
    else if (buttonModePressCount == 0)
    {
      client.publish(mqtt_topic_infor_TB.c_str(), "10");
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
//button bom2 handle
void handleButtonbom2Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonbom2,  // Function to be called
    "handleButtonbom2",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

//button bom1 handle
void handleButtonbom1Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonbom1,  // Function to be called
    "handleButton1",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButtonbom1(void *parameter) {
  while (1) {
    while (digitalRead(dieuKhienBom1) == HIGH);
    while (digitalRead(dieuKhienBom1) == LOW);
    buttonBom1PressCount ++;
    if (buttonBom1PressCount == 2)
    {
      buttonBom1PressCount = 0;
    }
    if(buttonModePressCount == 0)
    {
      if (buttonBom1PressCount == 1)
      {
        digitalWrite(bom1, HIGH);
        client.publish(mqtt_topic_infor_TB.c_str(), "21");
      }
      else if (buttonBom1PressCount == 0)
      {
        digitalWrite(bom1, LOW);
        client.publish(mqtt_topic_infor_TB.c_str(), "20");
      }      
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void handleButtonbom2(void *parameter) {
  while (1) {
    while (digitalRead(dieuKhienBom2) == HIGH);
    while (digitalRead(dieuKhienBom2) == LOW);
    buttonBom2PressCount ++;
    if (buttonBom2PressCount == 2)
    {
      buttonBom2PressCount = 0;
    }
    if(buttonModePressCount == 0)
    {
      if (buttonBom2PressCount == 1)
      {
        digitalWrite(bom2, HIGH);
        client.publish(mqtt_topic_infor_TB.c_str(), "31");
      }
      else if (buttonBom2PressCount == 0)
      {
        digitalWrite(bom2, LOW);
        client.publish(mqtt_topic_infor_TB.c_str(), "30");
      }      
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}



void sendValueTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    sendValue,  // Function to be called
    "sendValue",   // Name of task
    4096,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    5,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void sendValue(void *parameter) {
  while (1) {
    nhietDoMoiTruong = dht.readTemperature();
    doAmMoiTruong = dht.readHumidity();  
    lcd.setCursor(0, 0);
    char cANhietDo[20];
    char cADoAm[20];
    sprintf(cANhietDo, "Nhiet do: %0.2f C", nhietDoMoiTruong);
    sprintf(cADoAm, "Do am: %0.2f %c ", doAmMoiTruong, '%');
    lcd.print(cANhietDo);
    lcd.setCursor(0, 1);
    lcd.print(cADoAm);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void reconnect()
{

  while (!client.connected())
  {
    String clientId = String(random(0xffff), HEX); 
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd))
    {
      Serial.println("Connected MQTT");
      client.subscribe( mqtt_topic_nguong_MT.c_str());
      client.subscribe( mqtt_topic_control_TB.c_str());
//    return client.connected();
    }
    else
    {
      Serial.println("Không thể kết nối MQTT ");
      delay(3000);
    }
  }
}

void sendDataMQTT()
{
  DynamicJsonDocument doc(1024);
  char inforMoiTruongBuff[256];
  doc["nhietdo"] = nhietDoMoiTruong;
  doc["doammoitruong"]   = doAmMoiTruong;
  doc["doamdat"] = doAmDat;
  serializeJson(doc, inforMoiTruongBuff);
  client.publish(mqtt_topic_infor_MT.c_str(), inforMoiTruongBuff);

  
  DynamicJsonDocument doc2(1024);
  char inforThietBiBuff[256];
  doc2["mode"] =   buttonModePressCount;
  doc2["bom1"] =   buttonBom1PressCount;
  doc2["bom2"]   =   buttonBom2PressCount;
  doc2["nguongnhietdo"] = nguongNhietDoMoiTruong;
  doc2["nguongdoamdat"] = nguongDoAmDat;
  serializeJson(doc2, inforThietBiBuff);
  client.publish(mqtt_topic_infor_TB.c_str(), inforThietBiBuff);
}

void callback(char* topic, byte* payload, unsigned int length)
{

  /*
   *  10- mode man
      11- mode auto
      20- bom1 tat
      21- đèn bật
      30 - bơm tắt
      31 - bơm bật
      40- quạt tắt
      41 - quạt bật
   */

  if(strcmp(topic, mqtt_topic_control_TB.c_str()) == 0)
  {
    String Data = "";
    for (int i = 0; i < length; i++)
    {
      Data += (char)payload[i]; // abcde
    }
    dataInt = Data.toInt();
    Serial.println(dataInt);
    ///button set mode
    if (dataInt == 10)
    {
       buttonModePressCount = 0;
       client.publish(mqtt_topic_infor_TB.c_str(), "10");
    }
    else if (dataInt == 11)
    {
       buttonModePressCount = 1;
       client.publish(mqtt_topic_infor_TB.c_str(), "11");
    }
    else if (dataInt == 20)
    {
       digitalWrite(bom1, LOW);
       buttonBom1PressCount = 0;
       client.publish(mqtt_topic_infor_TB.c_str(), "20");
    } 
    else if (dataInt == 21)
    {
       digitalWrite(bom1, HIGH);
       buttonBom1PressCount = 1;
       client.publish(mqtt_topic_infor_TB.c_str(), "21");
    }
    else if (dataInt == 30)
    {
       digitalWrite(bom2, LOW);
       buttonBom2PressCount = 0;
       client.publish(mqtt_topic_infor_TB.c_str(), "30");
    } 
    else if (dataInt == 31)
    {
       digitalWrite(bom2, HIGH);
       buttonBom2PressCount = 1;
       client.publish(mqtt_topic_infor_TB.c_str(), "31");
    }  
   }

  else if(strcmp(topic, mqtt_topic_nguong_MT.c_str()) == 0)
  {
    String Data = "";
    for (int i = 0; i < length; i++)
    {
      Data += (char)payload[i]; // abcde
    }
    Serial.println(Data.toFloat());
    Serial.println(payload[length-1]);
    if(payload[length-1] == 65)
      nguongDoAmDat = Data.toFloat();
    else if(payload[length-1] == 66)
      nguongNhietDoMoiTruong = Data.toFloat(); //float 
  }
}
