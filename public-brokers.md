[EQMX Public Broker](https://www.emqx.com/en/mqtt/public-mqtt5-broker)

Works with eclipse Paho, ArduinoMqttClient, and mqtt.js

* Broker address: broker.emqx.io
* MQTT TCP Port: 1883
* MQTT SSL/TLS Port:8883
* WebSocket Port: 8083
* WebSocket SSL/TLS Port: 8084
* Certificate Authority: broker.emqx.io-ca.crt


| Broker| | Arduino | mqtt.js |  eclipse.PAHO.js |
| --- |--- | --- | --- | --- |
| EQMX |   |   |   |
|  | Initializer |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | address| broker.emqx.io | same | same |
| | port | 8883 | 8084| 8084|
|     |   |   |   |
| | credentials | none | none | none |
| shiftr.io cloud |   |   |   |
|  | Initializer |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | address| public.cloud.shiftr.io | same | same |
| | port | 8883 | not specified| 443|
| | credentials | username : public, password: public | same| same|
| shiftr.io desktop |   |   |   |
|  | Initializer |WiFiClient | ws:// | none | 
 | | address| your computer's IP address | same| same|
| | port | 1883 | 1884 | 1884|
| | credentials | none | none| none|
| test.mosquitto.org |   |   |   |
|  | Initializer |WiFiSSLClient | wss:// | useSSL: true in client.connect() | 
 | | address| test.mosquitto.org | same | same |
| | port | 8886 | 8081| 8081|
| | credentials | none | none | none |



There are a number of MQTT brokers you can use, the most popular is [mosquitto](http://mosquitto.org/). You can run mosquitto on your own computer or server, or you can use [test.mosquitto.org](https://test.mosquitto.org/) as a test broker. The instructions and port numbers for the various ways of reaching it are listed on [test.mosquitto.org](https://test.mosquitto.org). 

[Shiftr.io](https://next.shiftr.io/try) is another MQTT test broker, with a graphic interface so you can see a graph of clients and topics. Shiftr.io has a [desktop broker](https://next.shiftr.io/desktop) that you can download and use for local testing as well. These examples use shiftr.io as their test broker, though they have been tested on mosquitto.org as well. Note that Shiftr.io's examples use a different Arduino client library and a different JavaScript library than this site does. The code here is still compatible with that broker, however. 


