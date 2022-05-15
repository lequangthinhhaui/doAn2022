#include <WiFi.h>
#include <WebServer.h>

#define SENSOR_MUA 34
#define SENSOR_AS 35
#define POSITION1 32
#define POSITION2 33

//INT1, INT2 của L298 điều khiển chiều quay của Motor1: INT1(+), INT2(-) => QT, INT1(-), INT2(+) => QN
//INT3, INT4 của L298 điều khiển chiều quay của Motor2: INT3(+), INT4(-) => QT, INT3(-), INT4(+) => QN

#define INT1 25
#define INT2 26
#define INT3 27
#define INT4 14

#define BTN_RESET 21 //btn1
#define BTN_MODE 22 //btn2
#define BTN_QT 5  //btn3
#define BTN_QN 18 //btn4

// chiều quay: 1: quay thuận, 0: Quay ngược
int ISCLOCK = 0; 
bool MODE = false;
enum isClock{
  NOT_ROTATE = 0,
  FORWAR = 1,
  REVERSE = 2
};

int adc1, adc2 = 0;
float ASVal, MUAVal = 0.0;
/*
 * Require: Yêu cầu: 2 chế độ: 
 * - Tự động: Kéo rèm vào khi mưa, Kéo rèm ra khi tạnh mưa. 
 * - Menu: Điều khiển bằng tay thông qua webserver (điều khiển DC).
 * 
 */
 
// Wifi
char* ssid = "TP-LINK_D64E64";
char* pass = "88888888";
WebServer server(80);

//=========Biến chứa mã HTLM Website==//
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
      <h1>CONTROL SYSTEM</h1>
      <div><b>Mode</b></div>
      <div>
        <button class="bt_on" onclick="getdata('setAuto')">Auto Mode</button>
        <button class="bt_off" onclick="getdata('setMenu')">Menu Mode</button>
      </div>
      <div><b>Reset Position</b></div>
      <div>
        <button class="bt_on" onclick="getdata('onReset')">RESET</button>
      </div>
      <div><b>Rotation Direction</b></div>
      <div>
        <button class="bt_off" onclick="getdata('onDC')">Quay Thuận</button>
        <button class="bt_off" onclick="getdata('offDC')">Quay Ngược</button>
      </div>
      <div>Current Status: <pan id="reponsetext"><pan></div>
      <h1>DATA</h1>
      <div>Current Mode: <b><pan id="data3"><pan></b></div> 
      <div>Status Motor: <b><pan id="data1"><pan></b></div>
      <div>Status Position: <b><pan id="data2"><pan></b></div>
      <div>Brightness: <b><pan id="data5"><pan></b></div>
      <div>Detect Rain: <b><pan id="data4"><pan></b></div>
    
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
        //------------Kiểm tra trạng thái chân D1 D2------
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
            document.getElementById("data5").innerHTML = Obj.D5;
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

void Wifi_Init(){
  WiFi.begin(ssid, pass);
  Serial.begin(9600);
  Serial.print("\nConnecting ");
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }
  Serial.println("ESP Address is : ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void Web_Init(){
  server.on("/",mainpage);
  server.on("/onDC",DC_QT);
  server.on("/offDC",DC_QN);
  server.on("/setAuto",Auto_Func);
  server.on("/setMenu",Menu_Func);
  server.on("/onReset",Reset_Func);
  server.on("/getstatus",get_status);
  server.begin();
}

void interrupt_Fuc(){
  if(MODE == false)
    MODE = true;
  else
    MODE = false;
}
void Pin_init(){
  pinMode(POSITION1, INPUT);
  pinMode(POSITION2, INPUT);
  
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_QT, INPUT_PULLUP);
  pinMode(BTN_QN, INPUT_PULLUP);

  pinMode(BTN_MODE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_MODE), interrupt_Fuc, FALLING);
  
  pinMode(INT1, OUTPUT);
  pinMode(INT2, OUTPUT);
  pinMode(INT3, OUTPUT);
  pinMode(INT4, OUTPUT);
}

void setup() {
  Pin_init();
  Wifi_Init();
  Web_Init();
}

void Read_Sensor(){
  adc1 = analogRead(SENSOR_MUA);
  Serial.print("Mua Val: ");
  Serial.println(adc1);
  adc2 = analogRead(SENSOR_AS);

  MUAVal = ((float)adc1 /4096)*100;
  ASVal = ((float)adc2 /4096)*100;

}

void Set_Control_Motor(int dk){
  if(dk == FORWAR){
    digitalWrite(INT1, HIGH);
    digitalWrite(INT2, LOW);
    digitalWrite(INT3, HIGH);
    digitalWrite(INT4, LOW);
    Serial.println("Motor Is Rotated: FORWAR!");
  }else if(dk == REVERSE){
    digitalWrite(INT1, LOW);
    digitalWrite(INT2, HIGH);
    digitalWrite(INT3, LOW);
    digitalWrite(INT4, HIGH);
    Serial.println("Motor Is Rotated: REVERSE!");
  }else if(dk == NOT_ROTATE){
    digitalWrite(INT1, LOW);
    digitalWrite(INT2, LOW);
    digitalWrite(INT3, LOW);
    digitalWrite(INT4, LOW);
    Serial.println("Motor Is Stopped!");
  }
}

void loop() {
  server.handleClient();
  Read_Sensor();

  if(MODE == false){ // Auto Mode
     if(adc1 <= 1000){
        if(digitalRead(POSITION2) == 1){
          ISCLOCK = FORWAR;
          Set_Control_Motor(ISCLOCK);
          while(!digitalRead(POSITION1));
          ISCLOCK = NOT_ROTATE;
          Set_Control_Motor(ISCLOCK);
        }
     }
     if(ASVal >= 80 && adc1 > 1000){
        if(digitalRead(POSITION1) == 1){
          ISCLOCK = REVERSE;
          Set_Control_Motor(ISCLOCK);
          while(!digitalRead(POSITION2));
          ISCLOCK = NOT_ROTATE;
          Set_Control_Motor(ISCLOCK);
        }
     }
   }else if(MODE == true){ //Menu Mode
      if(digitalRead(BTN_QT) == 0){
          ISCLOCK = FORWAR;
          Set_Control_Motor(ISCLOCK);
          while(!digitalRead(POSITION1));
          ISCLOCK = NOT_ROTATE;
          Set_Control_Motor(ISCLOCK);
      }

      if(digitalRead(BTN_QN) == 0){
          ISCLOCK = REVERSE;
          Set_Control_Motor(ISCLOCK);
          while(!digitalRead(POSITION2));
          ISCLOCK = NOT_ROTATE;
          Set_Control_Motor(ISCLOCK);
      }

     if(digitalRead(BTN_RESET) == 0){ // Reset động cơ về vị trí cảm biến 2
       ISCLOCK = REVERSE;// Quay ngược
       Set_Control_Motor(ISCLOCK);
       while(!digitalRead(POSITION2));
       ISCLOCK = NOT_ROTATE;
       Set_Control_Motor(ISCLOCK);
    }
    
   }
}

void mainpage(){
  String s = FPSTR(MainPage);
  server.send(200,"text/html",s);
}

void DC_QT(){
  if(MODE == true){
    ISCLOCK = FORWAR;
    Set_Control_Motor(ISCLOCK);
    server.send(200,"text/html","Motor Quay Thuận");
    while(!digitalRead(POSITION2));
    ISCLOCK = NOT_ROTATE;
    Set_Control_Motor(ISCLOCK);
  }
}
void Auto_Func(){
  MODE = false;
  server.send(200,"text/html","Set Auto Mode");
}
void Menu_Func(){
  MODE = true;
  server.send(200,"text/html","Set Menu Mode");
}
void DC_QN(){
  if(MODE == true){
    ISCLOCK = REVERSE;
    Set_Control_Motor(ISCLOCK);
    server.send(200,"text/html","Motor Quay Ngược");
    while(!digitalRead(POSITION1));
    ISCLOCK = NOT_ROTATE;
    Set_Control_Motor(ISCLOCK);
  }
}

void Reset_Func(){
  if(MODE == true){
    ISCLOCK = REVERSE;
    Set_Control_Motor(ISCLOCK);
    server.send(200,"text/html","Reset Position");
    while(!digitalRead(POSITION1));
    ISCLOCK = NOT_ROTATE;
    Set_Control_Motor(ISCLOCK);
  }
}

void get_status(){
    String d1,d2,d3,d4,d5;

    if(ISCLOCK == NOT_ROTATE){
      d1 = "Not Rotate";
    }else if(ISCLOCK == FORWAR){
      d1 = "Forwar";
    }else if(ISCLOCK == REVERSE){
      d1 = "Reverse";
    }
    
    if(digitalRead(POSITION1)==1){
      d2 = "Position 1";
    }else if(digitalRead(POSITION2)==1){
      d2 = "Position 2";
    }else if(digitalRead(POSITION1)==0 && digitalRead(POSITION2)==0){
      d2 = "Moving";
    }

    if(MODE == false){
       d3 = "Auto Mode";
    }else if(MODE == true){
       d3 = "Menu Mode";
    }
    if(adc1 <= 1000){ //Detect Rain
      d4 = "Rain";
    }else{
      d4 = "No Rain";
    }
    d5 = String(ASVal);
    
    String s1 = "{\"D1\": \""+ d1 +"\", \"D2\": \""+ d2 +"\", \"D3\": \""+ d3 +"\", \"D4\": \""+ d4 +"\", \"D5\": \""+ d5 +"\"}";
    server.send(200,"application/json",s1);
} 
