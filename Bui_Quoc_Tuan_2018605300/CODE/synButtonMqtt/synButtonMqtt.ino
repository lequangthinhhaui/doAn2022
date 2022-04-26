#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
//#include <Button.h>

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

String mqtt_topic_pub = "buiquoctuan/btncontrol";
String mqtt_topic_sub = "buiquoctuan/btnrespon";

//user variable
long lastMsg = 0;
char msg[50];
int value = 0;

int dataInt = 0;

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
void callback(char* topic, byte* payload, unsigned int length);
void setupWifi();
void reconnect();

//task init
void taskInit();

//button 1 task
void handleButton1Task();
void handleButton1(void *parameter);

//button 2 task
void handleButton2Task();
void handleButton2(void *parameter);

//button 3 task
void handleButton3Task();
void handleButton3(void *parameter);

//button 4 task
void handleButton4Task();
void handleButton4(void *parameter);

//button 5 task
void handleButton5Task();
void handleButton5(void *parameter);

//button 6 task
void handleButton6Task();
void handleButton6(void *parameter);

//mqtt handle
void handleMQTTTask();
void handleMQTT(void *parameter);


void setup() {
  Serial.begin(115200);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  pinInit(6, 6);
  taskInit();
  setupWifi();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void pinInit(int pinNumberInput, int pinNumberOnput)
{
  for (int i = 0; i < pinNumberInput; i ++)
  {
    pinMode(inputPinArr[i], INPUT);
  }
  for (int i = 0; i < pinNumberOnput; i ++)
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
  handleMQTTTask();
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

void handleButto4Task()
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

void handleButton5Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton5,  // Function to be called
    "handleButton5",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleButton6Task()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleButton6,  // Function to be called
    "handleButton6",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}


void handleMQTTTask()
{
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
    handleMQTT,  // Function to be called
    "handleMQTT",   // Name of task
    2048,         // Stack size (bytes in ESP32, words in FreeRTOS)
    NULL,         // Parameter to pass to function
    1,            // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,         // Task handle
    1);
}

void handleButton1(void *parameter) {
  while (1) {
    while (digitalRead(BTN_01) == HIGH);
    while (digitalRead(BTN_01) == LOW);
    button1PressCount ++;
    if (button1PressCount == 2)
    {
      button1PressCount = 0;
    }
    if (button1PressCount == 0)
    {
      digitalWrite(OUT_01, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "11");
    }
    else if (button1PressCount == 1)
    {
      digitalWrite(OUT_01, LOW);
      client.publish(mqtt_topic_pub.c_str(), "10");
    }
  }
}

//handle button 2

void handleButton2(void *parameter) {
  while (1) {
    while (digitalRead(BTN_02) == HIGH);
    while (digitalRead(BTN_02) == LOW);
    button2PressCount ++;
    if (button2PressCount == 2)
    {
      button2PressCount = 0;
    }
    if (button2PressCount == 0)
    {
      digitalWrite(OUT_02, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "21");
    }
    else if (button2PressCount == 1)
    {
      digitalWrite(OUT_02, LOW);
      client.publish(mqtt_topic_pub.c_str(), "20");
    }
  }
}


//handle button 3

void handleButton3(void *parameter) {
  while (1) {
    while (digitalRead(BTN_03) == HIGH);
    while (digitalRead(BTN_03) == LOW);
    button3PressCount ++;
    if (button3PressCount == 2)
    {
      button3PressCount = 0;
    }
    if (button3PressCount == 0)
    {
      digitalWrite(OUT_03, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "31");
    }
    else if (button3PressCount == 1)
    {
      digitalWrite(OUT_03, LOW);
      client.publish(mqtt_topic_pub.c_str(), "30");
    }
  }
}

void handleButton4(void *parameter) {
  while (1) {
    while (digitalRead(BTN_04) == HIGH);
    while (digitalRead(BTN_04) == LOW);
    button4PressCount ++;
    if (button4PressCount == 2)
    {
      button4PressCount = 0;
    }
    if (button4PressCount == 0)
    {
      digitalWrite(OUT_04, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "41");
    }
    else if (button4PressCount == 1)
    {
      digitalWrite(OUT_04, LOW);
      client.publish(mqtt_topic_pub.c_str(), "40");
    }
  }
}


void handleButton5(void *parameter) {
  while (1) {
    while (digitalRead(BTN_05) == HIGH);
    while (digitalRead(BTN_05) == LOW);
    button5PressCount ++;
    if (button5PressCount == 2)
    {
      button5PressCount = 0;
    }
    if (button5PressCount == 0)
    {
      digitalWrite(OUT_05, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "51");
    }
    else if (button5PressCount == 1)
    {
      digitalWrite(OUT_05, LOW);
      client.publish(mqtt_topic_pub.c_str(), "50");
    }
  }
}

void handleButton6(void *parameter) {
  while (1) {
    while (digitalRead(BTN_06) == HIGH);
    while (digitalRead(BTN_06) == LOW);
    button6PressCount ++;
    if (button6PressCount == 2)
    {
      button6PressCount = 0;
    }
    if (button6PressCount == 0)
    {
      digitalWrite(OUT_06, HIGH);
      client.publish(mqtt_topic_pub.c_str(), "61");
    }
    else if (button6PressCount == 1)
    {
      digitalWrite(OUT_06, LOW);
      client.publish(mqtt_topic_pub.c_str(), "60");
    }
  }
}



void handleMQTT(void *parameter) {
  while (1)
  {

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

  Data = "";
  for (int i = 0; i < length; i++)
  {
    Data += (char)payload[i]; // abcde
  }
  dataInt = Data.toInt();
  Serial.println(dataInt);
  ///button 1
  if (dataInt == 11)
  {
    button1PressCount = 1;
    digitalWrite(OUT_01, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "11");
  }
  else if (dataInt == 10)
  {
    button1PressCount = 0;
    digitalWrite(OUT_01, LOW);
    client.publish(mqtt_topic_pub.c_str(), "10");
  }
  ///button 2
  else if (dataInt == 21)
  {
    button2PressCount = 1;
    digitalWrite(OUT_02, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "21");
  }
  else if (dataInt == 20)
  {
    button2PressCount = 0;
    digitalWrite(OUT_02, LOW);
    client.publish(mqtt_topic_pub.c_str(), "20");
  }
  ///button 3
  else if (dataInt == 31)
  {
    button2PressCount = 1;
    digitalWrite(OUT_03, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "31");
  }
  else if (dataInt == 30)
  {
    button3PressCount = 0;
    digitalWrite(OUT_03, LOW);
    client.publish(mqtt_topic_pub.c_str(), "30");
  }

  ///button 4
  else if (dataInt == 41)
  {
    button4PressCount = 1;
    digitalWrite(OUT_04, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "41");
  }
  else if (dataInt == 40)
  {
    button4PressCount = 0;
    digitalWrite(OUT_04, LOW);
    client.publish(mqtt_topic_pub.c_str(), "40");
  }
  ///button 5
  else if (dataInt == 51)
  {
    button5PressCount = 1;
    digitalWrite(OUT_05, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "51");
  }
  else if (dataInt == 50)
  {
    button5PressCount = 0;
    digitalWrite(OUT_05, LOW);
    client.publish(mqtt_topic_pub.c_str(), "50");
  }

  ///button 6
  else if (dataInt == 61)
  {
    button6PressCount = 1;
    digitalWrite(OUT_06, HIGH);
    client.publish(mqtt_topic_pub.c_str(), "61");
  }
  else if (dataInt == 60)
  {
    button6PressCount = 0;
    digitalWrite(OUT_06, LOW);
    client.publish(mqtt_topic_pub.c_str(), "60");
  }
}

//set up wifi
void setupWifi() {
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
