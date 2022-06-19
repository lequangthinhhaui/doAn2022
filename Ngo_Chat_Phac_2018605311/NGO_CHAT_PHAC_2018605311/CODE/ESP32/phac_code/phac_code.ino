#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
//#include <AsyncElegantOTA.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

//ota update
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


#include <PubSubClient.h>
//


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 


const char* ssid = "NCPhac";
const char* password = "12345678";

//AsyncWebServer server(80);


//dht pin define
#define DHTPIN 13    
#define DHTTYPE DHT22  
int doAmDatPin = 36;
int doSangPin = 39;

int doDucNuocPin = 34;
int nongDoKhiDocPin = 35;

//cac thong so cua moi truong
int doAmDat = 0;
int doSang = 0;
float doAmMoiTruong = 0.0;
float nhietDoMoiTruong = 0.0;

int nongDoKhiDoc = 0;
int doDucNuoc = 0;

int nguongDoSang = 10; //pham tram
int nguongDoAmDat = 0;
int nguongNongDoKhiDoc = 10;
int nguongDoDucNuoc = 30;

//khai bao data tu mqtt
int dataInt = 0;

//khai bao dht
DHT dht(DHTPIN, DHTTYPE); 

//khai bao lcd i2c
LiquidCrystal_I2C lcd(0x27, 20, 4); //lcd i2c 20x04

//khai bao esp client mqtt
WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);

//khai bao nut nhan su dung

int setMode = 18 ; 
int dieuKhienQuat = 4; 
int dieuKhienDen = 5; 
int dieuKhienBom = 15; 

//khai bao, khoi tao chan output

int quat = 14;
int den = 27;
int bom = 26;

//khai bao topic
String mqtt_topic_infor_MT = "ngochatphac/inforMoitruong";//publish

String mqtt_topic_nguong_MT = "ngochatphac/nguongMoiTruong";//subcribe

String mqtt_topic_infor_TB = "ngochatphac/inforThietBi";//publish  

String mqtt_topic_control_TB = "ngochatphac/controlThietBi";//subcribe



//khai bao button press counter
int buttonModePressCount = 0;
int buttonQuatPressCount = 0;
int buttonBomPressCount = 0;
int buttonDenPressCount = 0;

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

//ota update
void  otaUpdate();

void readTemHumi(float &temp, float &humi);
void setup() {
  Serial.begin(115200);
  sensorInit();
  pinInit();
  lcdInit();
  setUpWifi();
  taskInit();
  otaUpdate();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
}

void loop() {

  //các thông số về môi trường
//  readTemHumi(nhietDoMoiTruong, doAmMoiTruong);
  doAmDat = 100 - (analogRead(doAmDatPin) * 100) / 4095;
  doSang = (analogRead(doSangPin) * 100) / 4095;
  nongDoKhiDoc = (analogRead(nongDoKhiDocPin) * 100) / 4095;
  doDucNuoc = 100- (analogRead(doDucNuocPin) * 100) / 4095;
  Serial.println(nongDoKhiDoc);
  Serial.println(doDucNuoc);
  //
//int buttonQuatPressCount = 0;
//int buttonBomPressCount = 0;
//int buttonDenPressCount = 0;
  //chế độ điều khiển tự động
if(buttonModePressCount == 1)
{
  if((doAmDat < nguongDoAmDat) && doDucNuoc < nguongDoDucNuoc )
  {
    digitalWrite(bom, HIGH);
    buttonBomPressCount = 1;
  }
  else
  {
   digitalWrite(bom, LOW); 
   buttonBomPressCount = 0; 
  }

  if(doSang < nguongDoSang)
  {
    digitalWrite(den, HIGH);
    buttonDenPressCount = 1;
  }
  else
  {
   digitalWrite(den, LOW);
   buttonDenPressCount = 0;
  }

    
  if(nongDoKhiDoc > nguongNongDoKhiDoc)
  {
    digitalWrite(quat, HIGH);
    buttonQuatPressCount = 1;
  }
  else
  {
    digitalWrite(quat, LOW); 
    buttonQuatPressCount = 0;
  }
   
}

  vTaskDelay(3000 / portTICK_PERIOD_MS);
}

void pinInit()
{
  //pin input
  pinMode(setMode, INPUT); 
  pinMode(dieuKhienQuat, INPUT); 
  pinMode(dieuKhienDen, INPUT); 
  pinMode(dieuKhienBom, INPUT); 

  //pin output
  pinMode(quat, OUTPUT);
  pinMode(den, OUTPUT);
  pinMode(bom, OUTPUT);
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
    lcd.setCursor(0,0);
    lcd.print("dang ket noi wifi    ");
  }
  lcd.setCursor(0,0);
  lcd.print("Ket noi thanh cong         ");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(1000);
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
  handleButtonQuatTask();
  handleButtonDenTask();
  handleButtonBomTask();
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
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
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
//button den handle
void handleButtonDenTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonDen,  // Function to be called
    "handleButtonDen",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButtonDen(void *parameter) {
  while (1) {
    while (digitalRead(dieuKhienDen) == HIGH);
    while (digitalRead(dieuKhienDen) == LOW);
    buttonDenPressCount ++;
    if (buttonDenPressCount == 2)
    {
      buttonDenPressCount = 0;
    }
    if(buttonModePressCount == 0)
    {
      if (buttonDenPressCount == 1)
      {
        digitalWrite(den, HIGH);
        client.publish(mqtt_topic_infor_TB.c_str(), "21");
      }
      else if (buttonDenPressCount == 0)
      {
        digitalWrite(den, LOW);
        client.publish(mqtt_topic_infor_TB.c_str(), "20");
      }      
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
//button bom handle

void handleButtonBomTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonBom,  // Function to be called
    "handleButtonBom",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButtonBom(void *parameter) {
  while (1) {
    while (digitalRead(dieuKhienBom) == HIGH);
    while (digitalRead(dieuKhienBom) == LOW);
    buttonBomPressCount ++;
    if (buttonBomPressCount == 2)
    {
      buttonBomPressCount = 0;
    }
    if(buttonModePressCount == 0)
    {
      if (buttonBomPressCount == 1)
      {
        digitalWrite(bom, HIGH);
        client.publish(mqtt_topic_infor_TB.c_str(), "31");
      }
      else if (buttonBomPressCount == 0)
      {
        digitalWrite(bom, LOW);
        client.publish(mqtt_topic_infor_TB.c_str(), "30");
      }      
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}


//button quat handle
void handleButtonQuatTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButtonQuat,  // Function to be called
    "handleButton1",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButtonQuat(void *parameter) {
  while (1) {
    while (digitalRead(dieuKhienQuat) == HIGH);
    while (digitalRead(dieuKhienQuat) == LOW);
    buttonQuatPressCount ++;
    if (buttonQuatPressCount == 2)
    {
      buttonQuatPressCount = 0;
    }
    if(buttonModePressCount == 0)
    {
      if (buttonQuatPressCount == 1)
      {
        digitalWrite(quat, HIGH);
        client.publish(mqtt_topic_infor_TB.c_str(), "41");
      }
      else if (buttonQuatPressCount == 0)
      {
        digitalWrite(quat, LOW);
        client.publish(mqtt_topic_infor_TB.c_str(), "40");
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

  int modeSub = 0;
  while (1) {
    
    nhietDoMoiTruong = dht.readTemperature();
    doAmMoiTruong = dht.readHumidity();  
    
    if(buttonModePressCount == 1)
    {
      if(digitalRead(dieuKhienBom) == LOW)
      {
        modeSub = 0;
      }
      else if(digitalRead(dieuKhienQuat) == LOW)
      {
        modeSub = 1;
      }
      else if(digitalRead(dieuKhienDen) == LOW)
      {
        modeSub = 2;
      }

      
      if(modeSub == 0)
      {
        lcd.setCursor(0, 0);
        char cANhietDo[20];
        char cADoAm[20];
        sprintf(cANhietDo, "Nhiet do: %0.2f C   ", nhietDoMoiTruong);
        sprintf(cADoAm, "Do am: %0.2f %c    ", doAmMoiTruong, '%');
        lcd.print(cANhietDo);
        lcd.setCursor(0, 1);
        lcd.print(cADoAm);
        lcd.setCursor(0, 2); 
        char cADoAmDat[20];
        sprintf(cADoAmDat, "Do am dat: %d %c     ", doAmDat, '%');
        lcd.print(cADoAmDat);
        lcd.setCursor(0, 3); 
        char cADoSang[20];
        sprintf(cADoSang, "Anh sang: %d %c     ", doSang , '%');
        lcd.print(cADoSang);        
      }
      else if(modeSub == 1)
      {
        char cANongDoKhiDoc[20];
        char cADoDucNuoc[20];
        sprintf(cANongDoKhiDoc, "Nong do khi doc:%d%c   ", nongDoKhiDoc, '%');
        sprintf(cADoDucNuoc, "Do duc nuoc: %d %c    ", doDucNuoc, '%');

        lcd.setCursor(0, 0);
        lcd.print(cANongDoKhiDoc);
        
        lcd.setCursor(0, 1);
        lcd.print(cADoDucNuoc);

        
        lcd.setCursor(0, 2); 
        lcd.print("                    ");
        
        lcd.setCursor(0, 3); 
        lcd.print("                    ");
      }
      else if(modeSub == 2)
      {
        lcd.setCursor(0, 0);
        lcd.print("trang thai cua TB   ");
        lcd.setCursor(0, 1);
        if(buttonDenPressCount == 1)
        {
          lcd.print("Den dang duoc bat   ");             
        }
        else
          lcd.print("Den dang duoc tat   "); 
        lcd.setCursor(0, 2);
        
        if(buttonQuatPressCount == 1)
        {
          lcd.print("Quat dang duoc bat  ");             
        }
        else
          lcd.print("Quat dang duoc tat  ");
          lcd.setCursor(0, 3);
        
        if(buttonBomPressCount == 1)
        {
          lcd.print("Bom dang duoc bat   ");             
        }
        else
          lcd.print("Bom dang duoc tat   "); 
      }
    }
    else if(buttonModePressCount == 0)
    {
      lcd.setCursor(0, 0);
      lcd.print("trang thai cua TB   ");
      lcd.setCursor(0, 1);
      if(buttonDenPressCount == 1)
      {
        lcd.print("Den dang duoc bat   ");             
      }
      else
        lcd.print("Den dang duoc tat   "); 
      lcd.setCursor(0, 2);
      
      if(buttonQuatPressCount == 1)
      {
        lcd.print("Quat dang duoc bat  ");             
      }
      else
        lcd.print("Quat dang duoc tat  ");

        lcd.setCursor(0, 3);
      
      if(buttonBomPressCount == 1)
      {
        lcd.print("Bom dang duoc bat   ");             
      }
      else
        lcd.print("Bom dang duoc tat   "); 
    }

    
    //send data to mqtt
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
  doc["dosang"] = doSang;
  doc["doducnuoc"] = doDucNuoc;
  doc["nongdokhidoc"] = nongDoKhiDoc;
  serializeJson(doc, inforMoiTruongBuff);
  client.publish(mqtt_topic_infor_MT.c_str(), inforMoiTruongBuff);

  
  DynamicJsonDocument doc2(1024);
  char inforThietBiBuff[256];
  doc2["mode"] =   buttonModePressCount;
  doc2["quat"] =   buttonQuatPressCount;
  doc2["den"]   =   buttonDenPressCount;
  doc2["bom"] =   buttonBomPressCount;
  doc2["nguongnongdokhidoc"] = nguongNongDoKhiDoc;
  doc2["nguonganhsang"] = nguongDoSang;  
  doc2["nguongdoamdat"] = nguongDoAmDat;
  doc2["nguongdoducnuoc"] = nguongDoDucNuoc;
  
  serializeJson(doc2, inforThietBiBuff);
  client.publish(mqtt_topic_infor_TB.c_str(), inforThietBiBuff);
}

void otaUpdate()
{
  AsyncElegantOTA.begin(&server);
  server.begin();
}

void callback(char* topic, byte* payload, unsigned int length)
{

  /*
   *  10- mode man
      11- mode auto
      20- đèn tắt
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
    }
    else if (dataInt == 11)
    {
       buttonModePressCount = 1;
    }
    if(buttonModePressCount == 0)
    {
      if (dataInt == 20)
      {
         digitalWrite(den, LOW);
         buttonDenPressCount = 0;
      }
      else if (dataInt == 21)
      {
         digitalWrite(den, HIGH);
         buttonDenPressCount = 1;
      }
      else if (dataInt == 30)
      {
         digitalWrite(bom, LOW);
         buttonBomPressCount = 0;
      }
      else if (dataInt == 31)
      {
         digitalWrite(bom, HIGH);
         buttonBomPressCount = 1;
      }
      else if (dataInt == 40)
      {
         digitalWrite(quat, LOW);
         buttonQuatPressCount = 0;
      }
      else if (dataInt == 41)
      {
         digitalWrite(quat, HIGH);
         buttonQuatPressCount = 1;
      }       
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

    if(payload[length-1] == 67)
      nguongDoSang = Data.toFloat(); //pham tram
    else if(payload[length-1] == 66)
      nguongDoAmDat = Data.toFloat();
    else if(payload[length-1] == 65)
      nguongNongDoKhiDoc = Data.toInt(); //float 
    else if(payload[length-1] == 64)
      nguongDoDucNuoc = Data.toInt(); //float 
  }
}
