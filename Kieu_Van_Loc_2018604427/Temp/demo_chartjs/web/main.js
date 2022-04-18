var client = new Paho.MQTT.Client("broker.hivemq.com", 8000,"web_" + parseInt(Math.random() * 100, 10));  
client.onConnectionLost = onConnectionLost;  
client.onMessageArrived = onMessageArrived;

var options = { useSSL: false,userName: "hungthinhhaui",password: "CNxCDXpJ",onSuccess: onConnect , onFailure: doFail } 

client.connect(options);  
    
function onConnect()
{
    console.log("Connect tới Server");
    client.subscribe("hungthinhhaui/maylanh");	 // nhận dữ liệu		
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




const labels = [

  ];

  const data = {
    labels: labels,
    datasets: [{
      label: 'Điện áp',
      backgroundColor: 'rgb(255, 255, 255)',
      borderColor: '#73BF69',
      data: [],
    },
    {
        label: 'Dòng điện',
        backgroundColor: 'rgb(255, 255, 255)',
        borderColor: '#F2495C',
        data: [],
      }]
  };

  const config = {
    type: 'line',
    data: data,
    options: {
        maintainAspectRatio: false,
        scales: {
            x: {
              ticks: {
                color: "white"
              }
            },
            y: {
              ticks: {
                color: "white"
              }
            }
          },

          plugins: {
            legend: {
              labels: {
                font: {
                  size: 30
                }
              }
            }
          }
        }
  };


  Chart.defaults.font.size = 30;

  const myChart = new Chart(
    document.getElementById('myChart'),
    config
  );

  
  var i = 0;
  function addData(chart, label, data1, data2) {
    i++;
    chart.data.datasets.forEach((dataset, index) => {
      console.log(index);

        if (i<=30)
        {
            // dataset.data1.push(data);
            if(index == 0)
              dataset.data.push(data1);
            else if(index == 1)
              dataset.data.push(data2);
        }
        else
        {
          if(index == 0)
          {
            dataset.data.shift();
            dataset.data.push(data1);
          }
          else if(index == 1)
          {
            dataset.data.shift();
            dataset.data.push(data2);
          }
        }
    });

    if (i<=30)
        chart.data.labels.push(label);
    else
    {
        chart.data.labels.shift();
        chart.data.labels.push(label);
    }
    chart.update();
}

function onMessageArrived(message) 
{
    console.log("Data ESP:" + message.payloadString);
    var DataServer = message.payloadString;
    var DataJson = JSON.parse(DataServer);

    document.getElementById("dien-ap").innerHTML = DataJson.Vol;
    document.getElementById("dong-dien").innerHTML = DataJson.Amp;

    console.log(DataJson);

    var today = new Date();
    var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
    // console.log(time)
    var rdnumber = (Math.random()*250);
    var rdnumber2 = (Math.random()*10);


    var rdnumber = DataJson.Vol;
    var rdnumber2 = DataJson.Amp;

    addData(myChart, time, rdnumber, rdnumber2);

}

