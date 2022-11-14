/*
  Datalogger MQTT client

  Connects to an MQTT broker and uploads data.

  Works with MKR1010, MKR1000, Nano 33 IoT
  Uses the following libraries:
   http://librarymanager/All#WiFi101  // use this for MKR1000
   http://librarymanager/All#WiFiNINA  // use this for MKR1010, Nano 33 IoT
   http://librarymanager/All#ArduinoMqttClient
   http://librarymanager/All#Adafruit_TCS34725 (for the sensor)

  created 13 Jun 2021
  modified 14 Nov 2022
  by Tom Igoe
*/
// include required libraries and config files
//#include <WiFi101.h>        // for MKR1000 modules
#include <WiFiNINA.h>  // for MKR1010 modules and Nano 33 IoT modules
#include <ArduinoMqttClient.h>
// I2C and light sensor libraries:
#include <Wire.h>
#include <Adafruit_TCS34725.h>
// network names and passwords:
#include "arduino_secrets.h"

// initialize WiFi connection using SSL
// (use WIFiClient and port number 1883 for unencrypted connections):
WiFiClient wifi;
MqttClient mqttClient(wifi);
String addressString = "";

// details for MQTT client:
char broker[] = "test.mosquitto.org";
int port = 1883;
char topic[] = "undnet/tigoe";
String clientID = "light-client-tigoe";

// timestamp for the sensor reading and send:
long lastSendTime = 0;

// interval between requests:
int sendInterval = 60 * 1000;
// initialize the light sensor:
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

void setup() {
  Serial.begin(9600);  // initialize serial communication
  // if serial monitor is not open, wait 3 seconds:
  if (!Serial) delay(3000);

  // set the credentials for the MQTT client:
   mqttClient.setId(clientID);
  // // login to the broker with a username and password:
  // mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

// connect to WiFi
  connectToNetwork();

  // try to connect to the MQTT broker once you're connected to WiFi:
  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
}

void loop() {
  //if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  }

  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting to broker");
    connectToBroker();
  } else {
    // If the client is not connected:  if (!client.connected()) {
    // and the request interval has passed:
    if (millis() - lastSendTime >= sendInterval) {
      // read the sensor
      // get lux and color temperature from sensor:
      uint16_t r, g, b, c, colorTemp, lux;
      tcs.getRawData(&r, &g, &b, &c);
      colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
      lux = tcs.calculateLux(r, g, b);
      String body = "{\"lux\": ll, \"ct\", cc}";
      body.replace("ll", String(lux));
      body.replace("cc", String(colorTemp));

      // send:
      mqttClient.beginMessage(topic);
      // add the value:
      mqttClient.print(body);
      // send the message:
      mqttClient.endMessage();

      // take note of the time you make your request:
      lastSendTime = millis();
    }
  }
}


void connectToNetwork() {
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected.");
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
  mqttClient.subscribe(topic);
  // once you're connected, you
  // return that you're connected:
  return true;
}