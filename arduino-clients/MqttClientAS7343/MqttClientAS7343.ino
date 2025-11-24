/*
  This sketch demonstrates an MQTT client that connects to a broker, 
  and publishes messages to it from an AS7343 Spectral light sensor. 

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
  * http://librarymanager/All#SparkFun_AS7343

  the circuit:
  - AS7343 sensor attached to SDA and SCL of the Arduino
  
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 24 Nov 2025
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Uno WiFi
// #include <WiFi101.h>    // use this for MKR1000
// #include <WiFiS3.h>  // use this for Uno R4 WiFi
// #include <ESP8266WiFi.h>  // use this for ESP8266-based boards
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

#include <SparkFun_AS7343.h>     // include sensor library
SfeAS7343ArdI2C spectralSensor;  // make an instance of the library
uint16_t sensorData[18];         // Array to hold spectral data
String spectralReadings = "";    // String to send it

// initialize WiFi connection. If the broker is using
// encrypted mqtts, use SSL like so:
// WiFiSSLClient wifi;
// if the broker is not using mqtts, connect without SSL like so:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 1883;
char topic[] = "spectral_data";
String clientID = "as7343Client";

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

  // start the I2C library:
  Wire.begin();
  // Start sensor:
  if (!spectralSensor.begin()) {
    Serial.println("Sensor failed to begin. Please check your wiring!");
    while (1)
      ;
  }
  if (!spectralSensor.powerOn()) {
    Serial.println("Failed to power on the device.");
    while (1)
      ;
  }
  // set up for all 18 channels:
  if (!spectralSensor.setAutoSmux(AUTOSMUX_18_CHANNELS)) {
    Serial.println("Failed to set AutoSmux.");
    while (1)
      ;
  }

  // Enable spectral measurement:
  if (!spectralSensor.enableSpectralMeasurement()) {
    Serial.println("Failed to enable spectral measurement.");
    while (1)
      ;
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

  // once every interval, send a message:
  if (millis() - lastTimeSent > interval) {
    // get a sensor  reading:
    bool validReading = readSensor();
    // if the reading is not valid, skip the rest of the loop:
    if (!validReading) return;

    if (mqttClient.connected()) {
      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the body of the message:
      mqttClient.print(spectralReadings);
      // send the message:
      mqttClient.endMessage();
      // send a serial notification:
      Serial.print("published a message: ");
      Serial.println(spectralReadings);
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
  // print the result:
  Serial.println(result);
  delay(100);
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
  digitalWrite(LED_BUILTIN, HIGH);
}


bool readSensor() {
  // clear the sensor string:
  spectralReadings = "";
  if (spectralSensor.readSpectraDataFromSensor() == false) {
    // return that you didn't get a good reading:
    return false;
  }
  // read the sensor:
  int channelsRead = spectralSensor.getData(sensorData);

  // save the data for sending:
  for (int channel = 0; channel < channelsRead; channel++) {
    spectralReadings += String(sensorData[channel]);
    // add a comma for all but the last reading:
    if (channel < channelsRead - 1) spectralReadings += ",";
  }
  Serial.println(spectralReadings);
  // return that you got a good reading:
  return true;
}