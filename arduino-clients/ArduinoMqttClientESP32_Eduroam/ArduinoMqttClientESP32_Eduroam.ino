/*
  MQTT Client sender/receiver using on Nano ESP32 
  on WPA2-enterprise on on Eduroam

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and both listens for messages on that topic and sends messages to it, an analog reading.
  When the client receives a message, it prints it.

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

 Here's what your arduino_secrets.h file should look like:
  #define EAP_IDENTITY ""      // use identity@institution.domain in for most institutions 
  #define EAP_USERNAME ""      // usually just a repeat of the identity
  #define EAP_PASSWORD ""      // your  password
  #define SECRET_SSID "eduroam"    // eduroam SSID
  #define SECRET_MQTT_USER "public" // MQTT broker username
  #define SECRET_MQTT_PASS "public" // MQTT broker password

Libraries used:
  * http://librarymanager/All#WiFi // for Nano ESP32 WiFi 
  * esp_wpa2.h for Nano ESP23 WiFi WPA2-enterprise
  * http://librarymanager/All#ArduinoMqttClient

  the arduino_secrets.h file:
  #define EAP_IDENTITY ""      // use identity@institution.domain in for most institutions 
  #define EAP_USERNAME ""      // usually just a repeat of the identity
  #define EAP_PASSWORD ""      // your  password
  #define SECRET_SSID "eduroam"    // eduroam SSID
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 4 Mar 2024
  modified 14 Feb 2025
  by Tom Igoe
*/

#include <WiFi.h>      // use this for Nano ESP32
#include "esp_wpa2.h"  //wpa2 library for connections to Enterprise networks
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 1883;
char topic[] = "aardvarks";
String clientID = "nanoESPClient-";

// sending interval:
const long interval = 120 * 1000;
// last time you sent a reading:
int lastTimeSent = 0;

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
  // add the mac address to the clientID for a unique username:
  for (int i = 0; i < 3; i++) {
    clientID += String(mac[i], HEX);
  }
  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  Serial.println("client ID:" + clientID);
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
    if (millis() - lastTimeSent < interval && lastTimeSent != 0) return;
  // read analog in on pin A0:
  int sensor = analogRead(A0);

  // if the client's connected:
  if (mqttClient.connected()) {
    // start a new message on the topic:
    mqttClient.beginMessage(topic);
    // print the body of the message:
    mqttClient.print(sensor);
    // send the message:
    mqttClient.endMessage();
    // send a serial notification:
    Serial.print("published a message: ");
    Serial.println(sensor);
    lastTimeSent = millis();
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
  delay(100);
}

void connectToNetwork() {
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
    delay(2000);
  }

  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());
}