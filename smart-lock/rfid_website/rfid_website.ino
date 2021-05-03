#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "MFRC522.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <LiquidCrystal_I2C.h>

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;

#include "CTBot.h"
CTBot myBot;
String token = "1226409795:AAHRMT-QlFn6DOP6Gfw13W8p8SvBYc6DNGc";
const int id = 936592892;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid     = "Bening";
const char* password = "12345678";
String url = "http://wirabuana.sch.id/doorlock/public/user/";
bool cek = true;

#define RST_PIN D4
#define SS_PIN  D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

String logToServer(unsigned long cardUID) {
  lcd.setCursor(0,0);
  lcd.print("Autentikasi...  ");
  delay(700);

if(WiFi.status() != WL_CONNECTED){
    int masterKey = 1766455218; 
  if(masterKey == cardUID) {
        Serial.println("Silahkan masuk!");
        lcd.setCursor(0,0);
        lcd.print("Akses diterima  ");
        digitalWrite(D0, LOW);
        delay(5000);
        digitalWrite(D0, HIGH);
    }else{
        lcd.setCursor(0,0);
        lcd.print("  SEDANG DALAM  ");
        lcd.setCursor(0,1);
        lcd.print("  MODE OFFLINE  ");
        delay(5000);
    }
  }
  else{
  HTTPClient http;

  http.begin(url + cardUID); //HTTP
  int httpCode = http.GET();
  if (true) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    
    int masterKey = 1766455218; 
    if(httpCode == HTTP_CODE_OK || masterKey == cardUID) {
      String payload = http.getString();

        Serial.println("Silahkan masuk!");
        lcd.setCursor(0,0);
        lcd.print("Akses diterima  ");
        digitalWrite(D0, LOW);
        delay(5000);
        digitalWrite(D0, HIGH);
      Serial.println(payload);
    }else if(httpCode == HTTP_CODE_BAD_REQUEST){
     lcd.setCursor(0,0);
     lcd.print(" Akses ditolak  ");
     delay(3000);
     lcd.setCursor(0,0);
     lcd.print("   anda tidak   ");
     lcd.setCursor(0,1);
     lcd.print("    terdaftar   ");
     delay(3000);
     Serial.println("Anda tidak terdaftar disini!");
  }else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      lcd.setCursor(0,0);
      lcd.print("Koneksi lambat  ");
      delay(1500);
  }
  Serial.println("Selesai");
  }
      lcd.setCursor(0,0);
      lcd.print(" Ready to scan ");
      delay(1500);
  }
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D8, OUTPUT);
  
  digitalWrite(D8, HIGH);
  
  Serial.begin(9600);
  lcd.begin();   
  lcd.backlight();  
  lcd.setCursor(3,0);
  
  Serial.println("");
  delay(100);

 // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

   // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
      lcd.setCursor(0,0);
    lcd.print("Menunggu Koneksi");
    digitalWrite(D0, HIGH);
  
  // Uncomment and run it once, if you want to erase all the stored information
//    wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("Smartlock");

  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
    lcd.setCursor(0,0);
    lcd.print("   Connected    ");
    delay(1500);
  
  server.begin();

  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  //If you set Antenna Gain to Max it will increase reading distance
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

unsigned long getCardUID() {
  lcd.setCursor(0,3);
  lcd.print("loading");
  if ( !mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

int wait = 51;


void loop() {
   WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart-lock Web server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }


  if (wait > 50) {
    Serial.println("");
    lcd.setCursor(0,0);
    lcd.print("Scan kartu anda ");
    wait = 0;
  }

  Serial.print("");
  lcd.setCursor(0,1);
  lcd.print(".......................");

  wait++;

  if (wait % 2 == 0)
    digitalWrite(D1, HIGH);
  else
    digitalWrite(D1, LOW);

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(30);
    return;
  }

  unsigned long cardUID = getCardUID();

  for (int i = 0; i < 3; i++) {
    digitalWrite(D1, HIGH);
    delay(30);
    digitalWrite(D1, LOW);
    delay(30);
  }

  if (cardUID == -1) {
    Serial.println("Failed to get UID");
    lcd.setCursor(0,0);
    lcd.print("Scan ulang kartu!");
    delay(2000);
    return;
  }

  Serial.printf("\nCard UID is %u\n", cardUID);

  for (int i = 0; i < 3; i++) {
    digitalWrite(D1, HIGH);
    delay(30);
    digitalWrite(D1, LOW);
    delay(30);
  }

  logToServer(cardUID);

  wait = 51;
}
