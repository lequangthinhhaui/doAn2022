var client = new Paho.MQTT.Client("broker.hivemq.com", 8000,"web_" + parseInt(Math.random() * 100, 10));  
client.onConnectionLost = onConnectionLost;  
client.onMessageArrived = onMessageArrived;

var options = { useSSL: false,userName: "",password: " ",onSuccess: onConnect , onFailure: doFail } 

client.connect(options);  
    
function onConnect()
{
    console.log("Connect tới Server");
    client.subscribe("maixuankien2018605403/iotBtnInfo");	 		
}  

function doFail(e) {
    console.log("Không kết nối được tới server");
    console.log(e);
}

function onConnectionLost(responseObject) 
{
    if (responseObject.errorCode !== 0) 
    {
    console.log("error");
    console.log("onConnectionLost:" + responseObject.errorMessage);
    }
} 

function onMessageArrived(message) 
{
    console.log("Data ESP:" + message.payloadString);
    var DataServer = message.payloadString;
    //document.getElementById("status-relay-1").innerHTML = DataServer;
    var DataJson = JSON.parse(DataServer);
    console.log(DataJson.den1)


// hiển thị trạng thái relay 1

    if(DataJson.den6 == 1 || DataServer == 61)
    {

        document.getElementById("status-p1").innerHTML = "Trạng Thái đèn: Đang Bật";
    }
    else
        document.getElementById("status-p1").innerHTML = "Trạng Thái đèn: Đang Tắt";


    if(DataJson.den5 == 1 || DataServer == 51)
    {

        document.getElementById("status-p2").innerHTML = "Trạng Thái Quạt: Đang Bật";
    }
    else
        document.getElementById("status-p2").innerHTML = "Trạng Thái Quạt: Đang Tắt";

    if(DataJson.den3 == 1 || DataServer == 31)
    {

        document.getElementById("status-p3").innerHTML = "Trạng Thái đèn: Đang Bật";
    }
    else
        document.getElementById("status-p3").innerHTML = "Trạng Thái đèn: Đang Tắt";


    if(DataJson.den1 == 1 || DataServer == 11)
    {

        document.getElementById("status-p4").innerHTML = "Trạng Thái đèn: Đang Bật";
    }
    else
        document.getElementById("status-p4").innerHTML = "Trạng Thái đèn: Đang Tắt";

    if(DataJson.den4 == 1 || DataServer == 41)
    {

        document.getElementById("status-p5").innerHTML = "Trạng Thái đèn: Đang Bật";
    }
    else
        document.getElementById("status-p5").innerHTML = "Trạng Thái đèn: Đang Tắt";


    if(DataJson.den7 == 1 || DataServer == 71)
    {

        document.getElementById("status-p6").innerHTML = "Trạng Thái Quạt: Đang Bật";
    }
    else
        document.getElementById("status-p6").innerHTML = "Trạng Thái Quạt: Đang Tắt";

    if(DataJson.den2 == 1 || DataServer == 21)
    {

        document.getElementById("status-p7").innerHTML = "Trạng Thái đèn: Đang Bật";
    }
    else
        document.getElementById("status-p7").innerHTML = "Trạng Thái đèn: Đang Tắt";
}


//DOM HTML
// var onButton1 = document.querySelector("#on-button-1");
console.log(onButton1);
var offButton1 = document.getElementById("off-button-1");
var onButton2 = document.getElementById("on-button-2");
var offButton2 = document.getElementById("off-button-2");
var onButton3 = document.getElementById("on-button-3");
var offButton3 = document.getElementById("off-button-3");
var onButton4 = document.getElementById("on-button-4");
var offButton4 = document.getElementById("off-button-4");
var onButton5 = document.getElementById("on-button-5");
var offButton5 = document.getElementById("off-button-5");
var onButton6 = document.getElementById("on-button-6");
var offButton6 = document.getElementById("off-button-6");
var onButton7 = document.getElementById("on-button-7");
var offButton7 = document.getElementById("off-button-7");

//add event listener
// onButton1.addEventListener("click", onClick1);
offButton1.addEventListener("click", offClick1);
onButton2.addEventListener("click", onClick2);
offButton2.addEventListener("click", offClick3);
onButton3.addEventListener("click", onClick4);
offButton3.addEventListener("click", offClick5);
onButton4.addEventListener("click", onClick6);
offButton4.addEventListener("click", offClick7);

function onClick1()
{
	var DataSend = "61";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick1()
{
	var DataSend = "60";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}

function onClick2()
{
	var DataSend = "51";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick2()
{
	var DataSend = "50";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}


function onClick3()
{
	var DataSend = "31";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick3()
{
	var DataSend = "30";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}


function onClick4()
{
	var DataSend = "11";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick4()
{
	var DataSend = "10";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}


function onClick5()
{
	var DataSend = "41";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick5()
{
	var DataSend = "40";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}


function onClick6()
{
	var DataSend = "71";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick6()
{
	var DataSend = "70";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}


function onClick7()
{
	var DataSend = "21";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
	 
}
function offClick7()
{
	var DataSend = "20";
	client.send("maixuankien2018605403/iotRelayControl", DataSend, 2, false);
}

function onClick81()
{
	client.send("maixuankien2018605403/iotRelayControl", "71", 2, false);
    client.send("maixuankien2018605403/iotRelayControl", "51", 2, false);
}

function onClick82()
{
	client.send("maixuankien2018605403/iotRelayControl", "70", 2, false);
    client.send("maixuankien2018605403/iotRelayControl", "50", 2, false);
}


function onClick83()
{
	client.send("maixuankien2018605403/iotRelayControl", "11", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "21", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "31", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "41", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "61", 2, false);
}


function onClick84()
{
	client.send("maixuankien2018605403/iotRelayControl", "10", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "20", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "30", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "40", 2, false);
	client.send("maixuankien2018605403/iotRelayControl", "60", 2, false);
}

