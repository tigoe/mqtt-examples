/*
  This sketch demonstrates an MQTT client that connects to a broker, 
  and publishes messages to it from an ENS160 AQI sensor and
  an SHTC3 Temperature and rH sensor. Uses the temp/humidity
  sensor to provide compensation for the AQI sensor, Shows how to
  send JSON strings with just the String object. 

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
  * http://librarymanager/All#SparkFun_ENS160
  * http://librarymanager/All#SparkFun_SHTC3

  the circuit:
  - SHTC3 sensor and ENS160 sensor attached to SDA and SCL of the Arduino
  
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 11 June 2020
  updated 25 Feb 2023
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include "SparkFun_ENS160.h"
#include "SparkFun_SHTC3.h"
#include "arduino_secrets.h"

// initialize WiFi connection as SSL:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "AQISensor";
String clientID = "CombinedSensorClient-";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 10 * 1000;

// instance of the ENS160 sensor library:
SparkFun_ENS160 AQISensor;
SHTC3 temp_rHSensor;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);

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

  // attempt to start the sensor:
  Wire.begin();
  if (!AQISensor.begin(0x53)) {
    Serial.println("Sensor is not responding. Check wiring.");
    // stop the program here if the sensor didn't respond:
    while (true)
      ;
  }

  if (temp_rHSensor.begin() != SHTC3_Status_Nominal) {
    Serial.println("Temp & Humidity Sensor is not responding. Check wiring.");
    while (true)
      ;
  }

  if (!AQISensor.setOperatingMode(SFE_ENS160_RESET)) {
    while (true)
      ;
  }
  // Device needs to be set to idle to apply any settings.
  AQISensor.setOperatingMode(SFE_ENS160_IDLE);
  // get an update from the temp/rH sensor:
  temp_rHSensor.update();
  // wait until sensor returns a value:
  while (temp_rHSensor.lastStatus != SHTC3_Status_Nominal) {
    Serial.println("Humidity and Temperature Sensor is not ready.");
  }
  // read temp and rH:
  float rh = temp_rHSensor.toPercent();
  float tempC = temp_rHSensor.toDegC();

  // Give values to AQI sensor:
  AQISensor.setTempCompensationCelsius(tempC);
  AQISensor.setRHCompensationFloat(rh);
  // delay to make sure updates are written:
  delay(500);
  // Set AQI to standard operation:
  AQISensor.setOperatingMode(SFE_ENS160_STANDARD);
}

void loop() {
  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("attempting to connect to broker");
    connectToBroker();
  }

  // once every interval, send a message:
  if (millis() - lastTimeSent > interval) {
    // if the AQI sensor is ready:
    // ask for a reading:
    temp_rHSensor.update();
    // read the results:
    float tempC = AQISensor.getTempCelsius();
    float rh = AQISensor.getRH();

    // the sensor can have four possible states:
    // 0 - Operating ok: Standard Opepration
    // 1 - Warm-up: occurs for 3 minutes after power-on.
    // 2 - Initial Start-up: Occurs for the first hour of operation.
    //												and only once in sensor's lifetime.
    // 3 - No Valid Output
    int ensStatus = AQISensor.getFlags();
    // if the sensor's ready to read, read it:
    if (AQISensor.checkDataStatus()) {
      int aqi = AQISensor.getAQI();
      int tvoc = AQISensor.getTVOC();
      int eCO2 = AQISensor.getECO2();

      // put them into a JSON String:
      String payload = "{\"sensor\": \"SGP30 and ENS160\",";
      payload += "\"aqi\": AQI, \"tvoc\": TVOC, \"eCO2\": ECO2,";
      payload += "\"status\": STATUS, \"temp\": TEMP, \"humidity\": RH}";
      // replace the value substrings with actual values:
      payload.replace("AQI", String(aqi));
      payload.replace("TVOC", String(tvoc));
      payload.replace("ECO2", String(eCO2));
      payload.replace("STATUS", String(ensStatus));
      payload.replace("TEMP", String(tempC));
      payload.replace("RH", String(rh));

      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the payload of the message:
      mqttClient.println(payload);
      // send the message:
      mqttClient.endMessage();
      // send a serial notification:
      Serial.print("published: ");
      Serial.println(payload);
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
  // once you're connected, you
  // return that you're connected:
  return true;
}