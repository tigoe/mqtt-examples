# Node.js Clients

There are three clients built in node.js in this repository
* [MqttNodeClient]({{site.codeurl}}/node-clients/MqttNodeClient/) which is a basic example of how to make a client with the [node.js MQTT library](https://www.npmjs.com/package/mqtt). It sends a reading every few seconds, and subscribes to a topic called `lights`. It will work with the light control examples above.  
* [MqttNodeClientSerial]({{site.codeurl}}/node-clients/MqttNodeClientSerial/) is similar, but it uses the node serialport library from [serialport.io](https://serialport.io/docs) to connect MQTT to serial in and out.
* [MqttNodeClientFileWriter]({{site.codeurl}}/node-clients/MqttNodeClientFileWriter/) reads messages from a topic and writes them to a text file.
