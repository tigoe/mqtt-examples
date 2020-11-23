/*
  MQTT Client with an AQI sensor

  This sketch demonstrates an MQTT client that connects to a broker, subsrcibes to a topic,
  and  sends messages to it from an SGP30 AQI sensor. 
  
  This sketch uses https://next.shiftr.io/try as the MQTT broker.

  the circuit:
  - SGP30 sensor attached to SDA and SCL of the Arduino
  
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 26 Sep 2020
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
// Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include "SparkFun_SGP30_Arduino_Library.h"
#include <Wire.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// instantiate the sensor library:
SGP30 mySensor;
// timestamp for the sensor reading and send:
long lastSendTime = 0;

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 1883;
char topic[] = "AQISensor";
char clientID[] = "AQISensorClient";

// intensity of LED:
int intensity = 0;

void setup() {
  // initialize serial:
  Serial.begin(9600);
 
  // initialize I/O pins: 
  pinMode(LED_BUILTIN, OUTPUT);

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

  Wire.begin();
  //Initialize sensor
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1);
  }
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  mySensor.initAirQuality();

  // try to connect to the MQTT broker once you're connected to WiFi:
  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
}

void loop() {
  // turn on built-in LED when you are connected to WiFi:
  digitalWrite(LED_BUILTIN, WiFi.status());
  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
    connectToBroker();
  }

  // reasd the air quality once every 5 seconds:
  if (millis() - lastSendTime > 5000) {
    mySensor.measureAirQuality();

    // make the sensor readings into a JSON string:
    String dataString = "{";
    dataString += "\"CO2\":";
    dataString += String(mySensor.CO2);
    dataString += ", \"TVOC\": ";
    dataString += String(mySensor.TVOC);
    dataString += "}";
    Serial.println(dataString);

    // start the MQTT send:
    mqttClient.beginMessage(topic);
    // add the value:
    mqttClient.print(dataString);
    // send the message:
    mqttClient.endMessage();
    // timestamp for next send:
    lastSendTime = millis();
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
