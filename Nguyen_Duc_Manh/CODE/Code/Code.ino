#include <WiFi.h>
#include <WebServer.h>


//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>



#define TOUCH_PIN1 25
#define TOUCH_PIN2 26
#define TOUCH_PIN3 32
#define TOUCH_PIN4 33

#define RELAY1 17
#define RELAY2 18
#define RELAY3 19
#define RELAY4 21

#define LED1 13
#define LED2 12
#define LED4 14
#define LED3 27

const int threshlod = 8;
bool status1, status2, status3, status4;
int touchValue0,touchValue1,touchValue2,touchValue3 = 0;

// Wifi
char* ssid = "Zoro";
char* pass = "minhdien04";

WebServer server(80);
//AsyncWebServer server2(80);

// Web Interface
//=========Biến chứa mã HTLM Website===========//
const char MainPage[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
          .bt_on {height:50px; width:100px; margin:10px 0;background-color:#FF6600;border-radius:5px;}
          .bt_off {height:50px; width:100px; margin:10px 0;background-color:#00FF00;border-radius:5px;}
       </style>
       <meta charset="UTF-8">
   </head>
   <body> 
      <h1>TOUCH BUTTON SYSTEM</h1>
      
      <div>Relay1 Status: <b><pan id="data1"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onRelay1')">ON</button>
        <button class="bt_off" onclick="getdata('offRelay1')">OFF</button>
      </div>
      
      <div>Relay2 Status: <b><pan id="data2"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onRelay2')">ON</button>
        <button class="bt_off" onclick="getdata('offRelay2')">OFF</button>
      </div>

      <div>Relay3 Status: <b><pan id="data3"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onRelay3')">ON</button>
        <button class="bt_off" onclick="getdata('offRelay3')">OFF</button>
      </div>

      <div>Relay4 Status: <b><pan id="data4"><pan></b></div>
      <div>
        <button class="bt_on" onclick="getdata('onRelay4')">ON</button>
        <button class="bt_off" onclick="getdata('offRelay4')">OFF</button>
      </div>
      
      <script>
        //-----------Tạo đối tượng request----------------
        function create_obj(){
          td = navigator.appName;
          if(td == "Microsoft Internet Explorer"){
            obj = new ActiveXObject("Microsoft.XMLHTTP");
          }else{
            obj = new XMLHttpRequest();
          }
          return obj;
        }
        var xhttp = create_obj();
        //-----------Thiết lập dữ liệu và gửi request-------
        function getdata(url){
          xhttp.open("GET","/"+url,true);
          xhttp.onreadystatechange = process;//nhận reponse 
          xhttp.send();
        }
        //-----------Kiểm tra response--------------------
        function process(){
          if(xhttp.readyState == 4 && xhttp.status == 200){
            //------Updat data sử dụng javascript----------
            ketqua = xhttp.responseText; 
            document.getElementById("reponsetext").innerHTML=ketqua;       
          }
        }
        //------------Kiểm tra trạng thái chân D1 D2-------------
        function getstatus(){
          xhttp.open("GET","getstatus",true);
          xhttp.onreadystatechange = process_json;
          xhttp.send();
        }
       
        //-----------Kiểm tra response-------------------------------
        function process_json(){
          if(xhttp.readyState == 4 && xhttp.status == 200){
            //------Update data sử dụng javascript-------------------
            var trangthai_JSON = xhttp.responseText;
            var Obj = JSON.parse(trangthai_JSON);
            document.getElementById("data1").innerHTML = Obj.D1;
            document.getElementById("data2").innerHTML = Obj.D2;
            document.getElementById("data3").innerHTML = Obj.D3;
            document.getElementById("data4").innerHTML = Obj.D4;
          }
        }
        //---Ham update duu lieu tu dong---
        setInterval(function() {
          getstatus();
        }, 1000);
      </script>
   </body> 
  </html>
)=====";
//=========================================//

void Read_Touch(){
    if(digitalRead(TOUCH_PIN1) == 1){
      status1 = !status1;
      digitalWrite(RELAY1, status1);
      Serial.println("Touch1 OK ");
      while(digitalRead(TOUCH_PIN1));
      Serial.println("Touch1 Not ");
    }

    if(digitalRead(TOUCH_PIN2) == 1){
      status2 = !status2;
      digitalWrite(RELAY2, status2);
      Serial.println("Touch2 OK ");
      while(digitalRead(TOUCH_PIN2));
      Serial.println("Touch2 Not ");
    }

    if(digitalRead(TOUCH_PIN3) == 1){
      status3 = !status3;
      digitalWrite(RELAY3, status3);
      Serial.println("Touch3 OK ");
      while(digitalRead(TOUCH_PIN3));
      Serial.println("Touch3 Not ");
    }

    if(digitalRead(TOUCH_PIN4) == 1){
      status4 = !status4;
      digitalWrite(RELAY4, status4);
      Serial.println("Touch4 OK ");
      while(digitalRead(TOUCH_PIN4));
      Serial.println("Touch4 Not ");
    }
}

void Pin_init(){
  pinMode(TOUCH_PIN1, INPUT_PULLUP);
  pinMode(TOUCH_PIN2, INPUT_PULLUP);
  pinMode(TOUCH_PIN3, INPUT_PULLUP);
  pinMode(TOUCH_PIN4, INPUT_PULLUP);
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
}

void Wifi_Init(){
  WiFi.begin(ssid,pass);
  Serial.print("\nConnecting ");
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }
  Serial.println("ESP Address is : ");
  Serial.print(WiFi.localIP());
  
  server.on("/",mainpage);
  
  server.on("/onRelay1",on_Relay1);
  server.on("/onRelay2",on_Relay2);
  server.on("/onRelay3",on_Relay3);
  server.on("/onRelay4",on_Relay4);

  server.on("/offRelay1",off_Relay1);
  server.on("/offRelay2",off_Relay2);
  server.on("/offRelay3",off_Relay3);
  server.on("/offRelay4",off_Relay4);

  server.on("/getstatus",get_status);
  server.begin();
}
void setup() {
  Serial.begin(115200);
  Pin_init();
  Wifi_Init();
//  updateOTAInit();
}

void loop() {
  Read_Touch();
  server.handleClient();
}

void mainpage(){
  String s = FPSTR(MainPage);
  server.send(200,"text/html",s);
}
void on_Relay1(){
    digitalWrite(RELAY1,HIGH);
}
void on_Relay2(){
    digitalWrite(RELAY2,HIGH);
}
void on_Relay3(){
    digitalWrite(RELAY3,HIGH);
}
void on_Relay4(){
    digitalWrite(RELAY4,HIGH);
}
void off_Relay1(){
    digitalWrite(RELAY1,LOW);
}
void off_Relay2(){
    digitalWrite(RELAY2,LOW);
}
void off_Relay3(){
    digitalWrite(RELAY3,LOW);
}
void off_Relay4(){
    digitalWrite(RELAY4,LOW);
}

void get_status(){
  String d1,d2,d3,d4;
  if(digitalRead(RELAY1)==0){
    d1 = "OFF";
  }else{
    d1 = "ON";
  }
  if(digitalRead(RELAY2)==0){
    d2 = "OFF";
  }else{
    d2 = "ON";
  }
  if(digitalRead(RELAY3)==0){
    d3 = "OFF";
  }else{
    d3 = "ON";
  }
  if(digitalRead(RELAY4)==0){
    d4 = "OFF";
  }else{
    d4 = "ON";
  }
  String s1 = "{\"D1\": \""+ d1 +"\", \"D2\": \""+ d2 +"\", \"D3\": \""+ d3 +"\", \"D4\": \""+ d4 +"\"}";
  server.send(200,"application/json",s1);
}

//void updateOTAInit()
//{
//  server2.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//    request->send(200, "text/plain", "Hi! I am ESP32.");
//  });
//
//  AsyncElegantOTA.begin(&server2);    // Start ElegantOTA
//  server2.begin();
//  Serial.println("HTTP server started");
//}
