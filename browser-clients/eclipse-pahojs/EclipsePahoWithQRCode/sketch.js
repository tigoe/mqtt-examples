/*
  QR Code generator and MQTT sender

  Draws a QR code using a text string. The QR code is the sketch's 
  URL. Also sends an MQTT message to shiftr.io.
  
  Uses 
  https://github.com/kazuhikoarase/qrcode-generator
  as the QR Code generator library. It's hosted at this CDN:
  https://unpkg.com/qrcode-generator@1.4.4/qrcode.js

  created 22 Aug 2020
  modified 23 Nov 2020
  by Tom Igoe
*/

// a string to diisplay in the QR code
// (the URL of this sketch):
let urlString = parent.location.href;
// an HTML div to display it in:
let tagDiv;

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
// client credentials (add random number for unique ID):
let clientID = 'p5HueClient-' + Math.floor(Math.random()*1000000);

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

// topic to subscribe to when you connect to the broker:
let topic = 'lights';

// UI elements: 
let dimmer;
let brightness = 0;

// divs for text from the broker:
let localDiv;
let remoteDiv;

function setup() {
   noCanvas();
   noLoop();
   
   // make the HTML tag div:
   tagDiv = createDiv();
   // make the QR code:
   let qr = qrcode(0, 'L');
   qr.addData(urlString);
   qr.make();
   // create an image from it:
   let qrImg = qr.createImgTag(2, 8, "qr code");
   // put the image and the URL string into the HTML div:
   tagDiv.html(qrImg);
   // position it:
   tagDiv.position(10, 10);
   // set a callback function for clicking on the tag:
   tagDiv.mousePressed(hideTag);

     // a slider to dim one light:
   dimmer = createSlider(0, 254, 127)
   // position it:
   dimmer.position(10, 160);
   // set a behavior for it:
   dimmer.mouseReleased(changeBrightness);

   // Create an MQTT client:
   client = new Paho.MQTT.Client(broker, port, clientID);
   // set callback handlers for the client:
   client.onConnectionLost = onDisconnect;
   client.onMessageArrived = onMessage;
   // connect to the MQTT broker:
   client.connect(options);

   // create a div for local messages:
   localDiv = createDiv('local messages will go here');
   localDiv.position(20, 100);
   // create a div for the response:
   remoteDiv = createDiv('waiting for messages');
   remoteDiv.position(20, 130);
}

function draw() {

}

// This function hides the tag div when you click on it:
function hideTag() {
   tagDiv.hide();
}

function changeBrightness() {
   brightness = dimmer.value();
   sendMqttMessage(brightness);
}

// called when the client connects
function onConnect() {
   localDiv.html('client is connected');
   client.subscribe(topic);
}

// called when the client loses its connection
function onDisconnect(response) {
   if (response.errorCode !== 0) {
      localDiv.html('onDisconnect:' + response.errorMessage);
   }
}

// called when a message arrives
function onMessage(message) {
   remoteDiv.html('I got a message:' + message.payloadString);
 }

// called when you want to send a message:
function sendMqttMessage() {
   // if the client is connected to the MQTT broker:
   if (client.isConnected()) {
      // make a string with a random number form 0 to 15:
      let msg = String(brightness);
      // start an MQTT message:
      message = new Paho.MQTT.Message(msg);
      // choose the destination topic:
      message.destinationName = topic;
      // send it:
      client.send(message);
      // print what you sent:
      localDiv.html('I sent: ' + message.payloadString);
   }
}