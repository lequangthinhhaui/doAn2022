
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//ota update
 #include <ESPAsyncWebServer.h>
 #include <AsyncElegantOTA.h>


#define ssid "dieukhien"
#define password "123456788"


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_control_TB = "nguyenvanvinh/controlTB"; //subscribe nhan ve
String mqtt_topic_infor_TB_ALL = "nguyenvanvinh/inforTBALL";//publish
String mqtt_topic_infor_TB_Relay = "nguyenvanvinh/inforTBRelay"; //publish gui di

WiFiClient espClient;
PubSubClient client(espClient);


AsyncWebServer server(80);

//function define 
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void reconnect();

//rtos define khoi tao nhiêu luong
void taskInit();

//ota update
void  otaUpdate();


//user var define khai bao cac bien
String Data = "";
uint16_t dataInt = 0;
float dataFloat = 0;
uint16_t dataLight = 0;



//button variable vao
int den3Pin = 15;
int den2Pin = 4;
int den1Pin = 16;
int den4Pin = 17;
int denAllPin = 5;
int remPin = 18;


//output pin 
int den1OutPin = 25;
int den2OutPin = 32;
int den3OutPin = 23;
int den4OutPin = 26;
int den5OutPin = 33;


int l298COutPin = 14;
int l298DOutPin = 27;


int button1PressCount = 0;
int button2PressCount = 0;
int button3PressCount = 0;
int button4PressCount = 0;
int button5PressCount = 0;
int button6PressCount = 0;



int PWMchannel0 = 0;
int PWMchannel1 = 1;

// nguyen mau khoi tao ham
void l298nInit();
void pinInit();

void rotateClock();
void rotateNotClock();

//******************KHAI BAO NGUYEN MAU RTOS******************
//khai bao ham nguyen mau cua cac nut nhan rtos
void handleButton1Task();
void handleButton2Task();
void handleButton3Task();
void handleButton4Task();
void handleButton5Task();
void handleButton6Task();
void handleButton1(void *parameter);
void handleButton2(void *parameter);
void handleButton3(void *parameter);
void handleButton4(void *parameter);
void handleButton5(void *parameter);
void handleButton6(void *parameter);
//khai bao ham nguyen mau cua cac nut nhan rtos

//khai bao ham gui du lieu len mqtt
void sendMqttTask();
void sendMqtt(void *parameter);
void sendDataMQTT();
//khai bao ham gui du lieu len mqtt

//khai bao ham doc cam bien va xu ly cam bien
void handleAutoTask();
void handleAuto(void *parameter);
//khai bao ham doc cam bien va xu ly cam bien

//******************KHAI BAO NGUYEN MAU RTOS******************



void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinInit();
  l298nInit();
  setup_wifi();
  otaUpdate();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  taskInit();
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length)
{

  String Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; 
  }


  dataInt = Data.toInt();

  if(strcmp(topic, mqtt_topic_control_TB.c_str()) == 0)
  {
    String Data = "";
    for (int i = 0; i < length; i++)
    {
      Data += (char)payload[i]; 
    }
    dataInt = Data.toInt();
    Serial.println(dataInt);

    ///button 1
    if (dataInt == 10)
    {
       button1PressCount = 0;
       digitalWrite(den1OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
    }
    else if (dataInt == 11)
    {
       button1PressCount = 1;
       digitalWrite(den1OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");
    }
        ///button 2
    else if (dataInt == 20)
    {
       button2PressCount = 0;
       digitalWrite(den2OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
    }
    else if (dataInt == 21)
    {
       button2PressCount = 1;
       digitalWrite(den2OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
    }
        ///button 3
    else if (dataInt == 30)
    {
       button3PressCount = 0;
       digitalWrite(den3OutPin, LOW);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
    }
    else if (dataInt == 31)
    {
       button3PressCount = 1;
       digitalWrite(den3OutPin, HIGH);
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
    }
        ///button 4
    else if (dataInt == 40)
    {
       digitalWrite(den4OutPin, LOW);
       button4PressCount = 0;
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
    }
    else if (dataInt == 41)
    {
       digitalWrite(den4OutPin, HIGH);
       button4PressCount = 1;
       client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");
    }
         ///button 5
    else if (dataInt == 50)
    {
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "50");
      button5PressCount = 0;
      rotateNotClock();

    }
    else if (dataInt == 51)
    {
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "51");
      button5PressCount = 1;
      rotateClock();
    }
             ///button 6
    else if (dataInt == 60)
    {
      digitalWrite(den1OutPin, LOW);
      button1PressCount = 0;
      digitalWrite(den2OutPin, LOW);
      button2PressCount = 0;
      digitalWrite(den3OutPin, LOW);
      button3PressCount = 0;
      digitalWrite(den4OutPin, LOW);
      button4PressCount = 0;
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "60");
      button6PressCount = 0;
    }
    else if (dataInt == 61)
    {
      digitalWrite(den1OutPin, HIGH);
      button1PressCount = 1;
      digitalWrite(den2OutPin, HIGH);
      button2PressCount = 1;
      digitalWrite(den3OutPin, HIGH);
      button3PressCount = 1;
      digitalWrite(den4OutPin, HIGH);
      button4PressCount = 1;
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "61");
      button6PressCount = 1;
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
      Serial.println("Connected MQTT");
      client.subscribe(mqtt_topic_control_TB.c_str());
    }
    else
    {
      Serial.println("Not Connnected MQTT");
      delay(3000);
    }
  }
}



//************************gui du lieu len mqtt*************************
void sendMqttTask()
{
  xTaskCreatePinnedToCore(  
    sendMqtt,  
    "sendMqtt", 
    2048,         
    NULL,         
    1,            
    NULL,       
    1);
}

void sendMqtt(void *parameter) {
  while (1) {
  sendDataMQTT();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void sendDataMQTT()
{
  DynamicJsonDocument doc(1024);
  char inforInverterBuff[256];
  doc["den1"] = button1PressCount;
  doc["den2"]   =  button2PressCount;
  doc["den3"] =  button3PressCount;
  doc["den4"] =  button4PressCount;
  doc["rem"]   =  button5PressCount;
  doc["denALL"]   =  button6PressCount;
  serializeJson(doc, inforInverterBuff);
  client.publish(mqtt_topic_infor_TB_ALL.c_str(), inforInverterBuff);
}
//************************gui du lieu len mqtt*************************


//************************xu ly nut nhan 01*************************
void handleButton1Task()
{
  xTaskCreatePinnedToCore(  
    handleButton1,  
    "handleButton1",   
    2048,         
    NULL,         
    1,            
    NULL,        
    1);
}

void handleButton1(void *parameter) {
  while (1) {
    while (digitalRead(den1Pin) == HIGH);
    while (digitalRead(den1Pin) == LOW);
    button1PressCount ++;
    if (button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if (button1PressCount == 0)
    {
        digitalWrite(den1OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
    }
    else if (button1PressCount == 1)
    {
        digitalWrite(den1OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");

    }
  }
}
//************************xu ly nut nhan 01*************************

//************************xu ly nut nhan 02*************************
void handleButton2Task()
{
  xTaskCreatePinnedToCore(  
    handleButton2,  
    "handleButton2",   
    2048,         
    NULL,         
    1,            
    NULL,         
    1);
}

void handleButton2(void *parameter) {
  while (1) {
    while (digitalRead(den2Pin) == HIGH);
    while (digitalRead(den2Pin) == LOW);
    button2PressCount ++;
    if (button2PressCount == 2)
    {
      button2PressCount = 0;
    }
    if (button2PressCount == 0)
    {
        digitalWrite(den2OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
    }
    else if (button2PressCount == 1)
    {
        digitalWrite(den2OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
    }
  }
}
//************************xu ly nut nhan 02*************************


//************************xu ly nut nhan 03*************************
void handleButton3Task()
{
  xTaskCreatePinnedToCore( 
    handleButton3, 
    "handleButton3",  
    2048,       
    NULL,      
    1,           
    NULL,       
    1);
}
void handleButton3(void *parameter) {
  while (1) {
    while (digitalRead(den3Pin) == HIGH);
    while (digitalRead(den3Pin) == LOW);
    button3PressCount ++;
    if (button3PressCount == 2)
    {
      button3PressCount = 0;
    }
    if (button3PressCount == 0)
    {
        digitalWrite(den3OutPin, LOW);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
    }
    else if (button3PressCount == 1)
    {
        digitalWrite(den3OutPin, HIGH);
        client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
    }
  }
}
//************************xu ly nut nhan 03*************************


//************************xu ly nut nhan 04*************************
void handleButton4Task()
{
  xTaskCreatePinnedToCore( 
    handleButton4,  
    "handleButton4",  
    2048,        
    NULL,       
    1,          
    NULL,        
    1);
}

void handleButton4(void *parameter) {
  while (1) {
    while (digitalRead(den4Pin) == HIGH);
    while (digitalRead(den4Pin) == LOW);
    button4PressCount ++;
    if (button4PressCount == 2)
    {
      button4PressCount = 0;
    }
    if (button4PressCount == 0)
    {
      digitalWrite(den4OutPin, LOW);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
    }
    else if (button4PressCount == 1)
    {
      digitalWrite(den4OutPin, HIGH);
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");

    }
  }
}
//************************xu ly nut nhan 04*************************


//************************xu ly nut nhan 05*************************
void handleButton5Task()
{
  xTaskCreatePinnedToCore(  
    handleButton5, 
    "handleButton5",   
    2048,        
    NULL,         
    1,            
    NULL,         
    1);
}

void handleButton5(void *parameter) {
  while (1) {
    while (digitalRead(remPin) == HIGH);
    while (digitalRead(remPin) == LOW);
    button5PressCount ++;
    if (button5PressCount == 2)
    {
      button5PressCount = 0;
    }
    if (button5PressCount == 0)
    {
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "50");
      rotateNotClock();
    }
    else if (button5PressCount == 1)
    {
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "51");
      rotateClock();
    }
  }
}
//************************xu ly nut nhan 05*************************


//************************xu ly nut nhan 06*************************
void handleButton6Task()
{
  xTaskCreatePinnedToCore(  
    handleButton6,  
    "handleButton6",   
    2048,         
    NULL,         
    1,            
    NULL,        
    1);
}
void handleButton6(void *parameter) {
  while (1) {
    while (digitalRead(denAllPin) == HIGH);
    while (digitalRead(denAllPin) == LOW);
    button6PressCount ++;
    if (button6PressCount == 2)
    {
      button6PressCount = 0;
    }
    if (button6PressCount == 0)
    {
      digitalWrite(den1OutPin, LOW);
      button1PressCount = 0;
      digitalWrite(den2OutPin, LOW);
      button2PressCount = 0;
      digitalWrite(den3OutPin, LOW);
      button3PressCount = 0;
      digitalWrite(den4OutPin, LOW);
      button4PressCount = 0;
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "10");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "20");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "30");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "40");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "60");
    }
    else if (button6PressCount == 1)
    {
      digitalWrite(den1OutPin, HIGH);
      button1PressCount = 1;
      digitalWrite(den2OutPin, HIGH);
      button2PressCount = 1;
      digitalWrite(den3OutPin, HIGH);
      button3PressCount = 1;
      digitalWrite(den4OutPin, HIGH);
      button4PressCount = 1;
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "11");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "21");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "31");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "41");
      client.publish(mqtt_topic_infor_TB_Relay.c_str(), "61");
    }
  }
}
//************************xu ly nut nhan 06*************************


//************************xu ly den sang theo cam bien*************************
void handleAutoTask()
{
  xTaskCreatePinnedToCore(  
    handleAuto,  
    "handleAuto",  
    2048,       
    NULL,        
    5,            
    NULL,         
    1);
}

void handleAuto(void *parameter) {
  while (1) {
    dataLight = analogRead(36);
    if(dataLight <= 10)
    {
        digitalWrite(den5OutPin, HIGH);
    }
    else if(dataLight > 10)
    {
        digitalWrite(den5OutPin, LOW);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//************************xu ly den sang theo cam bien*************************



void taskInit()
{
  sendMqttTask();
  handleAutoTask();
  handleButton1Task();
  handleButton2Task();
  handleButton3Task();
  handleButton4Task();
  handleButton5Task();
  handleButton6Task();
}


 void otaUpdate()
 {
   AsyncElegantOTA.begin(&server);
   server.begin();
 }


void pinInit()
{
  pinMode(den1Pin, INPUT);
  pinMode(den2Pin, INPUT);
  pinMode(den3Pin, INPUT);
  pinMode(den4Pin, INPUT);
  pinMode(denAllPin, INPUT);
  pinMode(remPin, INPUT);

  pinMode(den1OutPin, OUTPUT);
  pinMode(den2OutPin, OUTPUT);
  pinMode(den3OutPin, OUTPUT);
  pinMode(den4OutPin, OUTPUT);
  pinMode(den5OutPin, OUTPUT);

  
  pinMode(l298COutPin, OUTPUT);
  pinMode(l298DOutPin, OUTPUT);
  
  digitalWrite(l298COutPin, LOW);
  digitalWrite(l298DOutPin, LOW);
}

void l298nInit()
{
  ledcSetup(PWMchannel0, 5000, 8);
  ledcSetup(PWMchannel1, 5000, 8);
  ledcAttachPin(l298COutPin, 0);
  ledcAttachPin(l298DOutPin, 1);
}

void rotateClock()
{
  for(int dutyCycle = 50; dutyCycle <= 255; dutyCycle++)
  {   
    ledcWrite(PWMchannel0, dutyCycle);
    ledcWrite(PWMchannel1, 0);
    delay(15);  
  }
    ledcWrite(PWMchannel0, 0);
    ledcWrite(PWMchannel1, 0);
}

void rotateNotClock()
{
  for(int dutyCycle = 50; dutyCycle <= 255; dutyCycle++)
  {   
    ledcWrite(PWMchannel0, 0);
    ledcWrite(PWMchannel1, dutyCycle);
    delay(15);  
  }
    ledcWrite(PWMchannel0, 0);
    ledcWrite(PWMchannel1, 0);
}
