/*
  Bare minimum client example for mqtt.js

  On document load, this script gets two divs from the HTML
  for local and remote messages. Then it attempts
  to connect to the broker. Once every two seconds, 
  it sends the local time if it's connected.

  References:
  * mqtt.js client library: https://github.com/mqttjs/MQTT.js
  * shiftr.io broker: https://www.shiftr.io/try
  * shiftr.io desktop client: https://www.shiftr.io/desktop
  * emqx.io broker: https://www.emqx.com/en/mqtt/public-mqtt5-broker

  created 29 Dec 2022
  by Tom Igoe
*/

// All these brokers work with this code. 
// Uncomment the one you want to use. 

////// emqx. Works in both basic WS and TLS WS:
// const url = 'wss://broker.emqx.io:8084/mqtt'
// const url = 'ws://broker.emqx.io:8083/mqtt'

//////// shiftr.io desktop client. 
// Fill in your desktop URL for localhost:
// const url = 'ws://localhost:1884';     

//////// shiftr.io, requires username and password:
// const url = 'wss://public.cloud.shiftr.io';

//////// test.mosquitto.org, uses no username and password:
const url = 'wss://test.mosquitto.org:8081';

// MQTT client:
let client;

// connect options:
let options = {
  // Clean session
  clean: true,
  // connect timeout in ms:
  connectTimeout: 10000,
  // Authentication
  clientId: 'mqttJsClient',
  // add these in for public.cloud.shiftr.io:
  // username: 'public',
  // password: 'public'
}
// topic to subscribe to when you connect:
let topic = 'aardvarks';
// divs to show messages:
let localDiv, remoteDiv;

function setup() {
  // put the divs in variables for ease of use:
  localDiv = document.getElementById('local');
  remoteDiv = document.getElementById('remote');

  // set text of localDiv:
  localDiv.innerHTML = 'trying to connect';
  // attempt to connect:
  client = mqtt.connect(url, options);
  // set listeners:
  client.on('connect', onConnect);
  client.on('close', onDisonnect);
  client.on('message', onMessage);
  client.on('error', onError);
}

function loop() {
  // if the client is connected, publish:
  if (client.connected) {
    // make a message with a random number from 0-255
    let thisMessage = Math.floor(Math.random() * 255).toString();
    // publish to broker:
    client.publish(topic, thisMessage);
    // update localDiv text:
    localDiv.innerHTML = 'published to broker.'
  }
}
function onConnect() {
   // update localDiv text:
  localDiv.innerHTML = 'connected to broker. Subscribing...'
  // subscribe to the topic:
  client.subscribe(topic, onSubscribe);
}

function onDisonnect() {
 // update localDiv text:
  localDiv.innerHTML = 'disconnected from broker.'
}

function onError(error) {
   // update localDiv text:
  localDiv.innerHTML = error;
}

function onSubscribe(error) {
  if (!error) {
     // update localDiv text:
    localDiv.innerHTML = 'Subscribed to broker.';
  } else {
     // update localDiv text with the error:
    localDiv.innerHTML = error;
  }
}


function onMessage(topic, payload, packet) {

  let result = 'received a message on topic:  ' + topic;
    // message is  a Buffer, so convert to a string:
  result += '<br>message payload: ' + payload.toString();
  // packet is a JSON object, so list its elements:
  result += '<br>MQTT packet: <ul>';
  for (let item in packet) {
    result += '<li>' + item + ': ' + packet[item] + '</li>';
  }
  // close the ul tag
  result += '</ul>';
  // update the remote div text:
  remoteDiv.innerHTML = result;
}

// on page load, call the QR code function:
document.addEventListener('DOMContentLoaded', setup);
// run a loop every 2 seconds:
setInterval(loop, 2000);