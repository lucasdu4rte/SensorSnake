#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include <ArduinoJson.h>

#include "authUtils.h" /* Variáveis globais */

#define DHTPIN D5 /* Pino do DHT */
#define DHTTYPE DHT11 /* Tipo do sensor usado */
#define TABLE_NAME "leituras" /* Nome da tabela no FirebaseDB */

#define PUBLISH_INTERVAL 1000*60*1 /* 1 minuto entre cada publicação */

DHT dht(DHTPIN, DHTTYPE); /* Integrando Lib com Pino */

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) { 
      Serial.print(".");
      delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(1000);
  Serial.println("Authenticated to Firebase.");
}

StaticJsonBuffer<200> jsonBuffer;
JsonObject &root = jsonBuffer.createObject();

void loop() {
  delay(PUBLISH_INTERVAL);
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (!isnan(humidity) && !isnan(temperature))
  {
    root["temperature"] = temperature;
    root["humidity"] = humidity;
    
    /* Publicando no Firebase */
    Firebase.push(TABLE_NAME, root);
    if (Firebase.failed()) {
      Serial.print( String(TABLE_NAME) +" erro ao enviar: ");
      Serial.println(Firebase.error());
      return;
    }
    Serial.print("Temp. = ");
    Serial.print(temperature);
    Serial.print(" C ");
    Serial.print("Um. = ");
    Serial.print(humidity);
    Serial.println(" % ");
    Serial.println("Published!");
  }
  else
  {
    Serial.println("Error Publishing");
  }
}
