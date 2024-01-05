PROJECT TITLE
Smart Bathroom

PROJECT DESCRIPTION
Use the Arduino Explore IoT Kit Rev2 to control an extractor fan and heating element.
The Arduino is connected to the internet via WiFi and can be controlled using
MQTT with Blynk. Data is stored in a Firebase RTDB and uploaded to a webpage.

TECHNOLOGIES USED
C++, JavaScript, HTML, MQTT, HTTP Web Sockets, WiFi

REQUIREMENTS
Arduino MKR WiFi 1010 board
Arduino MKR IoT Carrier Rev2
WiFi internet access 
PC/mobile phone with internet connection
Blynk account
Firebase account

HOW TO
-Create a Blynk account 
-Create a RTDB on Firebase 
-Upload the C++ code to Arduino using the Arduino IDE
-Enter relevant details and passwords in the config.h file on Arduino IDE
-Match up datastreams on Blynk app to those in the C++ code. 
 V0 to activate, V1 to check status, V2 to measure humidity,
 V3 to set temperature, V4 to set humidity.
-Connect Heater and Fan to the Relay Connectors on the MKR IoT Carrier.
-Set desired temperature and humidity via the Blynk App
-Activate Smart Bathroom using button on the app