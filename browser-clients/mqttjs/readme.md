# mqtt.js Browser Client Examples

These examples are based on the [mqtt.js](https://github.com/mqttjs/MQTT.js) client library. 

## mqttjs-client-simple
* [See the example running](mqttjs-client-simple)
* [See the source code]({{site.codeurl}}/browser-clients/mqttjs/mqttjs-client-simple)

This is a bare minimum client example for mqtt.js. On document load, the script for this page gets two divs from the HTML document for local and remote messages.Then it attempts to connect to the broker. Once it does, it sends the local time if it's connected every two seconds. The publish button allows you to turn on and off publishing status, in case you're testing with a second client that's sending to the same topic.

The [ArduinoMqttClient example]({{site.codeurl}}/arduino-clients/ArduinoMqttClient) uses the same topic and sends the same range of numeric values if you want to test against another client.  

## mqttjs-p5js-mousepressed-client
* [See the example running](mqttjs-p5js-mousepressed-client)
* [See the source code]({{site.codeurl}}/browser-clients/mqttjs/mqttjs-p5js-mousepressed-client)

This example uses [p5.js](https://p5js.org/) and the [mqtt.js client library](https://www.npmjs.com/package/mqtt)  to create an MQTT client that sends and receives MQTT messages. The  client is set up for use on the [shiftr.io](https://www.shiftr.io/try/) test MQTT broker, but other options listed will work.

## mqtt-midi-controller
* [See the example running](mqtt-midi-controller)
* [See the source code]({{site.codeurl}}/browser-clients/mqttjs/mqtt-midi-controller)

This example uses the [mqtt.js library](https://www.npmjs.com/package/mqtt) and the [Web MIDI API](https://www.w3.org/TR/webmidi/) to create an MQTT client that sends and receives MQTT messages that are MIDI messages.  You can use keyboard input as well, as shown in the HTML. 
The client is set up for use on the [shiftr.io test MQTT broker](https://www.shiftr.io/try/), but has also been tested on other brokers.
 
This [Arduino MQTT-to-MIDI Player Client]({{site.codeurl}}/arduino-clients/MqttClientMIDIPlayer/MqttClientMIDIPlayer.ino) can receive MIDI messages from the same broker and send MIDI to your operating system or MIDI system.  This [Arduino MIDI-to-MQTT Congtroller Client]({{site.codeurl}}/arduino-clients/MqttClientMIDIController/MqttClientMIDIController.ino) can send noteon and noteoff messages via MQTT at the push of a button.

## mqttjsHueLightControl
* [See the example running](mqttjsHueLightControl)
* [See the source code]({{site.codeurl}}/browser-clients/mqttjs/mqttjsHueLightControl)

This example will change the brightness of a light on a local [Philips Hue Hub](https://tigoe.github.io/hue-control/) when you change the slider, and will send an MQTT message with the value of the slider when you press the button. If it receives an MQTT message on the `lights` topic, it uses that value to change the brightness of the light. So you can use this to change the brightness of a local hub, or of a friend's hub on a remote network if you are both connected to the same broker.