# Arduino MQTT Clients
The [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) library makes it easy to send and receive MQTT messages using WiFi-enabled Arduino models such as the Nano 33 IoT, MKR1010, MKR1000, or other third-party devices with compatible WiFi libraries. This repository contains examples using this library. All of these will work with the basic MQTT client examples in the [JavaScript Clients]({{site.baseurl}}/#javascript-clients) section of the repository, if you match up the public brokers they are all using.

## Installing the library and Board Definitions

If you've never used one of the WiFi-capable Arduino boards, you'll need to install the board definition. When you plug the board in, the Arduino IDE should pop up a message asking if you want to install the board definition. If not, click the Tools Menu --> Boards... --> Board Manager, and search for the board you're using. When you find it, click Install. The Nano 33 IoT, the MKR 1010 and the Uno WiFi all use the WiFiNINA library, and the MKR1000 uses the WiFi101 library. These two libraries have identical APIs.

To install the [ArduinoMqttClient](https://www.arduino.cc/reference/en/libraries/arduinomqttclient/) library, click the Sketch Menu --> Include Library --> Manage Libraries, and search for ArduinoMqttClient. When you find it, click Install. Then do the same for the [WiFiNINA](https://www.arduino.cc/reference/en/libraries/wifinina/) library (if you're using a Nano 33 IoT or MKR 1010) or the [WiFi101](https://www.arduino.cc/reference/en/libraries/wifi101/) library (if you're using a MKR1000).


All of the Arduino examples in this repository require an extra file to be included to the sketch. Use command-shift-N on MacOS or or control-shift-N on Windows, then name the file `arduino_secrets.h` The following credentials should be included in that file:

````
#define SECRET_SSID ""  // add your network SSID here
#define SECRET_PASS ""  // add your netork password here
#define SECRET_MQTT_USER "public" // broker username for shiftr.io
#define SECRET_MQTT_PASS "public" // broker password for shiftr.io
````

Note: this assumes you're on a network with WPA2 encryption, which is what most people are using at home these days. If you're not, check the [WiFiNINA library documentation](https://www.arduino.cc/reference/en/libraries/wifinina/) for how to configure your sketches for other network types. 

## ArduinoMqttClient

[See the source code]({{site.codeurl}}/arduino-clients/ArduinoMqttClient/ArduinoMqttClient.ino) 

This is a basic  client example for the ArduinoMqttClient library. The global variables define the broker and credentials. In the setup, the Arduino connects to WiFi and sets the MQTT client ID and other pre-connect characteristics, including the `onMessage` handler. In the loop, it continually attempts to connect to WiFi if it's not connected using the custom `connectToNetwork()` function; attempts to connect to the broker if not connected using the custom `connectToBroker()` function; then polls for any incoming MQTT messages using the `onMqttMessage()` handler. Then, once every interval, it reads a sensor and publishes the value as an MQTT message on the topic.

The other Arduino examples in this repository follow more or less the same structure. 

## Sensor Clients

There are a couple sensor clients in this repository, to show how to read and publish sensors of differing values. 

### MqttClient_SensorTCS34725Sender

[See the source code]({{site.codeurl}}/arduino-clients/MqttClient_SensorTCS34725Sender/MqttClient_SensorTCS34725Sender.ino)

This example reads lux and color temperature levels from an AMS [TCS34725 light and color sensor](https://ams.com/en/tcs34725) using Adafruit's Adafruit_TCS34725 library. It also reads the MAC address of the WIFi radio and uses it as a unique ID to send in the MQTT message. It follows the same structure as the basic example described above. This example also shows how to send JSON strings with just the String object.

### MqttClient_SensorENS160Sender

[See the source code]({{site.codeurl}}/arduino-clients/MqttClient_SensorENS160Sender/MqttClient_SensorENS160Sender.ino)

This example reads a [Sciosense ENS160 AQI sensor](https://www.sciosense.com/products/environmental-sensors/digital-multi-gas-sensor/), which calculates CO2-equivalent (eCO2), Total Volatile Organic Compounds (TVOC) and, air quality index (AQI). Breakout boards from both Sparkfun and Adafruit were used in the testing and both work well. The example uses Sparkfun's SparkFun_ENS160 library. This example also shows how to send JSON strings with just the String object.

### MqttClient_SensorCombinedAQISender

[See the source code]({{site.codeurl}}/arduino-clients/MqttClient_SensorCombinedAQISender/MqttClient_SensorCombinedAQISender.ino)

This example reads temperature and humidity levels from an  [Sensiron SHTC3 Temperature and Relative Humidity (rH) sensor](https://sensirion.com/products/catalog/SHTC3/) and uses those values to provide temp. and rH compensation for a [Sciosense ENS160 AQI sensor](https://www.sciosense.com/products/environmental-sensors/digital-multi-gas-sensor/), as seen in the previous example. It uses Sparkfun's SparkFun_ENS160 and SparkFun_SHTC3 libraries. This example also shows how to send JSON strings with just the String object.

## MqttClientSubTopics

[See the source code]({{site.codeurl}}/arduino-clients/MqttClientSubTopics/MqttClientSubTopics.ino)

This example shows how to use MQTT to get and set the properties of a microcontroller's program. The microcontroller has two blinking LEDs and an analog sensor attached. You can change the behavior of the LEDs remotely by publishing to the subtopics `/brightness` and `/blinkInterval` and change the sensor update rate by publishing to `/sendInterval`.

## MqttClientNeoPixel

[See the source code]({{site.codeurl}}/arduino-clients/MqttClientNeoPixel/MqttClientNeoPixel.ino)

This example shows how to use MQTT subtopics to set the color of a strip of WorldSemi [WS2812 addressable LEDs (NeoPixels)](https://tigoe.github.io/LightProjects/addressable-leds). The microcontroller subscribes to a topic called `color` and looks for a comma-separated string of values, r, g, and b. When it gets these, it uses them to update the NeoPixel's colors.

## MqttClientHueControl

[See the source code]({{site.codeurl}}/arduino-clients/MqttClientHueControl/MqttClientHueControl.ino)

This example shows how to use MQTT  to set the brightness of a Philips Hue light through a Hue hub. It does so by making HTTP requests to the Hue hub. For more on controlling the Philips Hue, see [this repository](https://tigoe.github.io/hue-control/). It can work with the Eclipse PAHO and p5.js example called [EclipsePahoHueLightControl]({{site.codeurl}}/browser-clients/eclipse-pahojs/eclipse-pahojs/EclipsePahoHueLightControl)


## MIDI examples

These MIDI examples will work well with the [WebMIDI browser clients]({{site.baseurl}}/browser-clients/eclipse-pahojs/#web-midi-clients).

### MqttClientMIDIController

[See the source code]({{site.codeurl}}/arduino-clients/MqttClientMIDIController/MqttClientMIDIController.ino)

This sketch is an MQTT client that connects to a broker, subscribes to a topic, and  sends  messages on that topic. The messages are three-byte arrays that can be read as MIDI noteon and noteoff messages.

### MqttClientMIDIController

[See the source code]({{site.codeurl}}/arduino-clients/MqttClientMIDIPlayer/MqttClientMIDIPlayer.ino)

This sketch is an MQTT client that connects to a broker, subscribes to a topic, and  listens for messages on that topic. When it receives a three-byte message, it uses it to send out a MIDI note via MIDIUSB. It doesn't attempt to interpret  the MIDI message, it just sends it.
