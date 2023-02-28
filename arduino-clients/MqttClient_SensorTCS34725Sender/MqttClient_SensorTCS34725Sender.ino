/*
  MQTT Client  Light sensor sender/receiver

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and both listens for messages on that topic and sends messages to it.

  Uses a TCS34725 light sensor to read lux and color temperature

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
  * http://librarymanager/All#Adafruit_TCS34725 (for the sensor)

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  updated 28 Feb 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Uno WiFi
// #include <WiFi101.h>    // use this for MKR1000
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "conndev/tigoe";
String clientID = "light-client-";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 60 * 1000;

// initialize the light sensor:
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);
// string for the MAC address:
String macAddr;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);

  // get MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  // put it in a string:
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) macAddr += "0";
    macAddr += String(mac[i], HEX);
  }

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  // add mac address to make the clientID unique:
  clientID += macAddr;
  // if needed, login to the broker with a username and password:
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
}

void loop() {
  //if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
    // skip the rest of the loop until you are connected:
    return;
  }

  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("attempting to connect to broker");
    connectToBroker();
  }
  // poll for new messages from the broker:
  mqttClient.poll();

  // once every interval, send a message:
  if (millis() - lastTimeSent > interval) {
    // read the sensor
    // get lux and color temperature from sensor:
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

    if (mqttClient.connected()) {
      // start a new message on the topic:
      mqttClient.beginMessage(topic + String("/lux"));
      // print the body of the message:
      mqttClient.print(lux);
      // send the message:
      mqttClient.endMessage();
      // same pattern for other subtopics:
      mqttClient.beginMessage(topic + String("/ct"));
      mqttClient.print(colorTemp);
      mqttClient.endMessage();
      // include MAC address as an ID for this device:
      mqttClient.beginMessage(topic + String("/id"));
      mqttClient.print(macAddr);
      mqttClient.endMessage();
      // send a serial notification:
      Serial.print("published a set of readings: \nlux: ");
      Serial.print(lux);
      Serial.print(",  ct: ");
      Serial.print(colorTemp);
      Serial.print(",  id: ");
      Serial.println(macAddr);
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

  // set the message receive callback:
  mqttClient.onMessage(onMqttMessage);
  // subscribe to a topic:
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  mqttClient.subscribe(topic);

  // once you're connected, you
  // return that you're connected:
  return true;
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic ");
  Serial.print(mqttClient.messageTopic());
  Serial.print(", length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  String incoming = "";
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    incoming += (char)mqttClient.read();
  }

  // print the incoming message:
  Serial.println(incoming);
}

void connectToNetwork() {
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());
}
