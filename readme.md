# Arduino MQTT Client Examples

**Message Queueing Telemetry Transfer**, or **[MQTT](https://mqtt.org/)**, is a lightweight IP-based messaging protocol designed for communication between sensors, controllers, and other devices. It's designed to support equipment that may not always be online, like automated devices built with microcontrollers. MQTT server programs are called **brokers**. A broker keeps track of messages from clients, and allows any client to query the last message sent by another client. 

Messages are organized into **topics**. Typically, a topic represents a device, with each sub-topic representing its characteristics.  For example, a weather station might have the main topic "station" with subtopics "temperature", "humidity", "air quality", and so forth. The weather station itself would send messages to each of the subtopics, and a web client might subscribe to those topics to graph them onscreen over time. 

Clients  either publish new messages to topics, or subscribe to topics, and the broker notifies them when new messages arrive.  For this reason,  MQTT is known as a **Publish & Subscribe**, or **PubSub** system.

----

## MQTT Clients
The [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) library makes it easy to send and receive MQTT messages using WiFi-enabled Arduino models such as the Nano 33 IoT, MKR1010, MKR1000, or other third-party devices with compatible WiFi libraries. This repository contains examples using this library. Here's a [page to get you started](MqttClientSender/readme.md).

There are many other Arduino MQTT libraries. Joël Gähwiler's [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) is another good example. There are examples for many programming environments, like [node.js](https://github.com/mqttjs/MQTT.js/), [Processing](https://github.com/256dpi/processing-mqtt) (also by Joël Gähwiler) and more. 

For JavaScript clients, the [Eclipse PAHO library]() is an excellent library. It sends MQTT messages over webSockets. Many MQTT brokers support websocket connections. For example, here are the connection details for [test.mosquitto.org](https://test.mosquitto.org/) and [shiftr.io](https://docs.shiftr.io/interfaces/mqtt/), both mentioned below. There is a [p5.js](https://p5js.org/) example using Eclipse PAHO in this repository as well. Here's a [link introducing it](https://tigoe.github.io/mqtt-examples/p5js-mqtt-client/readme.md). You can see it in action at [this link](https://tigoe.github.io/mqtt-examples/p5js-mqtt-client/index.html).

## MQTT Brokers
There are a number of MQTT brokers you can use, the most popular is [mosquitto](http://mosquitto.org/). You can run mosquitto on your own computer or server, or you can use [test.mosquitto.org](https://test.mosquitto.org/) as a test broker. [Shiftr.io](https://shiftr.io/try) is another MQTT test broker, with a graphic interface so you can see a graph of clients and topics. These examples use shiftr.io as their test broker. 