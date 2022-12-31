/*
    MQTT Client example with JSON parsing
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker,
    but other options listed will work.

    There's no loop here: all the functions are event-driven.
    
    created 12 June 2020
    modified 31 Dec 2022
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
let topic = 'AQISensor';
// divs to show messages:
let localDiv, remoteDiv;

// incoming data:
let data;

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

// handler for mqtt connect event:
function onConnect() {
  // update localDiv text:
  localDiv.innerHTML = 'connected to broker.'
  // subscribe to the topic:
  client.subscribe(topic, {onSuccess:onSubscribe});
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

// handler for mqtt subscribe event:
function onSubscribe(response) {
    // update localDiv text:
    localDiv.innerHTML = JSON.stringify(response)
    +'<br>Subscribed to ' + topic;
}

function subscribeToTopic(target) {
  client.unsubscribe(topic);
  localDiv.innerHTML = "unsubscribed from " + topic;
   topic = target.value;
  if (client.isConnected()) {
    client.subscribe(topic, {onSuccess:onSubscribe});
  }
}

// handler for mqtt message received event:
function onMessage(message) {
  // variable to hold the incoming result:
  let result;
  // get the JSON string from the incoming message and parse it:
  try {
    data = JSON.parse(message.payloadString);
  } catch (error) {
    // if it's not JSON, report that
    result = "message is not JSON: " + message.payloadString;
    remoteDiv.innerHTML = result;
    return;
  }
  // assuming you got good JSON: 
  result = "Incoming data: <br>\n<ul>";
  // if it's JSON, doesn't matter what the items are, 
  // you can parse it as needed:
  for (item in data) {
    result += "<li>" + item + ":" + data[item] + "</li>\n";
  }
  // put it in the HTML:
  remoteDiv.innerHTML = result;
}

// on page load, call the setup function:
document.addEventListener('DOMContentLoaded', setup);