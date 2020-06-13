/*
  MQTT Client that plays MIDI

  This sketch demonstrates an MQTT client that connects to a broker, subsrcibes to a topic,
  and  listens for messages on that topic. When it receives a message that's a numeric
  string between 0 and 15, it uses it to play a MIDI note.

  This sketch can play multiple notes simultaneously. Here's how:
  There's an array representing the possible notes you want to play, called scale[]
  and an array representing the last time any of those notes were played, called noteState[].
  If any of the notes in the scale[] are playing, its corresponding noteState[] will be
  the millis() when that note was started (when MIDI noteOn was called).

  For more on the music theory in this sketch, see https://tigoe.github.io/SoundExamples/melody.html
  and https://www.musictheory.net/lessons/21 and https://www.musictheory.net/lessons/24

  This sketch uses https://shiftr.io/try as the MQTT broker.

  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "" // broker username
  #define SECRET_MQTT_PASS "" // broker password

  created 11 June 2020
  by Tom Igoe
*/
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <MIDIUSB.h>
#include <pitchToNote.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "broker.shiftr.io";
int port = 1883;
char topic[] = "try/notes";
char clientID[] = "midiClient";

// how many octaves do you want to play?
const int octaves = 2;
// that determines how many notes you will have
const int noteCount = 8 * octaves;
// the intervals in a major scale:
int major[] = {2, 2, 1, 2, 2, 2, 1};
// an array to hold the final notes of the scale:
int scale[noteCount];
// start with middle C:
int tonic = pitchC4;
// note to play:
int noteValue = tonic;
// an array to hold all the notes that can
// play simultaneously:
int noteState[noteCount];

// duration of note in ms:
int noteDuration = 300;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  while (!Serial);

  // fill the scale array with the intervals you want:
  // start with the initial note:
  scale[0] = tonic;
  int note = scale[0];
  // iterate over the note intervals, adding each to the next note
  // in the scale:
  for (int n = 0; n < noteCount - 1; n++) {
    // there are only 7 intervals in the major scale,
    // so go back to the first one when you reach the end of
    // an octave:
    note = note + major[n % 7];
    scale[n + 1] = note;
  }

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
  if (mqttClient.parseMessage() > 0) {
    Serial.print("Got a message on topic: ");
    Serial.println(mqttClient.messageTopic());
    // read the message:
    while (mqttClient.available()) {
      // convert numeric string to an int:
      int message = mqttClient.parseInt();
      Serial.println(message);
      // if the message value is within the note count:
      if (message >= 0 && message < noteCount) {
        // play a note:
        midiCommand(0x90, scale[message], 0x7F);
        noteState[message] = millis();
      }
    }
  }
  // iterate over the noteState array. If any are currently
  // playing notes, and their interval has passed, stop the note:
  for (int n = 0; n < noteCount; n++) {
    if (millis() - noteState[n] > noteDuration && noteState[n] > 0) {
      // turn the note off:
      midiCommand(0x80, scale[n], 0);
      noteState[n] = 0;
    }
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
