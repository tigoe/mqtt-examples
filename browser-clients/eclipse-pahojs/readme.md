## Eclipse PAHO JavaScript Library

The [Eclipse PAHO library](https://www.eclipse.org/paho/index.php?page=clients/js/index.php) is an excellent library. It sends MQTT messages over webSockets. Many MQTT brokers support websocket connections. For example, here are the connection details for [test.mosquitto.org](https://test.mosquitto.org/) and [shiftr.io](https://docs.shiftr.io/interfaces/mqtt/), both mentioned below. There is a [p5.js](https://p5js.org/) example using Eclipse PAHO in this repository as well. Here's a [link that introduces it](p5js-mqtt-client/readme.md). You can see it in action at [this link](p5js-mqtt-client/public/index.html).

A simpler p5.js client that sends data on a mouse press is available at [this link](p5js-mqtt-client/mousePressed-client).

An example combining the p5.js MQTT client and the [p5.js serialport library](https://github.com/p5-serial/p5.serialport/blob/main/lib/p5.serialport.js) is available at [this link](p5js-mqtt-client/p5Serial-client). You'll need the [p5.serialcontrol app](https://github.com/p5-serial/p5.serialcontrol/releases) to connect to your serial ports. 

## Air Quality Index Clients

There is also a p5.js example in this directory that [receives MQTT messages](p5js-mqtt-client/AQISensorReceiver/index.html) from an [SGP30 air quality sensor example](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientAQISender) for the Arduino Nano 33 IoT. Here is a [link to the code for the p5.js client](https://github.com/tigoe/mqtt-examples/tree/main/p5js-mqtt-client/AQISensorReceiver). This pair of examples is designed to show how to send and receive JSON.

## Web MIDI Clients

There are also examples here that combines [p5.js](https://p5js.org) with the [Web MIDI API](https://www.w3.org/TR/webmidi/), sending MIDI messages over an MQTT broker. It's called [mqtt-midi-client](p5js-mqtt-client/mqtt-midi-client). If you prefer a version without p5.js, and with keyboard input so that it can act as a MIDI controller, see [mqtt-midi-controller](mqtt-midi-controller).
 
This [Arduino MQTT-to-MIDI Client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIPlayer) can receive MIDI messages from the same broker and send MIDI to your operating system or MIDI system.  This [Arduino MIDI-to-MQTT client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIController) can send noteon and noteoff messages via MQTT at the push of a button.

## Hue Control with QR Code Client

There is a [p5.js-based client with QR code](MqttWithQRCode) as well.  It generates its URL in a QR code, to make it easy to pass from one phone to another. It works with [this Philips Hue client](MqttLightControl), which sends HTTP messages to a local Philips Hue hub to control Hue lights. 
