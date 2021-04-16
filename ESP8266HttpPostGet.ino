
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

const char* ssid = "TJ";
const char* password = "12345678";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://dishari-codestudio.herokuapp.com/api/v1/boat/login/";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;


float authReadingsArr[100];

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED)
    {
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      
      //If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"phone\":\"01832409849\",\"pin\":\"4567\"}");

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      //Data to send with HTTP POST
      String httpRequestData = "phone=01832409849&pin=4567";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      

     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println(payload);

      
      //Geting token
      
      
      JSONVar myObject = JSON.parse(payload);
      if (JSON.typeof(myObject) == "undefined") 
      {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
      
      JSONVar value = myObject[keys[0]];
      Serial.println(value);
      Serial.print(" \n \n ");
 
      // Free resources
      http.end();
      delay(30000);
      
    }
    
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

}
