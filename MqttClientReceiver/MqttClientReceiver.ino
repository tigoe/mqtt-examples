/*
  MQTT Client with an LED

  This sketch demonstrates an MQTT client that connects to a broker, subsrcibes to a topic,
  and  listens for messages on that topic and sends messages to it. 
  
  When the client receives a message, it parses it, and if the number matches the client's
  number (myNumber, chosen arbitrarily), it sets an LED to full. When nothing is happening,
  if the LED is not off, it's faded down one point every time through the loop.

  This sketch uses https://next.shiftr.io/try as the MQTT broker.

  the circuit:
  - LED's anode connected to pin 3, cathode connected to ground.

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  updated 23 Nov 2020
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 1883;
char topic[] = "notes";
char clientID[] = "ledClient";

// number value of incoming message that will light the LED:
int myNumber = 3;
// intensity of LED:
int intensity = 0;

// details for LED:
const int ledPin = 3;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  while (!Serial);

  // initialize I/O pin:
  pinMode (ledPin, OUTPUT);

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

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

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

  // if a message comes in, read it:
  if (mqttClient.parseMessage() > 0) {
    Serial.print("Got a message on topic: ");
    Serial.println(mqttClient.messageTopic());
    // read the message:
    while (mqttClient.available()) {
      // convert numeric string to an int:
      int message = mqttClient.parseInt();
      Serial.println(message);
      // if the message matches client's number, set the LED to full intensity:
      if (message == myNumber) {
        intensity = 255;
      }
    }
  }
  // if the LED is on:
  if (intensity > 0) {
    // update its level:
    analogWrite(ledPin, intensity);
    // fade level down one point for next time through loop:
    intensity = max(intensity--, 0);
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
  // once you're connected, you can proceed:
  mqttClient.subscribe(topic);
  // return that you're connected:
  return true;
}
