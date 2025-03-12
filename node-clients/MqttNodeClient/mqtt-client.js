/*
  Simple MQTT Node.js  client. 
  Connects to shiftr.io's public broker, and sends
  a message to a topic called lights every two 
  seconds. 

  created 10 Apr 2021
  by Tom Igoe
*/

// include the MQTT library:
const mqtt = require('mqtt');
// the broker you plan to connect to. 
// transport options: 
// 'mqtt', 'mqtts', 'tcp', 'tls', 'ws', or 'wss':
//const broker = 'mqtt://test.mosquitto.org';
const broker = 'mqtt://public.cloud.shiftr.io';

// client options:
const options = {
  // add a random number for a unique clientId:
  clientId: 'nodeClient-' + Math.floor(Math.random()*1000000),
  username: 'public',
  password: 'public',
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000
}
// topic and message payload:
let myTopic = 'aardvarks';
let payload;

// connect handler:
function setupClient() {
  console.log('setup');
  client.subscribe(myTopic);
  client.on('message', readMqttMessage);
}

// new message handler:
function readMqttMessage(topic, message) {
  // message is a Buffer, so convert to a string:
  let msgString = message.toString();
  console.log(topic);
  console.log(msgString);
}

 // message sender:
function sendMqttMessage(topic, msg) {
  if (client.connected) {
    let msgString = JSON.stringify(msg);
    client.publish(topic, msgString);
    console.log('update');
  }
}

// setInterval handler:
function update() {
  payload = Math.round(Math.random(254) * 254);
  sendMqttMessage(myTopic, payload);
}

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on('connect', setupClient);

// // send a message every two seconds:
 setInterval(update, 100);