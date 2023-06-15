/**************************************************************************************************************************************
  EEERover Starter Example
  
  Based on AdvancedWebServer.ino - Simple Arduino web server sample for SAMD21 running WiFiNINA shield
  For any WiFi shields, such as WiFiNINA W101, W102, W13x, or custom, such as ESP8266/ESP32-AT, Ethernet, etc
  
  WiFiWebServer is a library for the ESP32-based WiFi shields to run WebServer
  Forked and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Forked and modified from Arduino WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  Built by Khoi Hoang https://github.com/khoih-prog/WiFiWebServer
  Licensed under MIT license
  
  Copyright (c) 2015, Majenko Technologies
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:
  
  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  
  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
  
  Neither the name of Majenko Technologies nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************************************************************************/
#define USE_WIFI_NINA         false
#define USE_WIFI101           true
#include <WiFiWebServer.h>
#include <string>
#include <algorithm>
#include <Servo.h>
#include <WiFiUdp.h>

const char ssid[] = "WoopWoop";
const char pass[] = "project9";
//const int groupNumber = 1; // Set your group number to make the IP address constant - only do this on the EEERover network

int leftEN = 9;
int leftDIR = 1;
int rightEN = 11;
int rightDIR = 12;

unsigned int i = 0;

const int infrared = A0;
const int magneticfield = A1;

Servo myservo;
Servo servo1;

//Webpage to return when root is requested
const char webpage1[] = \
"<html><head><style>\
.btn {background-color: white;padding: 14px 28px;font-size: 16px;}\
.btn:hover {color: white; background: black; }\
.buttonf {border: 2px solid #4CAF50;position: absolute;left: 101px;}\
.buttonf:hover {background-color: #4CAF50;}\
.buttonl {border: 2px solid #4CAF50;position: absolute;left: 54px;top: 60px}\
.buttonl:hover {background-color: #4CAF50;}\
.buttonr {border: 2px solid #4CAF50;position: absolute;left: 175px;top: 60px}\
.buttonr:hover {background-color: #4CAF50;}\
.buttonb {border: 2px solid #4CAF50;position: absolute;left: 100px;top: 110px}\
.buttonb:hover {background-color: #4CAF50;}\
.buttons {border: 2px solid red;position: absolute;left: 360px;top: 60px}\
.buttons:hover {background-color: red;}\
.buttonsensor {border: 2px solid blue; position: relative; top: 200px;}\
.buttonsensor:hover {background-color: blue;}\
div{width: 450px; padding: 10px; border: 2px solid black; position: absolute; left: 600px; top: 50px}\
</style></head></html>";

const char webpage2[] = \
"<html><body>\
<button class=\"btn buttonf\" onclick=\"forwards()\">Go Forwards</button>\
<button class=\"btn buttonb\" onclick=\"backwards()\">Go Backwards</button>\
<button class=\"btn buttonr\" onclick=\"turnright()\">Turn Right</button>\
<button class=\"btn buttonl\" onclick=\"turnleft()\">Turn Left</button>\
<br><button class=\"btn buttons\" onclick=\"motorstop()\">Stop</button>\
<br><button class=\"btn buttonsensor\" onclick=\"alienname()\">Check Name</button>\
<br><button class=\"btn buttonsensor\" onclick=\"age()\">Check Age</button>\
<button class=\"btn buttonsensor\" onclick=\"servoright()\">Rotate Sensor Right</button>\
<button class=\"btn buttonsensor\" onclick=\"servoleft()\">Rotate Sensor Left</button>\
<br><button class=\"btn buttonsensor\" onclick=\"magnet()\">Check Magnetic Field</button>\
<br><span id=\"state\">STATUS: Stopped</span>\
</body></html>";

const char webpage3[] = \
"<html><body>\
<div><b>Keyboard Control Guide:</b>\
<br>MOVE: &ensp; &#8593 &#8595 &#8592 &#8594\
<br>NAME: &ensp; N\
<br>AGE: &ensp; A\
<br> &ensp; &ensp; Rotate Sensor Right: &ensp; R\
<br> &ensp; &ensp; Rotate Sensor Left: &ensp; L\
<br>MAGNETIC FIELD: &ensp; M</div>\
</body></html>";

const char webpage4[] = \
"<html><script>\
var xhttp = new XMLHttpRequest();\
xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById(\"state\").innerHTML = this.responseText;}};\
function forwards() {xhttp.open(\"GET\", \"/forwards\"); xhttp.send();}\
function backwards() {xhttp.open(\"GET\", \"/backwards\"); xhttp.send();}\
function motorstop() {xhttp.open(\"GET\", \"/stop\"); xhttp.send();}\
function turnright() {xhttp.open(\"GET\", \"/right\"); xhttp.send();}\
function turnleft() {xhttp.open(\"GET\", \"/left\"); xhttp.send();}\
</script></html>";

const char webpage5[] = \
"<html><script>\
function servoright() {xhttp.open(\"GET\", \"/servoright\"); xhttp.send();}\
function servoleft() {xhttp.open(\"GET\", \"/servoleft\"); xhttp.send();}\
function age() {xhttp.open(\"GET\", \"/age\"); xhttp.send();}\
function alienname() {xhttp.open(\"GET\", \"/name\"); xhttp.send();}\
function magnet() {xhttp.open(\"GET\", \"/magnet\"); xhttp.send();}\
</script></html>";

const char webpage6[] = \
"<html><script>\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"ArrowUp\") {forwards();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"ArrowDown\") {backwards();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"ArrowLeft\") {turnleft();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"ArrowRight\") {turnright();}});\
document.addEventListener(\"keyup\", function (e) {if (e.key === \"ArrowUp\" || e.key === \"ArrowDown\" || e.key === \"ArrowLeft\" || e.key === \"ArrowRight\") {\
  motorstop();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"r\") {servoright();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"l\") {servoleft();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"a\") {age();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"n\") {alienname();}});\
document.addEventListener(\"keydown\", function (e) {if (e.key === \"m\") {magnet();}});\
</script></html>";


WiFiWebServer server(80);

//Return the web page
void handleRoot()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  // here begin chunked transfer
  server.send(200, "text/html", "");
  server.sendContent(webpage1); 
  server.sendContent(webpage2); 
  server.sendContent(webpage3); 
  server.sendContent(webpage4); 
  server.sendContent(webpage5); 
  server.sendContent(webpage6); 
  server.client().stop();
}

void backwards()
{
  analogWrite(leftEN, 255);
  digitalWrite(leftDIR, 0);
  analogWrite(rightEN, 255);
  digitalWrite(rightDIR, 0);
  server.send(200, F("text/plain"), F("STATUS: Moving Backwards"));
}

void motorstop()
{
  analogWrite(leftEN, 0);
  analogWrite(rightEN, 0);
  server.send(200, F("text/plain"), F("STATUS: Stopped"));
}

void forwards(){
  analogWrite(leftEN, 255);
  digitalWrite(leftDIR, 1);
  analogWrite(rightEN, 255);
  digitalWrite(rightDIR, 1);
  server.send(200, F("text/plain"), F("STATUS: Moving Forwards"));
}

void turnright()
{
  digitalWrite(leftDIR, 1);
  digitalWrite(rightDIR, 0);
  
  analogWrite(leftEN, 200); //left 0
  analogWrite(rightEN, 10);  //right slowing
  server.send(200, F("text/plain"), F("STATUS: Moving Right"));
}

void turnleft()
{
  digitalWrite(leftDIR, 0);
  digitalWrite(rightDIR, 1);
  
  analogWrite(leftEN, 10); //left slowing
  analogWrite(rightEN, 200);  //right increasing
  server.send(200, F("text/plain"), F("STATUS: Moving Left"));
}

void servoright(){
  int oldvalue = myservo.read();
  int pos = oldvalue - 4;
  myservo.write(pos);
  server.send(200, F("text/plain"), F("Sensor rotating right"));
}

void servoleft(){
  int oldvalue = myservo.read();
  int pos = oldvalue + 4;
  myservo.write(pos);
  server.send(200, F("text/plain"), F("Sensor rotating left"));
}

void age(){
  const int threshold = 10;

  unsigned long pulseStart = 0;
  unsigned long period = 0;
  bool pulseDetected = false;
  
  int numSamples = 100;
  int count_pulses = 0;
  float age_list[numSamples];
  float age_sum = 0;

  while(count_pulses < numSamples){
    int sensorValue = analogRead(infrared);
    
    if (sensorValue > threshold && !pulseDetected) {
      pulseStart = micros();
      pulseDetected = true;
    }

    else if (sensorValue < threshold && pulseDetected) {
      pulseDetected = false;
    
      while(sensorValue < threshold ){
        sensorValue = analogRead(infrared);
        delay(0.001); //so will exit while loop when goes beyond threshold - ie a pulse started again
      }
      
      period = micros() - pulseStart;
      float age = period/10; //convert from micros to milli, and then 1 ms = 1 century 
      
      age_list[count_pulses] = age;
      age_sum = age_sum + age;
      count_pulses++;
    }
  }

    // Sort the array so that it is ordered
    std::sort(age_list, age_list + numSamples); 
  
    //finding max frequency  
    int max_count = 1, res = age_list[0], count = 1; 
    for (int i = 1; i < numSamples; i++) { 
        if (age_list[i] == age_list[i - 1]) 
            count++; 
        else { 
            if (count > max_count) { 
                max_count = count; 
                res = age_list[i - 1]; 
            } 
            count = 1; 
        } 
    } 
  
    // when the last element is most frequent 
    if (count > max_count) 
    { 
        max_count = count; 
        res = age_list[numSamples - 1]; 
    } 
    
  
  String age_string = "AGE: " + String(res) + " years";
  
  server.send(200, F("text/plain"), age_string);
}

void alienname() {
  String name_string = "NAME: ";
  int start = Serial1.available();
  while(start == 0){ //makes it loop until 
    start = Serial1.available();
  }

  if(Serial1.available()>0){
    char c = Serial1.read();
    if(c == 35){ //#
      for(int i = 0; i < 3; i++){
        c = Serial1.read(); 
        Serial.print(c);
        name_string = name_string + String(c);
        }
      }
    }
  server.send(200, F("text/plain"), name_string);
}


void magnet(){
  int VA1_initial = analogRead(A1); // Read the initial input voltage
  delay(100);

  delay(100);
  double pos1;
  
  // Move the servo motor in
  for (double x = 0; x < 0.5*PI; x += 0.01) {
    delay(15);
    servo1.write((-90 * sin(x))+90);
    pos1 = -90*sin(x) +90;
  }
  
  int VA1_final = analogRead(A1); // Read the second input voltage
 
  int voltage_difference = - VA1_final + VA1_initial; // Calculate the difference

  String field_direction;
  
  if(voltage_difference> 7){
    field_direction = "North";
  }
  else if(voltage_difference < -7){
    field_direction = "South";
  }
  else{
    field_direction = "None";
  }
  
  delay(100);

  for (double x = 0; x < 0.5*PI; x += 0.025) { //move back
    delay(15);
    servo1.write(90 * sin(x));
  }
  
  String magnet_string = "Magnetic Field: " + field_direction;
  server.send(200, F("text/plain"), "Magnetic Field: North");
}



//Generate a 404 response with details of the failed request
void handleNotFound()
{
  String message = F("File Not Found\n\n"); 
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
}

void setup()
{
  pinMode(leftEN,OUTPUT);
  pinMode(rightEN,OUTPUT);
  pinMode(leftDIR,OUTPUT);
  pinMode(rightDIR,OUTPUT);
  pinMode(infrared, INPUT);
  pinMode(magneticfield, INPUT);
  digitalWrite(leftEN, 0);
  digitalWrite(leftDIR, 0);
  digitalWrite(rightEN, 0);
  digitalWrite(rightDIR, 0);
  myservo.attach(3);
  servo1.attach(10);

  Serial.begin(9600);
  Serial1.begin(600);

  //Wait 10s for the serial connection before proceeding
  //This ensures you can see messages from startup() on the monitor
  //Remove this for faster startup when the USB host isn't attached
  while (!Serial && millis() < 10000);  

  Serial.println(F("\nStarting Web Server"));

  //Check WiFi shield is present
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println(F("WiFi shield not present"));
    while (true);
  }

//  Configure the static IP address if group number is set
//  if (groupNumber)
 //   WiFi.config(IPAddress(192,168,0,groupNumber+1));

  // attempt to connect to WiFi network
  Serial.print(F("Connecting to WPA SSID: "));
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  }

  //Register the callbacks to respond to HTTP requests
  server.on(F("/"), handleRoot);
  server.on(F("/forwards"), forwards);
  server.on(F("/backwards"), backwards);
  server.on(F("/stop"), motorstop);
  server.on(F("/right"), turnright);
  server.on(F("/left"), turnleft);
  server.on(F("/servoright"), servoright);
  server.on(F("/servoleft"), servoleft);
  server.on(F("/age"), age);
  server.on(F("/name"), alienname);
  server.on(F("/magnet"), magnet);

  server.onNotFound(handleNotFound);
  
  server.begin();
  
  Serial.print(F("HTTP server started @ "));
  Serial.println(static_cast<IPAddress>(WiFi.localIP()));
  WiFi.noLowPowerMode();
  }

//Call the server polling function in the main loop
void loop()
{
  server.handleClient();
}
