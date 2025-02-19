/*
    p5.js MQTT Client example
    This example uses p5.js: https://p5js.org/
    and the mqtt.js client library: https://www.npmjs.com/package/mqtt
    to create an MQTT client that sends and receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker,
    but other options listed will work.

    created 12 June 2020
    modified 19 Feb 2025
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
    clientId: 'mqttJsClient-' + Math.floor(Math.random()*1000000) ,
    // add these in for public.cloud.shiftr.io:
    username: 'public',
    password: 'public'
  }
  

// topic to subscribe to when you connect:
let topic = 'monkey';
// divs to show messages:
let localDiv, remoteDiv;

// position of the circle
let xPos, yPos;

function setup() {
    createCanvas(400, 400);
    // Create an MQTT client:
 // attempt to connect:
 client = mqtt.connect(broker, options);
 // set listeners:
 client.on('connect', onConnect);
 client.on('close', onDisconnect);
 client.on('message', onMessage);
 client.on('error', onError);

    // create a div for local messages:
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);
    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(20, 80);
}

function draw() {
    background(255);
    noStroke();
    // draw a circle when a message is received:
    fill('#2398CE');
    // circle moves with the message:
    circle(xPos, yPos, 30);
}

function mousePressed() {
    sendMqttMessage(mouseX + ',' + mouseY);
}

// called when the client connects
function onConnect() {
    localDiv.html('client is connected');
    client.subscribe(topic);
} 
  // handler for mqtt disconnect event:
  function onDisconnect() {
    // update localDiv text:
    localDiv.html('disconnected from broker.');
  }
  
  // handler for mqtt error event:
  function onError(error) {
    // update localDiv text:
    localDiv.html(error);
  }
  
  // handler for mqtt subscribe event:
  function onSubscribe(response, error) {
    if (!error) {
      // update localDiv text:
      localDiv.html('Subscribed to broker.');
    } else {
      // update localDiv text with the error:
      localDiv.html(error);
    }
  }

// called when a message arrives
function onMessage(topic, payload, packet) {
    let message = payload.toString(); 
    remoteDiv.html('I got a message:' + message);
    // assume the message is two numbers, mouseX and mouseY.
    // Split it into an array:
    let values = message.split(',');
    // convert the array values into numbers:
    xPos = Number(values[0]);
    yPos = Number(values[1]);
}

// called when you want to send a message:
function sendMqttMessage(msg) {
    // if the client is connected to the MQTT broker:
    if (client.connected) {
        client.publish(topic, msg);
        // update localDiv text
        localDiv.html('I sent: ' + msg);
    }
}