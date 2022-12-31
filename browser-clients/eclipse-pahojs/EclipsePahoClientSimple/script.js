/*
  Bare minimum client example for Eclipse PAHO JS library

  On document load, this script gets two divs from the HTML
  for local and remote messages. Then it attempts
  to connect to the broker. Once every two seconds, 
  it sends the local time if it's connected. 
  The publish button allows you to turn on and off publishing status.

  created 31 Dec 2022
  by Tom Igoe
*/

// All these brokers work with this code. 
// Uncomment the one you want to use. 

////// emqx. Works in both basic WS and SSL WS:
// const broker = 'broker.emqx.io'
// const port = 8083;   // no SSL
// const broker = 'broker.emqx.io'
// const port = 8084;   // SSL

//////// shiftr.io desktop client. 
// Fill in your desktop URL for localhost:
// const broker = 'localhost';   
// const port = 1884;  //  no SSL

//////// shiftr.io, requires username and password 
// (see options variable below):
const broker = 'public.cloud.shiftr.io';
const port = 443;

//////// test.mosquitto.org, uses no username and password:
// const broker = 'test.mosquitto.org';
// const port = 8081;

// MQTT client:
let client;
// client credentials:
let clientID = 'EclipsePahoClient';

let options = {
  // Clean session
  cleanSession: true,
  // connect timeout in seconds:
  timeout: 10,
  // callback function for when you connect:
  onSuccess: onConnect,
  // username & password:
  userName: 'public',
  password: 'public',
  // use SSL
  useSSL: true
};

// topic to subscribe to when you connect:
let topic = 'aardvarks';
// divs to show messages:
let localDiv, remoteDiv;
// whether the client should be publishing or not:
let publishing = true;

function setup() {
  // put the divs in variables for ease of use:
  localDiv = document.getElementById('local');
  remoteDiv = document.getElementById('remote');

  // set text of localDiv:
  localDiv.innerHTML = 'trying to connect';
  // Create an MQTT client:
  client = new Paho.MQTT.Client(broker, port, clientID);
  // set callback handlers for the client:
  client.onConnectionLost = onDisconnect;
  client.onMessageArrived = onMessage;
  // connect to the MQTT broker:
  client.connect(options);
}

function loop() {
  if (client.isConnected() && publishing) {
    // make a message with a random number from 0-255
    let payload = Math.floor(Math.random() * 255).toString();
    // publish to broker:
    let message = new Paho.MQTT.Message(payload);
    // // choose the destination topic:
    message.destinationName = topic;
    // send it:
    client.send(message);
    // update localDiv text:
    localDiv.innerHTML = 'published ' + message.payloadString + ' to broker.'
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
  // update localDiv text:
  localDiv.innerHTML = 'connected to broker. Subscribing...'
  // subscribe to the topic:
  client.subscribe(topic, onSubscribe);
}

// handler for mqtt disconnect event:
function onDisconnect(response) {
  // update localDiv text:
  if (response.errorCode !== 0) {
    localDiv.innerHTML = 'disconnected from broker: ' + response.errorMessage;
  }
}


// handler for mqtt error event:
function onError(error) {
  // update localDiv text:
  localDiv.innerHTML = error;
}

// handler for mqtt subsribe event:
function onSubscribe(error) {
  if (!error) {
    // update localDiv text:
    localDiv.innerHTML = 'Subscribed to broker.';
  } else {
    // update localDiv text with the error:
    localDiv.innerHTML = error;
  }
}

// handler for mqtt message received event:
function onMessage(message) {
  let result = 'received a message:<br>\n<ul>';
  // message is  a Buffer, so convert to a string:
  result += '<li>message payload: ' + message.payloadString + '</li>';
  result += '<li>message destinationName: ' + message.destinationName + '</li>';
  result += '<li>message qos: ' + message.qos + '</li>';
  result += '<li>message retained: ' + message.retained + '</li>';
  result += '<li>message duplicate: ' + message.duplicate + '</li>';
  // close the ul tag
  result += '</ul>';
  // update the remote div text:
  remoteDiv.innerHTML = result;
}

// on page load, call the setup function:
document.addEventListener('DOMContentLoaded', setup);
// run a loop every 2 seconds:
setInterval(loop, 2000);