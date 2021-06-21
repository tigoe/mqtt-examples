/*
  Simple MQTT Node.js  client with serial port. 
  Connects to shiftr.io's public broker. Reads from serial
  port and sends to broker; reads from broker and sends to
  serial port. 

  To launch this, give the portname like so:
  node mqtt-client-seial.js portname

  Arduino sketch SerialInOut.ino works with this script.

  created 10 Apr 2021
  modified 21 Jun 2021
  by Tom Igoe
*/

// include the MQTT library:
const mqtt = require('mqtt');
// include serialport libraries
const SerialPort = require('serialport');			// include the serialport library
const Readline = require('@serialport/parser-readline');

// the broker you plan to connect to. 
// transport options: 
// 'mqtt', 'mqtts', 'tcp', 'tls', 'ws', or 'wss':
const broker = 'mqtt://public.cloud.shiftr.io';

// client options:
const options = {
  clientId: 'nodeClient',
  username: 'public',
  password: 'public'
}
// topic and message payload:
let myTopic = 'lights';
let payload;

// connect handler:
function setupClient() {
  client.subscribe(myTopic);
  client.on('message', readMqttMessage);
}

// new message handler:
function readMqttMessage(topic, message) {
  // message is a Buffer, so convert to a string:
  let msgString = message.toString();
  console.log(topic + ': ' + msgString);
  // send it out the serial port:
  myPort.write(msgString);
}

// message sender:
function sendMqttMessage(topic, msg) {
  if (client.connected) {
    client.publish(topic, msg);
  }
}

///////////////////////// Serial functions
// make a readline parser:
const parser = new Readline({ delimiter: '\r\n' });
// get the port name from the command line:
var portName = process.argv[2];
// serial port config:
var portConfig = {
  baudRate: 9600,
};

function openPort() {
  console.log('port open');
  console.log('baud rate: ' + myPort.baudRate);
}

function serialEvent(data) {
  sendMqttMessage(myTopic, data);
  console.log(data);
}

// open the serial port:
var myPort = new SerialPort(portName, portConfig);
// set up a line reading parser:
myPort.pipe(parser);
// open the port:
myPort.on('open', openPort);
// set up a listener for new lines:
parser.on('data', serialEvent);

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on('connect', setupClient);