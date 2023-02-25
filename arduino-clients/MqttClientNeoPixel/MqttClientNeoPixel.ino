/*
  MQTT NeoPixel Client 

  This sketch shows how to control NeoPixels with MQTT. 
  It expects a comma-separated string of values for red, green, and blue
  in the incoming topic. When it gets that, it uses the values to 
  change the color of the strip. 

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

  The circuit:
  * NeoPixels attached to pin 5

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
  * http://librarymanager/All#Adafruit_NeoPixel

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
#include <Adafruit_NeoPixel.h>
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

const int neoPixelPin = 5;  // control pin
const int pixelCount = 7;   // number of pixels

// set up NeoPixels:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelCount, neoPixelPin, NEO_GRBW + NEO_KHZ800);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "color";
String clientID = "neoPixelClient-";

// intensity of LED:
int intensity = 0;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);
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
  // initialize the NeoPixels:
  strip.begin();
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
  // subscribe to a topic and all subtopics:
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  mqttClient.subscribe(topic);

  // once you're connected, you
  // return that you're connected:
  return true;
}

void onMqttMessage(int messageSize) {
  // initialize variables for red, green, blue:
  int r, g, b = 0;
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic ");
  Serial.println(mqttClient.messageTopic());
  Serial.print(" values: ");
  // read the incoming message as a comma-separated string of values:
  while (mqttClient.available()) {
    int r = mqttClient.parseInt();
    int g = mqttClient.parseInt();
    int b = mqttClient.parseInt();
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.println(b);
    // fill the strip with color:
    strip.fill(r, g, b);
    // update the strip:
    strip.show();
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
  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());
}