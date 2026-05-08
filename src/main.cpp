#include <Arduino.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* SSID_WIFI = "=)";
const char* PASS_WIFI = "12345678";
unsigned long ChannelID =  3374169;
const char* myWriteAPIKey = "ENCQU6T9ZX7OZGQS";

#define DHTPIN 4
#define DHTTYPE DHT22
#define MQPIN 35

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
unsigned long LastTime = 0;

void setup() {

    Serial.begin(115200);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Eroare: Ecranul nu a fost gasit!");
        for(;;);
    }

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Initializare...");
    display.display();

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_WIFI, PASS_WIFI);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Conectat la WiFi!");

    ThingSpeak.begin(client);
    analogSetAttenuation(ADC_11db);
    dht.begin();

}

void loop() {

float h = dht.readHumidity();
float t = dht.readTemperature();
int aq = analogRead(MQPIN);

if(isnan(h) || isnan(t)){
    Serial.println("Eroare citire DHT!");
    return;
}

Serial.printf("Temp: %.1fC | Hum: %.1f%% | AirQ: %d\n", t, h, aq);

display.clearDisplay();
display.setTextSize(1);
display.setCursor(0, 0);
display.println("Monitor Mediu");
display.printf("Temp: %.1fC\n", t);
display.printf("Hum: %.1f%%\n", h);
display.printf("AirQ: %d\n", aq);
display.display();

if(millis() - LastTime > 20000) {
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);
    ThingSpeak.setField(3, aq);

    int x = ThingSpeak.writeFields(ChannelID, myWriteAPIKey);
    if( x == 200){
        Serial.println("Cloud actualizat");
    } 
    else {
        Serial.println("Eroare incarcare cloud");
    }
    LastTime = millis();
}

delay(2000);

}
