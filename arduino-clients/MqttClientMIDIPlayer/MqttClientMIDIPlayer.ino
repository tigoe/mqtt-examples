/*
  MQTT Client that plays MIDI

  This sketch demonstrates an MQTT client that connects to a broker, subsrcibes to a topic,
  and  listens for messages on that topic. When it receives a three-byte message,
  it uses it to play a MIDI note. It doesn't attempe to interpret the MIDI message, it
  just plays it

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker. It uses SSL
  via the WiFiSSLClient, on port 8883

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  modified 23 Nov 2020
  by Tom Igoe
*/
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <MIDIUSB.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "midi";
char clientID[] = "arduinoMidiClient";

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  while (!Serial);

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
  // use this if you're using a personal account on shiftr.io:
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
}

void loop() {
  // if not connected, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
    connectToBroker();
  }

  // if a message comes in, read it:
  int messageSize = mqttClient.parseMessage();
  if (messageSize > 0) {
    // set up an array for the MIDI bytes:
    byte message[messageSize];
    Serial.print("Got a message on topic: ");
    Serial.println(mqttClient.messageTopic());
    // message byte counter:
    int i = 0;
    // read the message:
    while (mqttClient.available()) {
      // convert numeric string to an int:
      message[i] = mqttClient.read();
      Serial.print(message[i], HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    // play a note:
    midiCommand(message[0], message[1], message[2]);
  }
}

boolean connectToBroker() {
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    return false;
  }
  // once you're connected, you can proceed:
  mqttClient.subscribe(topic);
  return true;
}


// send a 3-byte midi message
void midiCommand(byte cmd, byte data1, byte  data2) {
  // First parameter is the event type (top 4 bits of the command byte).
  // Second parameter is command byte combined with the channel.
  // Third parameter is the first data byte
  // Fourth parameter second data byte

  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}
