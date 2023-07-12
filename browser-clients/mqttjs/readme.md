# mqtt.js Browser Client Examples

These examples are based on the [mqtt.js](https://github.com/mqttjs/MQTT.js) client library. 

## MqttJsClientSimple
* [See the example running](MqttJsClientSimple)
* [See the source code]({{site.codeurl}}/browser-clients/mqttjs/MqttJsClientSimple)

This is a bare minimum client example for mqtt.js. On document load, the script for this page gets two divs from the HTML document for local and remote messages.Then it attempts to connect to the broker. Once it does, it sends the local time if it's connected every two seconds. The publish button allows you to turn on and off publishing status, in case you're testing with a second client that's sending to the same topic.

The [ArduinoMqttClient example]({{site.codeurl}}/arduino-clients/ArduinoMqttClient) uses the same topic and sends the same range of numeric values if you want to test against another client.  