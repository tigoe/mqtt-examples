# p5.js Client Example

Here's an example for p5.js in which you can send a message to someone else's MQTT client from a button in an HTML page in your browser. 

Open the sketch in a browser. Here's a [link to this example](https://tigoe.github.io/mqtt-examples/p5js-mqtt-client/public/index.html) hosted online at gitHub, but you can also download this whole repository, and open the index.html page in the p5js-mqtt-client folder in your browser.

When you open the page, you'll get a button labeled "Send a message" and a circle and two lines of text reading:

````
I sent: 0
I got a message:
````

When you click the button, you're sending an MQTT message to `shiftr.io/try` to a topic called `/notes`, with a random number from 0 to 15. Your page is also listening for messages from that topic too. When you receive any numeric message greater than 0, the circle will turn white, then slowly fade to black until another message is received.  Congratulations, you're sending and receiving MQTT messages. For the details, check out the [javaScript](public/sketch.js) for this page. 

This page will also work with the Arduino MQTT clients in this repository, specifically the [MqttClientSender](../MqttClientSender) and [MqttClientButtonLed](../MqttClientButtonLed).
