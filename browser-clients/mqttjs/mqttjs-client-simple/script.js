/*
  Bare minimum client example for mqtt.js

  On document load, this script gets two divs from the HTML
  for local and remote messages. Then it attempts
  to connect to the broker. Once every two seconds, 
  it sends the local time if it's connected. 
  The publish button allows you to turn on and off publishing status.

  created 29 Dec 2022
  modified 25 Feb 2026
  by Tom Igoe
*/

// All these brokers work with this code. 
// Uncomment the one you want to use. 

////// emqx. Works in both basic WS and TLS WS:
// const broker = 'wss://broker.emqx.io:8084/mqtt'
// const broker = 'ws://broker.emqx.io:8083/mqtt'

//////// shiftr.io desktop client. 
// Fill in your desktop IP address for localhost:
// const broker = 'ws://localhost:1884';     

//////// shiftr.io, requires username and password 
// (see options variable below):
const broker = 'wss://public.cloud.shiftr.io';

//////// test.mosquitto.org, uses no username and password:
// const broker = 'wss://test.mosquitto.org:8081';

// MQTT client:
let client;

// connection options:
let options = {
  // Clean session
  clean: true,
  // connect timeout in ms:
  connectTimeout: 10000,
  // Authentication
  // add a random number for a unique client ID:
  clientId: 'mqttJsClient-' + Math.floor(Math.random() * 1000000),
  // add these in for public.cloud.shiftr.io:
  username: 'public',
  password: 'public'
}
// topic to subscribe to when you connect:
let topic = 'aardvarks';
// divs to show messages:
let localDiv, remoteDiv;
let topicField;
// whether the client should be publishing or not:
let publishing = false;

function setup() {
  // put the divs in variables for ease of use:
  localDiv = document.getElementById('local');
  remoteDiv = document.getElementById('remote');
  topicField = document.getElementById('topic');
  // set text of localDiv:
  localDiv.innerHTML = 'trying to connect';

  // attempt to connect:
  client = mqtt.connect(broker, options);
  // set listeners:
  client.on('connect', onConnect);
  client.on('close', onDisconnect);
  client.on('message', onMessage);
  client.on('error', onError);
  // set topic:
  topicField.value = topic;
}

function loop() {
  // if the client is connected, publish:
  if (client.connected && publishing) {
    // make a message with a random number from 0-255
    let thisMessage = Math.floor(Math.random() * 255).toString();
    // publish to broker:
    client.publish(topic, thisMessage);
    // update localDiv text:
    localDiv.innerHTML = 'published to broker.'
  }
}

// changes the status of the publishing variable
// on a click of the publishStatus button:
function changeSendStatus(target) {
  // change the publishing status:
  publishing = !publishing;
  // set the html of the button accordingly:
  if (publishing) {
    target.innerHTML = 'stop publishing';
  } else {
    target.innerHTML = 'start publishing';
  }
}

// handler for mqtt connect event:
function onConnect() {
  console.log("connecting");
  // update localDiv text:
  localDiv.innerHTML = 'connected to broker. Subscribing...'
  // subscribe to the topic:
  client.subscribe(topic, onSubscribe);
}

// handler for mqtt disconnect event:
function onDisconnect() {
  // update localDiv text:
  localDiv.innerHTML = 'disconnected from broker.'
}

// handler for mqtt error event:
function onError(error) {
  // update localDiv text:
  localDiv.innerHTML = error;
}

// handler for mqtt subscribe event:
function onSubscribe(response, error) {
  if (!error) {
    // update localDiv text:
    localDiv.innerHTML = 'Subscribed to broker.';
  } else {
    // update localDiv text with the error:
    localDiv.innerHTML = error;
  }
}

// handler for mqtt message received event:
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

function setTopic() {
  topic = topicField.value;
  console.log(topic);
  // subscribe to the topic:
  client.subscribe(topic, onSubscribe);
}

// on page load, call the setup function:
document.addEventListener('DOMContentLoaded', setup);
// run a loop every 2 seconds:
setInterval(loop, 2000);