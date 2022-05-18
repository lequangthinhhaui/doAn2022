/*
  ModbusRTU ESP32
  Concurent thread example
  
  (c)2020 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  Tool Modbus Slave on PC for test
  https://www.modbustools.com/download.html
*/


#include <PubSubClient.h>

#include <WiFi.h>

#include <ArduinoJson.h>





//ota update
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


#define ssid "ducmanh"
#define password "ducmanhdt4"


#define mqtt_server "broker.hivemq.com" 
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_control_TB = "nguyenducmanh/controlTB"; //subscribe

String mqtt_topic_infor_TB = "nguyenducmanh/inforTB";//publish

String mqtt_topic_infor_TB_Now = "nguyenducmanh/inforTBNow"; //publish

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);


// AsyncWebServer server(80);

//function define 
void callback(char* topic, byte* payload, unsigned int length);
void setup_wifi();
void SendDataMQTT();
void reconnect();



//rtos define 
void taskInit();
void sendMqttTask();
void sendMqtt(void *parameter);

//handle task
void sendDataMQTT();

//ota update
void  otaUpdate();

//wifi define
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

//user var define
String Data = "";
uint16_t dataInt = 0;
float dataFloat = 0;



//touch pin 
int touch1pin = 25;
int touch2pin = 26;
int touch3pin = 32;
int touch4pin = 33;


//output pin
int relay1pin = 17;
int relay2pin = 18;
int relay3pin = 19;
int relay4pin = 21;

int button1PressCount = 0;
int button2PressCount = 0;
int button3PressCount = 0;
int button4PressCount = 0;






void pinInit();

///handle button task
void handleButton1Task();
void handleButton2Task();
void handleButton3Task();
void handleButton4Task();




void handleButton1(void *parameter);
void handleButton2(void *parameter);
void handleButton3(void *parameter);
void handleButton4(void *parameter);




//data send
void sendData(void *parameter);

void handleAutoTask();
void handleAuto(void *parameter);



void setup() {
  Serial.begin(115200);
  WiFi.disconnect(true);

  delay(1000);
  

  /* Remove WiFi event
  Serial.print("WiFi Event ID: ");
  Serial.println(eventID);
  WiFi.removeEvent(eventID);*/

  WiFi.begin(ssid, password);
    
  Serial.println();
  Serial.println();
  Serial.println("Wait for WiFi... ");
   pinInit();
//  setup_wifi() ;

   client.setServer(mqtt_server, mqtt_port); 
   client.setCallback(callback);
   taskInit();
    otaUpdate();
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
    Data += (char)payload[i]; // abcde
  }


  dataInt = Data.toInt();

  if(strcmp(topic, mqtt_topic_control_TB.c_str()) == 0)
  {
    String Data = "";
    for (int i = 0; i < length; i++)
    {
      Data += (char)payload[i]; // abcde
    }
    dataInt = Data.toInt();
    Serial.println(dataInt);
    ///button 1
    if (dataInt == 10)
    {
       button1PressCount = 0;
       digitalWrite(relay1pin, LOW);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "10");
    }
    else if (dataInt == 11)
    {
       button1PressCount = 1;
       digitalWrite(relay1pin, HIGH);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "11");
    }
        ///button 2
    else if (dataInt == 20)
    {
       button2PressCount = 0;
       digitalWrite(relay2pin, LOW);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "20");
    }
    else if (dataInt == 21)
    {
       button2PressCount = 1;
       digitalWrite(relay2pin, HIGH);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "21");
    }
        ///button 3
    else if (dataInt == 30)
    {
       button3PressCount = 0;
       digitalWrite(relay3pin, LOW);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "30");
    }
    else if (dataInt == 31)
    {
       button3PressCount = 1;
       digitalWrite(relay3pin, HIGH);
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "31");
    }
        ///button 4
    else if (dataInt == 40)
    {
       digitalWrite(relay4pin, LOW);
       button4PressCount = 0;
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "40");
    }
    else if (dataInt == 41)
    {
       digitalWrite(relay4pin, HIGH);
       button4PressCount = 1;
       client.publish(mqtt_topic_infor_TB_Now.c_str(), "41");
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

void sendDataMQTT()
{
  DynamicJsonDocument doc(1024);
  char inforInverterBuff[256];
  doc["relay1"] = button1PressCount;
  doc["relay2"]   =  button2PressCount;
  doc["relay3"] =  button3PressCount;
  doc["relay4"]   =  button4PressCount;
  serializeJson(doc, inforInverterBuff);
  client.publish(mqtt_topic_infor_TB.c_str(), inforInverterBuff);
}

//rtos task 
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
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


void handleButton1Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton1,  // Function to be called
    "handleButton1",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton2Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton2,  // Function to be called
    "handleButton2",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton3Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton3,  // Function to be called
    "handleButton3",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButton4Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton4,  // Function to be called
    "handleButton4",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}



void handleButton1(void *parameter) {
  while (1) {
    while (digitalRead(touch1pin) == HIGH);
    while (digitalRead(touch1pin) == LOW);
    button1PressCount ++;
    if (button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if (button1PressCount == 0)
    {
        digitalWrite(relay1pin, LOW);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "10");
    }
    else if (button1PressCount == 1)
    {
        digitalWrite(relay1pin, HIGH);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "11");

    }
  }
}

//handle button 2

void handleButton2(void *parameter) {
  while (1) {
    while (digitalRead(touch2pin) == HIGH);
    while (digitalRead(touch2pin) == LOW);
    button2PressCount ++;
    if (button2PressCount == 2)
    {
      button2PressCount = 0;
    }
    if (button2PressCount == 0)
    {
        digitalWrite(relay2pin, LOW);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "20");
    }
    else if (button2PressCount == 1)
    {
        digitalWrite(relay2pin, HIGH);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "21");
    }
  }
}


//handle button 3

void handleButton3(void *parameter) {
  while (1) {
    while (digitalRead(touch3pin) == HIGH);
    while (digitalRead(touch3pin) == LOW);
    button3PressCount ++;
    if (button3PressCount == 2)
    {
      button3PressCount = 0;
    }
    if (button3PressCount == 0)
    {
        digitalWrite(relay3pin, LOW);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "30");
    }
    else if (button3PressCount == 1)
    {
        digitalWrite(relay3pin, HIGH);
        client.publish(mqtt_topic_infor_TB_Now.c_str(), "31");
    }
  }
}


void handleButton4(void *parameter) {
  while (1) {
    while (digitalRead(touch4pin) == HIGH);
    while (digitalRead(touch4pin) == LOW);
    button4PressCount ++;
    if (button4PressCount == 2)
    {
      button4PressCount = 0;
    }
    if (button4PressCount == 0)
    {
      digitalWrite(relay4pin, LOW);
      client.publish(mqtt_topic_infor_TB_Now.c_str(), "40");
    }
    else if (button4PressCount == 1)
    {
      digitalWrite(relay4pin, HIGH);
      client.publish(mqtt_topic_infor_TB_Now.c_str(), "41");

    }
  }
}

void taskInit()
{
  sendMqttTask();
  handleButton1Task();
  handleButton2Task();
  handleButton3Task();
  handleButton4Task();
}



void otaUpdate()
{
  AsyncElegantOTA.begin(&server);
  server.begin();
}




//pin handle
void pinInit()
{

  
  pinMode(touch1pin, INPUT_PULLUP);
  pinMode(touch2pin, INPUT_PULLUP);
  pinMode(touch3pin, INPUT_PULLUP);
  pinMode(touch4pin, INPUT_PULLUP);

  pinMode(relay1pin, OUTPUT);
  pinMode(relay2pin, OUTPUT);
  pinMode(relay3pin, OUTPUT);
  pinMode(relay4pin, OUTPUT);


//choose the positive of touch (fix hardware)
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);

  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(27, HIGH);
///
}
