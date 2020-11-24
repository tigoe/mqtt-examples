/*
  MQTT Client that sends MIDI

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and  sends  messages on that topic. The messages are three-byte arrays that are MIDI
  noteon and noteoff messages

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker. It uses SSL
  via the WiFiSSLClient, on port 8883

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 24 Nov 2020
  by Tom Igoe
*/
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);
const int buttonPin = 4;


// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "midi";
char clientID[] = "arduinoMidiControllerClient";

// musical items:
int major[] = {2, 2, 1, 2, 2, 2, 1};
// an array to hold the final notes of the scale:
int scale[8];

// start with middle C:
int tonic = 48;  // MIDI note value for middle C
// note to play:
int noteValue = tonic;

// previous state of the button:
int lastButtonState = LOW;
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

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

  // fill the scale array with the scale you want:
  // start with the initial note:
  scale[0] = tonic;
  int note = scale[0];
  // iterate over the intervals, adding each to the next note
  // in the scale. You can change major to naturalMinor
  // if you want that kind of scale instead:
  for (int n = 0; n < 7; n++) {
    note = note + major[n];
    scale[n + 1] = note;
  }
}

void loop() {
  // if not connected, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
    connectToBroker();
  }

  // read the pushbutton:
  int buttonState = digitalRead(buttonPin);
  // compare its state to the previous state:
  if (buttonState != lastButtonState) {
    // debounce delay:
    delay(5);
    // if the button's changed and it's pressed:
    if (buttonState == LOW) {
      // pick a random note in the scale:
      noteValue = scale[random(8)];
      // play it:
      sendMqttMessage(0x90, noteValue, 0x7F);
    } else  {
      // turn the note off:
      sendMqttMessage(0x80, noteValue, 0);
    }
    // save the button state for comparison next time through:
    lastButtonState = buttonState;
  }


  // if a message comes in, read it.
  // let's assume it's a MIDI message:
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


void sendMqttMessage(byte cmd, byte data1, byte  data2) {
  mqttClient.beginMessage(topic);
  mqttClient.write(cmd);
  mqttClient.write(data1);
  mqttClient.write(data2);
  // send the message:
  mqttClient.endMessage();
}
