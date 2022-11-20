/*
  MQTT Client sender with ENS160 AQI sensor

  Sends the values from an ENS160 Air Quality Sensor as a JSON string
  to an MQTT broker. 

  This sketch uses https://test.mosquitto.org as the MQTT broker.

  The ENS160 sensor is attached to the Arduino via the I2C interface. 

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER    // MQTT username if needed
  #define SECRET_MQTT_PASS    // MQTT passwd if needed

  created 11 June 2020
  updated 19 Nov 2022
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include "SparkFun_ENS160.h"
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// instance of the ENS160 sensor library:
SparkFun_ENS160 myENS;

// details for MQTT client:
char broker[] = "test.mosquitto.org";
int port = 1883;
char topic[] = "undnet/yourName/AQI";
char clientID[] = "yourNameAQIClient";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 60 * 1000;

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

  // attempt to start the sensor:
  Wire.begin();
  if (!myENS.begin(0x53)) {
    Serial.println("Sensor is not responding. Check wiring.");
    // stop the program here if the sensor didn't respond:
    while (true)
      ;
  }

  // Reset the indoor air quality sensor's settings:
  if (myENS.setOperatingMode(SFE_ENS160_RESET)) {
    Serial.println("Ready.");
  }
  delay(100);
  // Set to standard operation:
  myENS.setOperatingMode(SFE_ENS160_STANDARD);

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  // if needed, login to the broker with a username and password:
  //mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

  // try to connect to the MQTT broker once you're connected to WiFi:
  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
}

void loop() {
  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
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
    int ensStatus = myENS.getFlags();
    // if the sensor's ready to read, read it:
    if (myENS.checkDataStatus()) {
      int AQI = myENS.getAQI();
      int TVOC = myENS.getTVOC();
      int eCO2 = myENS.getECO2();

      // put them into a JSON String:
      String body = "{\"AQI\": aqi, \"TVOC\": tvoc, \"eCO2\": eco2, \"status\": ensStat}";
     // replace the value substrings with actual values:
      body.replace("aqi", String(AQI));
      body.replace("tvoc", String(TVOC));
      body.replace("eco2", String(eCO2));
      body.replace("ensStat", String(ensStatus));

      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the body of the message:
      mqttClient.println(body);
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