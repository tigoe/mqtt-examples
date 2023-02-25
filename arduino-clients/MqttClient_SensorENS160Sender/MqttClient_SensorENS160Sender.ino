/*
  This sketch demonstrates an MQTT client that connects to a broker, 
  and publishes messages to it from an ENS160 AQI sensor. Shows how to
  send JSON strings with just the String object. 

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
  * http://librarymanager/All#SparkFun_ENS160

  the circuit:
  - ENS160 sensor attached to SDA and SCL of the Arduino
  
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  updated 25 Feb 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include "SparkFun_ENS160.h"
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "AQISensor";
String clientID = "AQISensorClient-";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 10 * 1000;

// instance of the ENS160 sensor library:
SparkFun_ENS160 AQISensor;

void setup() {
   // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);

  // initialize WiFi, if not connected:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());
  // make the clientID unique by adding the last three digits of the MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 0; i < 3; i++) {
    clientID += String(mac[i], HEX);
  }
  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  // if needed, login to the broker with a username and password:
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

  // attempt to start the sensor:
  Wire.begin();
  if (!AQISensor.begin(0x53)) {
    Serial.println("Sensor is not responding. Check wiring.");
    // stop the program here if the sensor didn't respond:
    while (true)
      ;
  }

  // Reset the indoor air quality sensor's settings:
  if (AQISensor.setOperatingMode(SFE_ENS160_RESET)) {
    Serial.println("Ready.");
  }
  delay(100);
  // Set to standard operation:
  AQISensor.setOperatingMode(SFE_ENS160_STANDARD);
}

void loop() {
 // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("attempting to connect to broker");
    connectToBroker();
  }

  // once every interval, send a message:
  if (millis() - lastTimeSent > interval) {

    // the sensor can have four possible states:
    // 0 - Operating ok: Standard Opepration
    // 1 - Warm-up: occurs for 3 minutes after power-on.
    // 2 - Initial Start-up: Occurs for the first hour of operation.
    //												and only once in sensor's lifetime.
    // 3 - No Valid Output
    int ensStatus = AQISensor.getFlags();
    // if the sensor's ready to read, read it:
    if (AQISensor.checkDataStatus()) {
      int aqi = AQISensor.getAQI();
      int tvoc = AQISensor.getTVOC();
      int eCO2 = AQISensor.getECO2();

      // put them into a JSON String:
      String payload = "{\"sensor\": \"ENS160\",";
      payload += "\"aqi\": AQI, \"tvoc\": TVOC, \"eCO2\": ECO2, \"status\": STATUS}";
      // replace the value substrings with actual values:
      payload.replace("AQI", String(aqi));
      payload.replace("TVOC", String(tvoc));
      payload.replace("ECO2", String(eCO2));
      payload.replace("STATUS", String(ensStatus));

      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the payload of the message:
      mqttClient.println(payload);
      // send the message:
      mqttClient.endMessage();
      // timestamp this message:
      lastTimeSent = millis();
    }
  }
}

boolean connectToBroker() {
  // if the MQTT client is not connected:
  if (!mqttClient.connect(broker, port)) {
    // print out the error message:
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    // return that you're not connected:
    return false;
  }
  // once you're connected, you
  // return that you're connected:
  return true;
}