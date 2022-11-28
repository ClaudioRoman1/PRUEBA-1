// ARCHIVO DE ESP8266//
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define SS_PIN D4
#define RST_PIN D2

#define WIFI_SSID "Claudio Roman"
#define WIFI_PASSWORD "12345678"


MFRC522 mfrc522(SS_PIN, RST_PIN);
void setup() {
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID , WIFI_PASSWORD );
  Serial.println("");


  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID );
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  SPI.begin();
  mfrc522.PCD_Init();
}    // ESTE ES EL ARCHIVO QUE COMPILE 19/07

void sendRfidLog(String cardid) {
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    cardid=cardid;
    String postData = "?cardid="+String(cardid);
    http.begin(client, "http://172.20.10.1/a/post.php?cardid=");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 

    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
     http.end();
    if (httpCode != -1) {
      Serial.println("El Servidor respondió");
      Serial.println(payload);
    
    } else {
      
      Serial.println(cardid);
      Serial.println(postData);
      Serial.println(" No se pudieron enviar estos datos al servidor");
    }
     
    delay(500);
  }
}


void loop() {
 int i = 1;
 while (i == 1) {
    if (WiFi.status() != WL_CONNECTED)
    {
      return;
    } else if (!mfrc522.PICC_IsNewCardPresent())
    {
      return;
    } else if (!mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    String serial = "";
    for (int x = 0; x < mfrc522.uid.size; x++)
    {
      // If it is less than 10, we add zero
      if (mfrc522.uid.uidByte[x] < 0x10)
      {
        serial += "0";
      }
      // Transform the byte to hex
      serial += String(mfrc522.uid.uidByte[x], HEX);
      // Add a hypen
      if (x + 1 != mfrc522.uid.size)
      {
        serial += "-";
      }
    }
    serial.toUpperCase();
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    
    sendRfidLog(serial);
    // Stop encryption on PCD.
    mfrc522.PCD_StopCrypto1();
   

 
    delay(100);
 
  }

}
