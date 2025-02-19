/*
  Hue Hub light control over MQTT

  This example will change the brightness of a light
  on a local hub when you change the slider, and will
  send an MQTT message with the value of the slider
  when you press the button. If it receives an MQTT message
  on the `lights` topic, it uses that value to change the
  brightness of the light. So you can use this
  to change the brightness of a local hub, or of a 
  friend's hub on a remote network if you are both connected
  to the same broker.
  
  created 23 July 2020
  modified 18 Feb 2025
  by Tom Igoe
*/
// Fill in your hue hub IP credentials here:
let url = '192.168.0.8';
let username = 'xxxxxxxx-xxxxxxxxx';
// slider for dimming the lights:
let dimmer;
// the number of the light in the hub:
let lightNumber = 3;
// The light state:
let lightState = {
   bri: 0,
   on: false
}

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
// client credentials:
let clientID = 'p5HueClient';

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

// topic to subscribe to when you connect to the broker:
let topic = 'lights';

// UI elements: 
// a pushbutton to send messages:
let sendButton;
// divs for text from the broker:
let localDiv;
let remoteDiv;

function setup() {
   noLoop();
   //   createCanvas(windowWidth, windowHeight);
   // a div for the Hue hub's responses:
   remoteDiv = createDiv('Hub response');
   // position it:
   remoteDiv.position(20, 130);
   // a slider to dim one light:
   dimmer = createSlider(0, 254, 127)
   // position it:
   dimmer.position(10, 10);
   // set a behavior for it:
   dimmer.mouseReleased(changeBrightness);

   // attempt to connect:
   client = mqtt.connect(broker, options);
   // set listeners:
   client.on('connect', onConnect);
   client.on('close', onDisconnect);
   client.on('message', onMessage);
   client.on('error', onError);

   // create the send button:
   sendButton = createButton('send a message');
   sendButton.position(20, 40);
   sendButton.mousePressed(sendMqttMessage);
   // create a div for local messages:
   localDiv = createDiv('local messages will go here');
   localDiv.position(20, 70);
   // create a div for the response:
   remoteDiv = createDiv('waiting for messages');
   remoteDiv.position(20, 100);
   hueConnect();
}

/*
this function makes the HTTP GET call to get the light data:
HTTP GET http://your.hue.hub.address/api/username/lights/
*/
function hueConnect() {
   url = "http://" + url + '/api/' + username + '/lights/';
   httpDo(url, 'GET', getLights);
}

/*
this function uses the response from the hub
to create a new div for the UI elements
*/
function getLights(result) {
   remoteDiv.html(result);
}

function changeBrightness() {
   lightState.bri = dimmer.value();
   if (lightState.bri > 0) {
      lightState.on = true;
   } else {
      lightState.on = false;
   }
   // make the HTTP call with the JSON object:
   setLight(lightNumber, lightState);
}

/*
this function makes an HTTP PUT call to change the properties of the lights:
HTTP PUT http://your.hue.hub.address/api/username/lights/lightNumber/state/
and the body has the light state:
{
  on: true/false,
  bri: brightness
}
*/
function setLight(whichLight, data) {
   var path = url + whichLight + '/state/';

   var content = JSON.stringify(data);				 // convert JSON obj to string
   httpDo(path, 'PUT', content, 'text', getLights); //HTTP PUT the change
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

// handler for mqtt error event:
function onError(error) {
   // update localDiv text:
   localDiv.html("error: " + error);
 }

// called when a message arrives
function onMessage(topic, payload, packet) {
   remoteDiv.html('I got a message:' + payload);
   let incomingNumber = parseInt(payload);
   // use it to set the light:
   lightState.bri = incomingNumber;
   if (lightState.bri > 0) {
      lightState.on = true;
   } else {
      lightState.on = false;
   }
   // make the HTTP call with the JSON object:
   setLight(lightNumber, lightState);
}

// called when you want to send a message:
function sendMqttMessage() {
   // if the client is connected to the MQTT broker:
   if (client.isConnected()) {
      // make a string with a random number form 0 to 15:
      let msg = String(dimmer.value());
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