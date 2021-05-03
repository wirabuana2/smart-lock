#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "LAB KOMPUTER";
const char* password = "labserver";

const int pinCH_1 = 13;
const int pinCH_2 = 12;
const int pinCH_3 = 14;
const int pinCH_4 = 27;

void setup () {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
   pinMode(pinCH_1, OUTPUT);
   pinMode(pinCH_2, OUTPUT);
   pinMode(pinCH_3, OUTPUT);
   pinMode(pinCH_4, OUTPUT);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }

  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected!!!");
  }
  else{ 
    Serial.println("Connected Failed!!!");
  }

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin("http://iot.untukdunia.com/process.php"); 
    int httpCode = http.GET();

    if (httpCode > 0) {
      char json[500];
      String payload = http.getString();
      payload.toCharArray(json, 500);
      
      //StaticJsonDocument<200> doc;
      DynamicJsonDocument doc(JSON_OBJECT_SIZE(5));

     // Deserialize the JSON document
       deserializeJson(doc, json);
       
     int id   = doc["id"];
     int CH_1 = doc["CH_1"];
     int CH_2 = doc["CH_2"];
     int CH_3 = doc["CH_3"];
     int CH_4 = doc["CH_4"];

     
     Serial.print("id= ");
     Serial.println(id);
     Serial.print("Channel 1= ");
     Serial.println(CH_1);
     Serial.print("Channel 2= ");
     Serial.println(CH_2);
     Serial.print("Channel 3= ");
     Serial.println(CH_3);
     Serial.print("Channel 4= ");
     Serial.println(CH_4);
     Serial.println(" ");

     if(CH_1 == 0){
      digitalWrite(pinCH_1, LOW);
     }
     else{  
      digitalWrite(pinCH_1, HIGH);
     }

      if(CH_2 == 0){
      digitalWrite(pinCH_2, LOW);
     }
     else{  
      digitalWrite(pinCH_2, HIGH);
     }

      if(CH_3 == 0){
      digitalWrite(pinCH_3, LOW);
     }
     else{  
      digitalWrite(pinCH_3, HIGH);
     }

      if(CH_4 == 0){
      digitalWrite(pinCH_4, LOW);
     }
     else{  
      digitalWrite(pinCH_4, HIGH);
     }
      
      delay(1000);
    }

    http.end();

  }

}
