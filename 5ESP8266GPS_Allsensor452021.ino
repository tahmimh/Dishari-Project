#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <DHT.h>

TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(3,1); // The serial connection to the GPS device3rx1txesp

#define DHTPIN 5 //D1 Pin as GPIO5
#define DHTTYPE DHT22
DHT dht (DHTPIN, DHTTYPE);
int smokeA0  = A0; //for_MQ6
int sensorThres = 800; //threshold_forSmoke
const int sensorPin = 4; //for waterlevel13
int sensorState; //for waterlevel1
const int sensorPin2 = 0; //for waterlevel2
int sensorState2; //for waterlevel2
const int sensorPin3 = 2; //for waterlevel3
int sensorState3;//for waterlevel3
const int trigPin = 14; //for sonar
const int echoPin = 12; //for sonar

long duration; //for sonar
int distance; //for fonar



const char* ssid = "TJ";
const char* password = "12345678";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://dishari-codestudio.herokuapp.com/api/v1/boat/login/";
const char* serverName2 = "http://dishari-codestudio.herokuapp.com/api/v1/boat/location/update/";
const char* serverName3 = "http://dishari-codestudio.herokuapp.com/api/v1/boat/otherinfo";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
//unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
//unsigned long timerDelay = 5000;

String authReadingsArr;
float latitude, longitude;
String lat_str, lng_str, hum_str, temp_str, smoke_str;
unsigned int i =0;
int j=0;
String latarray;
String lngarray;
String humarray;
String temparray;
String smokearray;
String water_str1;
String water_str2;
String water_str3;
String fueldistance_str;


void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  dht.begin ();
  pinMode(smokeA0, INPUT);
  //water level setup
  pinMode(sensorPin, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  //sonar sensor setup
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
}

void loop() {
      
      HTTPClient http;
      
      if (j == 0)
      {
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
      //if (JSON.typeof(myObject) == "undefined") 
      //{
      //  Serial.println("Parsing input failed!");
      //  return;
      //}
      Serial.print("JSON object = ");
      Serial.println(myObject);
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
      JSONVar value2 = myObject[keys[0]];
      String token = JSON.stringify(value2);
      Serial.println(token);
      authReadingsArr = String(token);
      authReadingsArr.remove(33,1);
      authReadingsArr.remove(0,1);
      Serial.println(authReadingsArr);

      // Free resources
      http.end();
      j = 1;

      }

      //Code of GPS
       while (ss.available())
       {
          gps.encode(ss.read());
       }
       if (gps.location.isUpdated())
              {
                //Serial.print("Getting Data");
                latitude = gps.location.lat();
                lat_str = String(latitude , 6);
                longitude = gps.location.lng();
                lng_str = String(longitude , 6);
                
            
                Serial.println("Latitude:");
                Serial.println(lat_str);
                Serial.println("Longitude:");
                Serial.println(lng_str);      
               }

      
     //Long and Lat Sending
     http.begin(serverName2);
     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     //Data to send with HTTP POST
     //String httpRequestData2 = "token="+authReadingsArr+"&lat="+latarray[i]+"&lng="+lngarray[i]; 
     String httpRequestData2 = "token="+authReadingsArr+"&lat=23.755163&lng=90.410965";         
     // Send HTTP POST request
     int httpResponseCode2 = http.POST(httpRequestData2);
     Serial.print("\n \n");
     Serial.print("HTTP Response code: ");
     Serial.println(httpResponseCode2);
      
     String payload2 = http.getString();
     Serial.println(payload2);
      
     http.end(); 


    //Code of Humidity and temperature and smoke

     //temp n humid
     float h = dht.readHumidity(); 
     float t = dht.readTemperature(); 
     int analogSensor = analogRead(smokeA0);
        
     /*if (isnan (h) || isnan (t)) 
     {
       Serial.println("Failed to read from DHT sensor!");
       return;
     }*/
     
     Serial.print("Humidity: "); 
     Serial.print(h);
     hum_str = String(h , 2);
     Serial.print("% ; Temperature: "); 
     Serial.print(t); Serial.println ("°C");
     temp_str = String(t , 2);
     

     Serial.println("String Humidity: ");
     Serial.println(hum_str);
     Serial.println("String Temperature: ");
     Serial.println(temp_str);  

     
     //smoke
     analogSensor = analogRead(smokeA0);
     Serial.print("Value of Smoke: ");
     Serial.println(analogSensor);
     if (analogSensor > sensorThres)
     {
        Serial.print(" Smoke/Fire Detected \n");
     }
     smoke_str = String(analogSensor);
     Serial.println("String Smoke: ");
     Serial.println(smoke_str); 

     //waterlevel1 code
     
     sensorState = digitalRead(sensorPin);
     water_str1 = String(sensorState);
     Serial.println(water_str1);

     //waterlevel2 code
     
     sensorState2 = digitalRead(sensorPin2);
     water_str2 = String(sensorState2);
     Serial.println(water_str2);

     //waterlevel3 code
     
     sensorState3 = digitalRead(sensorPin3);
     water_str3 = String(sensorState3);
     Serial.println(water_str2);

     
     
     //Fuel Measurement Code
     
     // Clears the trigPin
     digitalWrite(trigPin, LOW);
     delayMicroseconds(2);
      
     // Sets the trigPin on HIGH state for 10 micro seconds
     digitalWrite(trigPin, HIGH);
     delayMicroseconds(10);
     digitalWrite(trigPin, LOW);
      
     // Reads the echoPin, returns the sound wave travel time in microseconds
     duration = pulseIn(echoPin, HIGH);
      
     // Calculating the distance
     distance = duration*0.034/2;
     // Prints the distance on the Serial Monitor
     fueldistance_str = String(distance);
     Serial.print("Distance: ");
     Serial.println(fueldistance_str);
     
     
     
     
     
     //Sending ALL sensors expect GPS data
     
     http.begin(serverName3);
     http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     //Data to send with HTTP POST
     String httpRequestData3 = "token="+authReadingsArr+"&ovr1="+water_str1+"&ovr2="+water_str2+"&ovr3="+water_str3+"&fuelm="+fueldistance_str+"&temp="+temp_str+"&humidity="+hum_str+"&smoke="+smoke_str;  
     //String httpRequestData3 = "token="+authReadingsArr+"&ovr1=0&ovr2=0&ovr3=0&fuelm=1.090455&temp=35.09&humidity=45.08&smoke=45.09";
     //String httpRequestData3 = "token="+authReadingsArr+"&ovr1=1&ovr2=0&ovr3=1&fuelm=1.090455&temp=35.09&humidity=45.08&smoke=45.09";
     // Send HTTP POST request
     int httpResponseCode3 = http.POST(httpRequestData3);
     Serial.print("\n");
     Serial.print("HTTP Response code: ");
     Serial.println(httpResponseCode3);
      
     String payload3 = http.getString();
     Serial.println(payload3);
      
     http.end();
      
   
   delay(5000);    

}
