#include <SPI.h>
#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define SS_PIN 21  
#define RST_PIN 22 

const char* ssid     = "Bening";
const char* password = "12345678";
String url = "http://wirabuana.sch.id/doorlock/public/user";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

boolean reconnect() {
  WiFi.begin(ssid, password);

   int retry = 51;
  while (WiFi.status() != WL_CONNECTED) {
    if (retry > 50) {
      Serial.println("");
      Serial.printf("Trying connect to %s", ssid);
      retry = 0;
    }
    delay(100);
    Serial.print(".");
    retry++;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println("");
  Serial.println(WiFi.localIP());
  Serial.println("Tempelkan kartu");
}

String logToServer(unsigned long cardUID) {
  HTTPClient http;
  
  http.begin(url); //HTTP
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  String httpRequestData = "uid="+ String(cardUID); 
  Serial.println(httpRequestData);
  int httpCode = http.POST(httpRequestData);

        Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
  
  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
        Serial.println("Silahkan masuk!");
      Serial.println(payload);
    }else if(httpCode == HTTP_CODE_BAD_REQUEST){
     String payload = http.getString();
     Serial.println("ID card anda sudah terdaftar!");
  }
  }else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  Serial.println("keluar if");
  http.end();

}

void setup() {
  Serial.begin(9600);

  reconnect();

  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  //If you set Antenna Gain to Max it will increase reading distance
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

unsigned long getCardUID() {
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
 if (WiFi.status() != WL_CONNECTED)
    reconnect();

  if (wait > 50) {
    Serial.println("");
//    lcd.setCursor(0,0);
//    lcd.print("Scan kartu anda ");
    wait = 0;
  }

  Serial.print("");
//  lcd.setCursor(0,1);
//  lcd.print(".......................");

  wait++;

  if (wait % 2 == 0)
//    digitalWrite(D1, HIGH);
Serial.print("");
  else
//    digitalWrite(D1, LOW);
Serial.print("");

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(30);
    return;
  }

  unsigned long cardUID = getCardUID();

  for (int i = 0; i < 3; i++) {
//    digitalWrite(D1, HIGH);
Serial.print("");
    delay(30);
//    digitalWrite(D1, LOW);
Serial.print("");
    delay(30);
  }

  if (cardUID == -1) {
    Serial.println("Failed to get UID");
//    lcd.setCursor(0,0);
//    lcd.print("Scan ulang kartu!");
    delay(1000);
    return;
  }

  Serial.printf("\nCard UID is %u\n", cardUID);

  for (int i = 0; i < 3; i++) {
//    digitalWrite(D1, HIGH);
Serial.print("");
    delay(30);
//    digitalWrite(D1, LOW);
Serial.print("");
    delay(30);
  }
  
  Serial.println("masuk dunia kegelapan");

  logToServer(cardUID);

  wait = 51;
  Serial.println("ngelewatin wait");
}
