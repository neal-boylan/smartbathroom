#include "config.h"

#include <SPI.h>
#include <WiFi.h>
#include <BlynkSimpleWifi.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <Arduino_MKRIoTCarrier.h>
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = WIFI_NAME;
char pass[] = WIFI_PASSWORD;

//Define Firebase data object
FirebaseData fbdo;
const long interval = 5000;
unsigned long previousMillis = millis();
unsigned long currentMillis = millis();
int count = 0;

BlynkTimer timer;
MKRIoTCarrier carrier;

int value;
int tempSet;
int humiditySet;
float temperature;
float humidity;
bool oldFanState = 0;
bool newFanState = 0;
bool oldHeaterState = 0;
bool newHeaterState = 0;

void setup() {
  // Debug console
  Serial.begin(9600);

  //Provide the autntication data
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, WIFI_NAME, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  int defaultTemp = 15;
  int defaultHumidity = 20;
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  //Set Blynk widgets to default values
  Blynk.virtualWrite(V0, LOW);
  Blynk.virtualWrite(V1, LOW);
  Blynk.virtualWrite(V3, defaultTemp);
  Blynk.virtualWrite(V4, defaultHumidity);

  tempSet = defaultTemp;
  humiditySet = defaultHumidity;

  // Setup a function to be called every second
  timer.setInterval(2000L, writeEnvData);
  carrier.withCase();                         //Remove this if your arduino is NOT in it's case/housing
  carrier.begin();
  carrier.display.fillScreen(0x07E0);
  carrier.leds.setBrightness(75);
}

void loop() {
  Blynk.run();
  timer.run();
  displayEnv();
  heaterFunction();
  fanFunction();
}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  value = param.asInt(); // Get value as integer
  if (value) {
    sendMessage("Smart Bathroom Active");
    count++;
    heaterFunction();
    fanFunction(); 
  } else {
    sendMessage("Smart Bathroom Inactive"); 
    carrier.leds.setPixelColor(1, 0, 0, 0); 
    carrier.leds.setPixelColor(3, 0, 0, 0); 
    //Switch off Heater and Fan via Relay
    carrier.Relay1.open();
    carrier.Relay2.open();
    count++;
  }
  carrier.leds.show();

}

// This function is called every time the Virtual Pin 3 state changes
BLYNK_WRITE(V3) {
  // Set incoming value from pin V3 to a variable
  tempSet = param.asDouble();
  heaterFunction();
}

// This function is called every time the Virtual Pin 4 state changes
BLYNK_WRITE(V4) {
  // Set incoming value from pin V4 to a variable
  humiditySet = param.asInt();
  fanFunction();
}

// This function sends temperature every second to Virtual Pin 1.
void writeEnvData() {
  // Don't send more that 10 values per second.
  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();
 
  //Send humidity data to Blynk graph
  Blynk.virtualWrite(V2, humidity);

  Serial.println("TempSet: " + String(tempSet) + "°C; Temp: " + String(temperature));
  Serial.println("HumdSet: " + String(humiditySet) + "% ; Humd: " + String(humidity) + "\n");
  //If temperature is above the user set threshold
  //send data to Blynk so that notification can be sent
  if (value) {
    if (temperature >= tempSet) {
      Blynk.virtualWrite(V1, 1);
    } else{
      Blynk.virtualWrite(V1, 0);
    }
  }
}

//Function to display temperature and humidity 
//readings to arduino MKR display
void displayEnv() {
  carrier.display.fillScreen(ST77XX_BLACK); //oled clear()
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(50, 50);
  carrier.display.println("Humidity");
  carrier.display.setCursor(70, 75);
  carrier.display.print(humidity);
  carrier.display.print("%");
  carrier.display.setCursor(20, 115);
  carrier.display.println("Temperature");
  carrier.display.setCursor(70, 140);
  carrier.display.print(temperature);
  carrier.display.print("C");
  //delay(2500);
}

void heaterFunction(){
  //Check if the Smart Bathroom is active
  if (value) {
    //Check if temperature is less than required
    if (carrier.Env.readTemperature() < tempSet) {
      carrier.leds.setPixelColor(1, 255, 20, 20);  
      carrier.Relay1.close();
      newHeaterState = 1;
      //Check if heater is already on
      if (newHeaterState != oldHeaterState){
        oldHeaterState = newHeaterState;
        sendMessage("heaterOn");
        count++;
      }
    } else {
      carrier.leds.setPixelColor(1, 5, 5, 5);
      carrier.Relay1.open();
      newHeaterState = 0; 
      if (newHeaterState != oldHeaterState){
        oldHeaterState = newHeaterState; 
        sendMessage("heaterOff");
        count++;
      }
    }
  }
  carrier.leds.show();
}

void fanFunction() {
  if (value) {
    if (carrier.Env.readHumidity() > humiditySet) {
      carrier.leds.setPixelColor(3, 20, 20, 255); 
      carrier.Relay2.close();
      newFanState = 1;
      if (newFanState != oldFanState){
        oldFanState = newFanState;
        sendMessage("fanOn");
        count++; 
      }
    } else {
      carrier.leds.setPixelColor(3, 5, 5, 5); 
      carrier.Relay2.open();
      newFanState = 0;
      if (newFanState != oldFanState){
        oldFanState = newFanState;
        sendMessage("fanOff");
        count++; 
      }
    }
  } 
  carrier.leds.show();
}

void sendMessage(String event) {
  // send message, the Print interface can be used to set the message contents
  String path = "/events";
  String jsonStr;
  unsigned long timestamp = millis();
  //float temperature = carrier.Env.readTemperature();

  Serial.println("Pushing json... ");
  // Constuct JSON string. the \" is used to escape the double quote so that it  treats the double quote as part of the string
  jsonStr = "{\"event\":\"" + event + "\",\"temperature\":"+ String(temperature) + ",\"tempSet\":"+ String(tempSet) + ",\"humidity\":"+ String(humidity) +",\"humiditySet\":"+ String(humiditySet) + ",\"timeStamp\":"+ String(timestamp) + ",\"count\":"+ String(count) + "}";
  Serial.println(jsonStr);
  if (Firebase.pushJSON(fbdo, path + "/button", jsonStr)) {
    Serial.println("path: " + fbdo.dataPath());
  } else {
    Serial.println("error, " + fbdo.errorReason());
  }
  fbdo.clear();
}