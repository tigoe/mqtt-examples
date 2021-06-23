# MQTT Client Examples

**Message Queueing Telemetry Transfer**, or **[MQTT](https://mqtt.org/)**, is a lightweight IP-based messaging protocol designed for communication between sensors, controllers, and other devices. It's designed to support equipment that may not always be online, like automated devices built with microcontrollers. MQTT server programs are called **brokers**. A broker keeps track of messages from clients, and allows any client to query the last message sent by another client. 

Messages are organized into **topics**. Typically, a topic represents a device, with each sub-topic representing its characteristics.  For example, a weather station might have the main topic "station" with subtopics "temperature", "humidity", "air quality", and so forth. The weather station itself would send messages to each of the subtopics, and a web client might subscribe to those topics to graph them onscreen over time. 

Clients  either publish new messages to topics, or subscribe to topics, and the broker notifies them when new messages arrive.  For this reason,  MQTT is known as a **Publish & Subscribe**, or **PubSub** system. 

What's nice about MQTT, as opposed to HTTP, for example, is that it's a simple protocol, you can send anything you want in a message with whatever formatting you want, and when you subscribe to a topic, you get updates whenever a new message from a remote client arrives. So it's great for situations where two or more devices need to communicate in both directions in near real-time. It offers the convenience of web sockets, but without having to maintain a connection to the server, as all communication is message-based, not session-based.

Because MQTT doesn't care what you put in each message, you can format your messages to match whatever end use you need. You text-formatted messages like comma-separate values (CSV) or JSON strings inside an MQTT message, for example, or binary messages like MIDI commands or OSC messages. The broker doesn't care what's in the message. This means that your client's job is just to send and receive messages between the broker and your end application, whatever it may be.

For a more detailed explanation, see [this explanation from IBM](https://developer.ibm.com/technologies/messaging/articles/iot-mqtt-why-good-for-iot), who developed the protocol initially. 

Here is a [comparison between WebSockets and MQTT](mqtt-vs-websockets.md). 

----

## Arduino MQTT Clients
The [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) library makes it easy to send and receive MQTT messages using WiFi-enabled Arduino models such as the Nano 33 IoT, MKR1010, MKR1000, or other third-party devices with compatible WiFi libraries. This repository contains examples using this library. Here's a [page to get you started](MqttClientSender/readme.md). 

There are many other Arduino MQTT libraries. Joël Gähwiler's [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) is another good example. There are examples for many programming environments, like [node.js](https://github.com/mqttjs/MQTT.js/), [Processing](https://github.com/256dpi/processing-mqtt) (also by Joël Gähwiler) and more. 

## JavaScript Clients

For JavaScript clients, the [Eclipse PAHO library](https://www.eclipse.org/paho/index.php?page=clients/js/index.php) is an excellent library. It sends MQTT messages over webSockets. Many MQTT brokers support websocket connections. For example, here are the connection details for [test.mosquitto.org](https://test.mosquitto.org/) and [shiftr.io](https://docs.shiftr.io/interfaces/mqtt/), both mentioned below. There is a [p5.js](https://p5js.org/) example using Eclipse PAHO in this repository as well. Here's a [link that introduces it](p5js-mqtt-client/readme.md). You can see it in action at [this link](p5js-mqtt-client/public/index.html).

A simpler p5.js client that sends data on a mouse press is available at [this link](p5js-mqtt-client/mousePressed-client).

An example combining the p5.js MQTT client and the [p5.js serialport library](https://github.com/p5-serial/p5.serialport/blob/main/lib/p5.serialport.js) is available at [this link](p5js-mqtt-client/p5Serial-client). You'll need the [p5.serialcontrol app](https://github.com/p5-serial/p5.serialcontrol/releases) to connect to your serial ports. 

## Air Quality Index Clients

There is also a p5.js example in this directory that [receives MQTT messages](p5js-mqtt-client/AQISensorReceiver/index.html) from an [SGP30 air quality sensor example](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientAQISender) for the Arduino Nano 33 IoT. Here is a [link to the code for the p5.js client](https://github.com/tigoe/mqtt-examples/tree/main/p5js-mqtt-client/AQISensorReceiver). This pair of examples is designed to show how to send and receive JSON.

## Web MIDI Clients

There are also examples here that combines [p5.js](https://p5js.org) with the [Web MIDI API](https://www.w3.org/TR/webmidi/), sending MIDI messages over an MQTT broker. It's called [mqtt-midi-client](p5js-mqtt-client/mqtt-midi-client). If you prefer a version without p5.js, and with keyboard input so that it can act as a MIDI controller, see [mqtt-midi-controller](mqtt-midi-controller).
 
This [Arduino MQTT-to-MIDI Client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIPlayer) can receive MIDI messages from the same broker and send MIDI to your operating system or MIDI system.  This [Arduino MIDI-to-MQTT client](https://github.com/tigoe/mqtt-examples/tree/main/MqttClientMIDIController) can send noteon and noteoff messages via MQTT at the push of a button.

## Hue Control with QR Code Client

There is a [p5.js-based client with QR code](MqttWithQRCode) as well.  It generates its URL in a QR code, to make it easy to pass from one phone to another. It works with [this Philips Hue client](MqttLightControl), which sends HTTP messages to a local Philips Hue hub to control Hue lights. 

## Node.js Clients

There are two clients built in node.js in this repository, [MqttNodeClient](https://github.com/tigoe/mqtt-examples/tree/main/MqttNodeClient) which is a basic example of how to make a client with the [node.js MQTT library](https://www.npmjs.com/package/mqtt). It sends a reading every few seconds, and subscribes to a topic called `lights`. It will work with the light control examples above. The other, [MqttNodeClientSerial](https://github.com/tigoe/mqtt-examples/tree/main/MqttNodeClientSerial) is similar, but it uses the node serialport library from [serialport.io](https://serialport.io/docs) to connect MQTT to serial in and out.

## MQTT Brokers

There are a number of MQTT brokers you can use, the most popular is [mosquitto](http://mosquitto.org/). You can run mosquitto on your own computer or server, or you can use [test.mosquitto.org](https://test.mosquitto.org/) as a test broker. The instructions and port numbers for the various ways of reaching it are listed on [test.mosquitto.org](https://test.mosquitto.org). 

[Shiftr.io](https://next.shiftr.io/try) is another MQTT test broker, with a graphic interface so you can see a graph of clients and topics. Shiftr.io has a [desktop broker](https://next.shiftr.io/desktop) that you can download and use for local testing as well. These examples use shiftr.io as their test broker, though they have been tested on mosquitto.org as well. Note that Shiftr.io's examples use a different Arduino client library and a different JavaScript library than this site does. The code here is still compatible with that briker, however. 

[mqtt.eclipse.org](https://mqtt.eclipse.org/) is another MQTT test broker with a fairly bare-bones set of documentation and no visualizer. 

## MQTT Explorer

[MQTT Explorer](http://mqtt-explorer.com/) is a desktop client that can bve useful for diagnosing issues with client-to-broker communication. It's available on all major operating systems. 

## MQTT, Web Sockets, and Encryption

You can make an MQTT request directly, or you can do it encrypted. You can also make MQTT requests over websockets. Each transport method is typically done on a different port number. For example, test.mosquitto.org lists the following ports:

* 1883 : MQTT, unencrypted
* 8883 : MQTT, encrypted
* 8884 : MQTT, encrypted, client certificate required
* 8080 : MQTT over WebSockets, unencrypted
* 8081 : MQTT over WebSockets, encrypted

Different client APIs support different approaches. For example, the ArduinoMqttClient supports both unencrypted and encrypted MQTT or MQTTS connections just by changing the port number and the WiFiClient to a WiFiSSLClient. The Eclipse Paho JS client send MQTT messages over web sockets, encrypted or unencrypted. Other MQTT client frameworks use one or more of these four methods. 