/*
  MQTT Client Property Changer using subtopics

  This sketch shows how to use MQTT subtopics to get and set 
  properties of a microcontroller's program. 
  The microcontroller has two blinking LEDs and an analog sensor attached. 
  You can change the behavior of the LEDs remotely by publishing 
  to the subtopics `/brightness` and `/blinkInterval` and change 
  the sensor update rate by publishing to `/sendInterval`.

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

  The circuit:
  * any analog input on pin A0;
  * LED on digital pin 2;
  * built-in LED on pin LED_BUILTIN

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 5 Jan 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Uno WiFi
// #include <WiFi101.h>    // use this for MKR1000
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// an LED to dim is on pin 2:
const int ledPin = 2;

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
String topic = "ocelots";
char clientID[] = "arduinoMqttClient";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int sendInterval = 10 * 1000;
// built-in LED blink interval:
int blinkInterval = 1000;

// properties for the LED on pin 2:
int brightness = 255;
int lastBrightness = brightness;
// property for built-in LED:
int status = HIGH;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  // connect to WiFi:
  connectToNetwork();

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
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

  // once every interval, read the sensor and publish.
  // also publish  sendInterval,  brightness and  blinkInterval:
  if (millis() - lastTimeSent > sendInterval) {
    // get an analog reading:
    int sensorReading = analogRead(A0) ;
    // make a subtopic for it:
    String subTopic = topic + String("/sensor");
    mqttUpdate(subTopic, String(sensorReading));
      // make a subtopic for the other variables too:
    subTopic = topic + String("/sendInterval");
    mqttUpdate(subTopic, String(sendInterval));
    subTopic = topic + String("/brightness");
    mqttUpdate(subTopic, String(brightness));
    subTopic = topic + String("/blinkInterval");
    mqttUpdate(subTopic, String(blinkInterval));
  }
  // if  brightness has changed, change the dimming LED:
  if (brightness != lastBrightness) {
    analogWrite(ledPin, brightness);
    lastBrightness = brightness;
  }
  // if blinkInterval has passed, change the state of the blinking LED:
  if (millis() - lastTimeSent > blinkInterval) {
    digitalWrite(LED_BUILTIN, status);
    status = !status;
    lastTimeSent = millis();
  }
}
// publish a message to a topic:
void mqttUpdate(String thisTopic, String message) {
  if (mqttClient.connected()) {
    // start a new message on the topic:
    mqttClient.beginMessage(thisTopic);
    // print the body of the message:
    mqttClient.print(message);
    // send the message:
    mqttClient.endMessage();
    // send a serial notification:
    Serial.print("published to: ");
    Serial.print(thisTopic);
    Serial.print(",  message: ");
    Serial.println(message);
    // timestamp this message:
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
  // subscribe to a topic and all subtopics:
  Serial.print("Subscribing to topic: ");
  Serial.println(topic + "/#");
  mqttClient.subscribe(topic + "/#");

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
  // get the subtopic. Assumes the messageTopic is "topic/" then something:
  String subTopic = mqttClient.messageTopic().substring(topic.length() + 1);
  // an empty string to read the result
  String incoming = "";
  // read the contensts into the string:
  while (mqttClient.available()) {
    incoming += (char)mqttClient.read();
  }
  Serial.println(incoming);

  // change the appropriate variable depending on the subTopic:
  if (subTopic == "sendInterval") {
    sendInterval = incoming.toInt();
  }
  if (subTopic == "brightness") {
    brightness = incoming.toInt();
  }
  if (subTopic == "blinkInterval") {
    blinkInterval = incoming.toInt();
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