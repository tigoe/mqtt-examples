# MQTT Examples

**Message Queueing Telemetry Transfer**, or **[MQTT](https://mqtt.org/)**, is a lightweight IP-based messaging protocol designed for communication between sensors, controllers, and other devices. It's designed to support equipment that may not always be online, like automated devices built with microcontrollers. MQTT server programs are called **brokers**. A broker keeps track of messages from clients, and allows any client to query the last message sent by another client. 

Messages are organized into **topics**. Typically, a topic represents a device, with each sub-topic representing its characteristics.  For example, a weather station might have the main topic "station" with subtopics "temperature", "humidity", "air quality", and so forth. The weather station itself would send messages to each of the subtopics, and a web client might subscribe to those topics to graph them onscreen over time. 

Clients  either publish new messages to topics, or subscribe to topics, and the broker notifies them when new messages arrive.  For this reason,  MQTT is known as a **Publish & Subscribe**, or **PubSub** system. 

What's nice about MQTT, as opposed to HTTP, for example, is that it's a simple protocol, you can send anything you want in a message with whatever formatting you want, and when you subscribe to a topic, you get updates whenever a new message from a remote client arrives. So it's great for situations where two or more devices need to communicate in both directions in near real-time. It offers the convenience of web sockets, but without having to maintain a connection to the server, as all communication is message-based, not session-based.

Because MQTT doesn't care what you put in each message, you can format your messages to match whatever end use you need. You text-formatted messages like comma-separate values (CSV) or JSON strings inside an MQTT message, for example, or binary messages like MIDI commands or OSC messages. The broker doesn't care what's in the message. This means that your client's job is just to send and receive messages between the broker and your end application, whatever it may be.

For a more detailed explanation, see [this explanation from IBM](https://developer.ibm.com/articles/iot-mqtt-why-good-for-iot/), who developed the protocol initially. 

Here is a [comparison between WebSockets and MQTT](mqtt-vs-websockets.md). 

## Arduino Client Libraries

[ArduinoMqttClient examples in this repository](arduino-clients)

There are multiple MQTT client libraries for Arduino. The examples here all use the [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) library. This library works with all the WiFi-enabled Arduino models, and many third-party models as well. 

There are many other Arduino MQTT libraries. Joël Gähwiler's [arduino-mqtt](https://github.com/256dpi/arduino-mqtt) is another good example. 


## JavaScript Clients

For JavaScript clients, there are multiple libraries. The examples here use the [Eclipse PAHO library](https://www.eclipse.org/paho/index.php?page=clients/js/index.php) and the [mqtt.js library](https://github.com/mqttjs/MQTT.js#readme). They both have similar functionality, but the mqtt.js library has a bit simpler syntax, and it can be used both in the browser and in node.js scripts. 
* [Eclipse PAHO browser examples](browser-clients/eclipse-pahojs/)
* [mqtt.js browser examples](browser-clients/mqttjs/)

## Node.js Clients

There are a few clients built in node.js in this repository, using the same mqtt.js library used in some of the browser clients above
* [node.js clients](node-clients)

## Processing Library

There is an MQTT library for [Processing](https://github.com/256dpi/processing-mqtt) (also by Joël Gähwiler). 

## Desktop and Mobile Client Apps

[MQTT Explorer](http://mqtt-explorer.com/) is a desktop client that can be useful for diagnosing issues with client-to-broker communication. It's available on all major operating systems. 

[MQTTX](https://mqttx.app/) is a desktop client for Windows, MacOS, Linux, and Ubuntu, from EMQX. 

[MQTTTool](https://apps.apple.com/us/app/mqttool/id1085976398) is a mobile client for iOS. 

[MyMQTT](https://play.google.com/store/apps/details?id=at.tripwire.mqtt.client&hl=en_US&gl=US&pli=1) is a good Android MQTT client app. 

There are multiple other desktop and command line client apps. 

## MQTT Brokers

There are a number of MQTT brokers you can use. The most popular is [mosquitto](http://mosquitto.org/). You can run mosquitto on your own computer or server, or you can use [test.mosquitto.org](https://test.mosquitto.org/) as a test broker. 

[Shiftr.io](https://shiftr.io/try) is another MQTT test broker, with a graphic interface so you can see a graph of clients and topics. Shiftr.io has a [desktop broker](https://shiftr.io/desktop) that you can download and use for local testing as well. These examples use shiftr.io as their test broker, though they have been tested on mosquitto.org as well. Note that Shiftr.io's examples use a different Arduino client library and a different JavaScript library than this site does. The code here is still compatible with that briker, however. 

## MQTT, Web Sockets, and Encryption

You can make an MQTT request directly, or you can do it encrypted. You can also make MQTT requests over websockets. Each transport method is typically done on a different port number. 

### test.mosquitto.org Ports

[test.mosquitto.org](https://test.mosquitto.org) lists the following ports:

* 1883 MQTT, unencrypted, unauthenticated
* 1884 MQTT, unencrypted, authenticated
* 8883 MQTT, encrypted, unauthenticated
* 8884 MQTT, encrypted, client certificate required
* 8885 MQTT, encrypted, authenticated
* 8886 MQTT, encrypted, unauthenticated
* 8887 MQTT, encrypted, server certificate deliberately expired
* 8080 MQTT over WebSockets, unencrypted, unauthenticated
* 8081 MQTT over WebSockets, encrypted, unauthenticated
* 8090 MQTT over WebSockets, unencrypted, authenticated
* 8091 MQTT over WebSockets, encrypted, authenticated

### Shiftr.io Cloud Ports

[Shiftr.io](https://www.shiftr.io/docs/broker/mqtt-interface/) lists the following ports:
* 1883 MQTT, unencrypted port 1883
* 8883: MQTT, encrypted
* 443: secure WebSocket (WSS/HTTPS)

### Shiftr.io Desktop Ports

* 1883 MQTT, unencrypted port 1883
* 1884:  WebSocket (WS/HTTP)

### EMQX Public Broker Ports

[EMQX](https://www.emqx.com/en/mqtt/public-mqtt5-broker) lists the following ports:

* MQTT TCP Port: 1883
* WebSocket Port: 8083
* MQTT SSL/TLS Port: 8883
* WebSocket SSL/TLS Port: 8084

Different client APIs support different approaches. For example, the ArduinoMqttClient supports both unencrypted and encrypted MQTT or MQTTS connections just by changing the port number and the WiFiClient to a WiFiSSLClient. The browser clients generally send MQTT messages over web sockets, encrypted or unencrypted. Other MQTT client frameworks use one or more of these four methods. 

## Broker Client Settings 

Table 1 below shows the settings for each of three client tools: the ArduinoMqttClient library: mqtt.js in a browser; and eclipse.paho.js in a browser; and four different brokers: EMQX; shiftr.io cloud; shiftr.io desktop client; and test.mosquitto.org. 

| Broker| | ArduinoMqttClient | mqtt.js |  eclipse.paho.js |
| --- |--- | --- | --- | --- |
| [EMQX](https://www.emqx.com/en/mqtt/public-mqtt5-broker) |   |   |   |
|  | Initializer: |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | Address:| broker.emqx.io | same | same |
| | Port: | 8883 | 8084| 8084|
| | Credentials: | none | none | none |
| [shiftr.io cloud](https://www.shiftr.io/docs/broker) |   |   |   |
|  | Initializer: |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | Address:| public.cloud.shiftr.io | same | same |
| | Port: | 8883 | not specified| 443|
| | Credentials: | username : public, password: public | same| same|
| [shiftr.io desktop](https://www.shiftr.io/docs/broker) |   |   |   |
|  | Initializer: |WiFiClient | ws:// | none | 
 | | Address:| your computer's IP address | same| same|
| | Port: | 1883 | 1884 | 1884|
| | Credentials: | none | none| none|
| [test.mosquitto.org](https://test.mosquitto.org/) |   |   |   |
|  | Initializer: |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | Address:| test.mosquitto.org | same | same |
| | Port: | 8886 | 8081| 8081|
| | Credentials: | none | none | none |