/*
  MQTT Client that sends MIDI

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and  sends  messages on that topic. The messages are three-byte arrays that are MIDI
  noteon and noteoff messages.

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

  created 24 Nov 2020
  modified 5 Jan 2023
  by Tom Igoe
*/
#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Uno WiFi
// #include <WiFi101.h>    // use this for MKR1000
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
String clientID = "arduinoMidiControllerClient-";

// musical items:
int major[] = { 2, 2, 1, 2, 2, 2, 1 };
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
    } else {
      // turn the note off:
      sendMqttMessage(0x80, noteValue, 0);
    }
    // save the button state for comparison next time through:
    lastButtonState = buttonState;
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic ");
  Serial.print(mqttClient.messageTopic());
  Serial.print(", length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  if (messageSize > 0) {
    // set up an array for the MIDI bytes:
    byte message[messageSize];

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

void sendMqttMessage(byte cmd, byte data1, byte data2) {
  mqttClient.beginMessage(topic);
  mqttClient.write(cmd);
  mqttClient.write(data1);
  mqttClient.write(data2);
  // send the message:
  mqttClient.endMessage();
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
