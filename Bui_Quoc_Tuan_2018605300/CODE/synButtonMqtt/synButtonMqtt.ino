#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Button.h>

//define pin to use
//input
#define BTN_01 15
#define BTN_02 4
#define BTN_03 5
#define BTN_04 18
#define BTN_05 19
#define BTN_06 23

//output
#define OUT_01 14
#define OUT_02 27
#define OUT_03 26
#define OUT_04 25
#define OUT_05 33
#define OUT_06 32

//define wifi
#define ssid "Zoro"//"WIFi bỊ nhiễm virut"
#define password "minhdien04"//"minhdien04"

//define mqtt
#define mqtt_server "broker.hivemq.com"
#define mqtt_user ""    
#define mqtt_pwd ""
const uint16_t mqtt_port = 1883; 

String mqtt_topic_pub = "button/control";   
String mqtt_topic_sub = "button/respon";

//user variable
long lastMsg = 0;
char msg[50];
int value = 0;

String Data = "";
String ChuoiSendWebJson = "";
float nhietdo = 0;

unsigned long last = 0, bien = 0;

//define 
int button1PressCount = 0;
int button2PressCount = 0;
int button3PressCount = 0;
int button4PressCount = 0;
int button5PressCount = 0;
int button6PressCount = 0;


WiFiClient espClient;
PubSubClient client(espClient);

//array of pin
const int inputPinArr[6] = {BTN_01, BTN_02, BTN_03, BTN_04, BTN_05, BTN_06};

const int outputPinArr[6] = {OUT_01, OUT_02, OUT_03, OUT_04, OUT_05, OUT_06};




//function define
void pinInit(int pinNumberInput, int pinNumberOnput);
void buttonInit();
//task init
void taskInit();
//button 1 task
void handleButton1Task();
void handleButton1(void *parameter);

void setup() {
  Serial.begin(9600);
  pinInit(6, 6);
  taskInit();
}

void loop() {
//  for(int i = 0; i<6; i++)
//  {
//    if(digitalRead(inputPinArr[i]) == LOW)
//      digitalWrite(outputPinArr[i], HIGH);
//    else
//      digitalWrite(outputPinArr[i], LOW);
//  }
}

void pinInit(int pinNumberInput, int pinNumberOnput)
{
  for(int i = 0; i < pinNumberInput; i ++)
  {
    pinMode(inputPinArr[i], INPUT);
  }
  for(int i = 0; i < pinNumberOnput; i ++)
  {
    pinMode(outputPinArr[i], OUTPUT);
  }
}

void buttonInit()
{

}
void taskInit()
{
  handleButton1Task();  
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

void handleButton1(void *parameter) {
  while(1) {
    while(digitalRead(BTN_01) == HIGH);
    while(digitalRead(BTN_01) == LOW);
    button1PressCount ++;
    if(button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if(button1PressCount == 0)
    {
      digitalWrite(OUT_01, HIGH);
    }
    else if(buttonPressCount == 1)
    {
      digitalWrite(OUT_01, LOW);
    }
  }
}


//mqtt function

void reconnect()
{

  while (!client.connected())
  {
    String clientId = String(random(0xffff), HEX); 
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd))
    {
      Serial.println("Connected MQTT ngoinhaiot.com");
      client.subscribe(mqtt_topic_sub.c_str());
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

//call back mqtt
void callback(char* topic, byte* payload, unsigned int length)
{

  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }
  Serial.print("Data nhận = ");
  Serial.println(Data);

  if (Data.indexOf("A0B") >= 0)
  {
    Serial.println("OFF thiết bị 1");
    button1PressCount --;
  }
  else if (Data.indexOf("A1B") >= 0)
  {
    Serial.println("ON thiết bị 1");
    digitalWrite(D4,HIGH);
    trangthairelay1 = 1;
  }

  if (Data.indexOf("C0D") >= 0)
  {
    Serial.println("OFF thiết bị 2");
    trangthairelay2 = 0;
    digitalWrite(D5,LOW);
  }
  else if (Data.indexOf("C1D") >= 0)
  {
    Serial.println("ON thiết bị 2");
    trangthairelay2 = 1;
    digitalWrite(D5,HIGH);
  }
  else if (Data.indexOf("E0F") >= 0)
  {
    Serial.println("OFF thiết bị 3");
    trangthairelay3 = 0;
    digitalWrite(D6,LOW);
  }
  else if (Data.indexOf("E1F") >= 0)
  {
    Serial.println("ON thiết bị 3");
    trangthairelay3 = 1;
    digitalWrite(D6,HIGH);
  }
  else if (Data.indexOf("G0H") >= 0)
  {
    Serial.println("OFF thiết bị 4");
    trangthairelay4 = 0;
    digitalWrite(D7,LOW);
  }
  else if (Data.indexOf("G1H") >= 0)
  {
    Serial.println("ON thiết bị 4");
    trangthairelay4 = 1;
    digitalWrite(D7,HIGH);
  }
  Data = "";
  
}
