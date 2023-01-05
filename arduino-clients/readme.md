
## Arduino MQTT Clients
The [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) library makes it easy to send and receive MQTT messages using WiFi-enabled Arduino models such as the Nano 33 IoT, MKR1010, MKR1000, or other third-party devices with compatible WiFi libraries. This repository contains examples using this library. 

## ArduinoMqttClient

[See the source code]({{site.codeurl}}/arduino-clients/ArduinoMqttClient/ArduinoMqttClient.ino) 

This is a basic  client example for the ArduinoMqttClient library. The global variables define the broker and credentials. In the setup, the Arduino connects to WiFi and sets the MQTT client ID and other pre-connect characteristics, including the `onMessage` handler. In the loop, it continually attempts to connect to WiFi if it's not connected using the custom `connectToNetwork()` function; attempts to connect to the broker if not connected using the custom `connectToBroker()` function; then polls for any incoming MQTT messages using the `onMqttMessage()` handler. Then, once every interval, it reads a sensor and publishes the value as an MQTT message on the topic.

All of the Arduino examples in this repository require an extra file to be included to the sketch. Use command-shift-N on MacOS or or control-shift-N on Windows, then name the file `arduino_secrets.h` The following credentials should be included in that file:

````
#define SECRET_SSID ""  // add your network SSID here
#define SECRET_PASS ""  // add your netork password here
#define SECRET_MQTT_USER "public" // broker username for shiftr.io
#define SECRET_MQTT_PASS "public" // broker password for shiftr.io
````

The other Arduino examples in this repository follow more or less the same structure. 

## MqttLightSensor

[See the source code]({{site.codeurl}}/arduino-clients/MqttLightSensor/MqttLightSensor.ino)

This example reads lux and color temperature levels from an AMS [TCS34725 light and color sensor](https://ams.com/en/tcs34725) using Adafruit's Adafruit_TCS34725 library. It also reads the MAC address of the WIFi radio and uses it as a unique ID to send in the MQTT message. It follows the same structure as the basic example described above. This example also shows how to send JSON strings with just the String object.

## MqttClient_SensorENS160Sender

[See the source code]({{site.codeurl}}/arduino-clients/MqttClient_SensorENS160Sender/MqttClient_SensorENS160Sender.ino)

This example reads a [Sciosense ENS160 AQI sensor](https://www.sciosense.com/products/environmental-sensors/digital-multi-gas-sensor/), which calculates CO2-equivalent (eCO2), Total Volatile Organic Compounds (TVOC) and, air quality index (AQI). Breakout boards from both Sparkfun and Adafruit were used in the testing and both work well. The example uses Sparkfun's SparkFun_ENS160 library. This example also shows how to send JSON strings with just the String object.

 It follows the same structure as the basic example described above. 

## MqttClient_SensorCombinedAQISender

[See the source code]({{site.codeurl}}/arduino-clients/MqttClient_SensorCombinedAQISender/MqttClient_SensorCombinedAQISender.ino)

This example reads temperature and humidity levels from an  [Sensiron SHTC3 Temperature and Relative Humidity (rH) sensor](https://sensirion.com/products/catalog/SHTC3/) and uses those values to provide temp. and rH compensation for a [Sciosense ENS160 AQI sensor](https://www.sciosense.com/products/environmental-sensors/digital-multi-gas-sensor/), which calculates CO2-equivalent (eCO2), Total Volatile Organic Compounds (TVOC) and, air quality index (AQI). Breakout boards from both Sparkfun and Adafruit were used in the testing and both work well. The example uses Sparkfun's SparkFun_ENS160 and SparkFun_SHTC3 libraries. This example also shows how to send JSON strings with just the String object.

 It follows the same structure as the basic example described above. 