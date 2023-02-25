/*
  MQTT Client sender/receiver

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and both listens for messages on that topic and sends messages to it, a random number between 0 and 255.
  When the client receives a message, it parses it, and PWMs the built-in LED.

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  updated 25 Feb 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Uno WiFi
// #include <WiFi101.h>    // use this for MKR1000
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "aardvarks";
String clientID = "arduinoMqttClient-";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 10 * 1000;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);
  pinMode(LED_BUILTIN, OUTPUT);
  // connect to WiFi:
  connectToNetwork();
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
}

void loop() {
  // if you disconnected from the network, reconnect:
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
    // get a random analog reading, divide by 4 to set range to 0-255:
    int sensorReading = analogRead(A0) / 4;
    if (mqttClient.connected()) {
      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the body of the message:
      mqttClient.print(sensorReading);
      // send the message:
      mqttClient.endMessage();
      // send a serial notification:
      Serial.print("published a message: ");
      Serial.println(sensorReading);
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
  // convert the incoming string to an int so you can use it:
  int result = incoming.toInt();
  // use the result to dim the builtin LED:
  if (result > 0) {
    analogWrite(LED_BUILTIN, result);
  }
  // print the result:
  Serial.println(result);
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