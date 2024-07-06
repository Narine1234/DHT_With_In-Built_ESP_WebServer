#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
//SSID and Password of your WiFi router
const char* ssid = "Your Wifi";
const char* password = "Your Password";
#define LED 2       
#define DHTTYPE DHT11 
uint8_t DHTPin = 4; 
DHT dht(DHTPin, DHTTYPE); 
float humidity, temperature;
ESP8266WebServer server(80); 
const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
  <title>Data Logger</title>
  <h1 style="text-align:center; color:red;">Humidity And Temperature</h1>
  <h3 style="text-align:center;">ESP8266 Data Logger</h3>
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
  /* Data Table Styling*/ 
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
    text-align: center;
  }
  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }
  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
  #dataTable tr:hover {background-color: #ddd;}
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: center;
    background-color: #050505;
    color: white;
  }
  </style>
</head>
<body>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>Temperaure (&deg;C)</th><th>Humidity (%)</th></tr>
  </table>
</div>
<br>
<br>  
<script>
var Tvalues = [];
var Hvalues = [];
var timeStamp = [];
setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
}, 5000); //5000mSeconds update rate
 function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var txt = this.responseText;
  var obj = JSON.parse(txt); 
      Tvalues.push(obj.Temperature);
      Hvalues.push(obj.Humidity);
      timeStamp.push(time);
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1); //Add after headings
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    cell1.innerHTML = time;
    cell2.innerHTML = obj.Temperature;
    cell3.innerHTML = obj.Humidity;
    }
  };
  xhttp.open("GET", "readData", true); //Handle readData server on ESP8266
  xhttp.send();
}
</script>
</body>
</html>

)=====";
 

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void readData() {

 String data = "{\"Temperature\":\""+ String(temperature) +"\", \"Humidity\":\""+ String(humidity) +"\"}";
 digitalWrite(LED,!digitalRead(LED)); //Toggle LED on data request ajax
 server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
 delay(2000);
 temperature = dht.readTemperature(); 
 humidity = dht.readHumidity(); 
 Serial.print(humidity, 1);
 Serial.print(temperature, 1);
}

void setup ()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(DHTPin, INPUT);
  dht.begin();
 
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readData", readData); //This page is called by java Script AJAX
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();          //Handle client requests
}