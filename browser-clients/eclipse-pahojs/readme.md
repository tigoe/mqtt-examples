# Eclipse PAHO JavaScript Library

* [Eclipse PAHO home](https://www.eclipse.org/paho/index.php?page=clients/js/index.php)
* [PAHO JS documentation](https://www.eclipse.org/paho/files/jsdoc/index.html)

There are several examples for Eclipse PAHO in [this directory]({{site.codeurl}}/browser-clients/eclipse-pahojs/). 

## EclipsePahoClientSimple
* [See the example running](EclipsePahoClientSimple)
* [See the source code]({{site.codeurl}}/browser-clients/eclipse-pahojs/EclipsePahoClientSimple)

This is a bare minimum client example for Eclipse PAHO JS. On document load, the script for this page gets two divs from the HTML document for local and remote messages.Then it attempts to connect to the broker. Once it does, it sends the local time if it's connected every two seconds. The publish button allows you to turn on and off publishing status, in case you're testing with a second client that's sending to the same topic.

The [ArduinoMqttClient example]({{site.codeurl}}/arduino-clients/ArduinoMqttClient) uses the same topic and sends the same range of numeric values if you want to test against another client.  The [MqttJsClientSimple](../mqttjs/MqttJsClientSimple/) does the same. 

## EclipsePahoP5Client
* [See the example running](EclipsePahoP5Client)
* [See the source code]({{site.codeurl}}/browser-clients/eclipse-pahojs/EclipsePahoP5Client)

This example combines Eclipse PAHO and [p5.js](https://p5js.org/). It sends a value between 1 and 15 to a topic called "notes". The MQTT functionality is similar to the example above. 

## mousePressed-client
* [See the example running](mousePressed-client)
* [See the source code]({{site.codeurl}}/browser-clients/eclipse-pahojs/mousePressed-client)
A simpler Eclipse PAHO and p5.js client that sends the mouseX and mouseY on a mouse press. The topic for this one is "monkey".

## p5-webSerial-client
* [See the example running](p5-webSerial-client)
* [See the source code]({{site.codeurl}}/browser-clients/eclipse-pahojs/p5-webSerial-client)

This example combines the p5.js MQTT client and the [p5.js webserial library](https://github.com/yoonbuck/p5.WebSerial). It takes any incoming serial messages and sends them out as MQTT messages. An accompanying Arduino sketch, [ArduinoJoystick](p5-webSerial-client/ArduinoJoystick/) sends the values from a Arduino connected via asynchronous serial port. For more on p5.WebSerial, see [these exercises](https://itp.nyu.edu/physcomp/labs/#p5js_webserial_library).

## Sensor Reader Client with JSON
* [See the example running](EclipsePahoSensorReceiverJSON)
* [See the source code]({{site.codeurl}}/browser-clients/eclipse-pahojs/EclipsePahoSensorReceiverJSON)

The example `EclipsePahoSensorReceiverJSON` subscribes to a topic and listens for JSON messages. It then parses them and displays them. It doesn't care what the data is, as long as it's in JSON format. You can change topics just by entering a new topic name in the topic field.

This [JSON validator](https://jsonlint.com/) may be useful for when you start reformatting strings and make mistakes. 

This example will work with the following Arduino examples:
* [MqttClient_SensorENS160Sender]({{site.codeurl}}/arduino-clients/MqttClient_SensorENS160Sender)- an Air Quality sensor, the [Sciosense ENS160](https://www.sciosense.com/products/environmental-sensors/digital-multi-gas-sensor/)
* [MqttClient_SensorCombinedAQISender]({{site.codeurl}}/arduino-clients/MqttClient_SensorCombinedAQISender)- an Air Quality sensor, the ENS160, combined with a temperature and relative humidity sensor, the [Sensiron SHTC3](https://sensirion.com/products/catalog/SHTC3/), for compensation. 

## Web MIDI Clients

There are also examples here that combines [p5.js](https://p5js.org) with the [Web MIDI API](https://www.w3.org/TR/webmidi/), sending MIDI messages over an MQTT broker. It's called [mqtt-midi-client](p5js-mqtt-client/mqtt-midi-client). If you prefer a version without p5.js, and with keyboard input so that it can act as a MIDI controller, see [mqtt-midi-controller](mqtt-midi-controller).
 
This [Arduino MQTT-to-MIDI Client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIPlayer) can receive MIDI messages from the same broker and send MIDI to your operating system or MIDI system.  This [Arduino MIDI-to-MQTT client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIController) can send noteon and noteoff messages via MQTT at the push of a button.

## Hue Control with QR Code Client

There is a [p5.js-based client with QR code](MqttWithQRCode) as well.  It generates its URL in a QR code, to make it easy to pass from one phone to another. It works with [this Philips Hue client](MqttLightControl), which sends HTTP messages to a local Philips Hue hub to control Hue lights. 
