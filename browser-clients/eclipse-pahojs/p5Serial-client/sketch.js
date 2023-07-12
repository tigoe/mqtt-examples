/*
    p5.js MQTT Client and p5.WebSerial example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    It takes input via asynchronous serial from p5.WebSerial, 
    allowing you to connect microcontroller projects together
    using p5.js, p5.WebSerial, and MQTT.
    NOTE: WebSerial does not work on all browsers, just Chrome, Edge, and Opera.
    See this chart for more: 
    https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility
    The MQTT client is set up for use on the shiftr.io test MQTT broker,
    but other options listed will work.

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
// client credentials (add random number for unique ID):
let clientID = 'EclipsePahoClient-' + Math.floor(Math.random()*1000000);

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
let topic = 'circle';
// divs to show messages:
let localDiv, remoteDiv;

// position of the circle
let xPos, yPos;
// variable to hold an instance of the p5.webserial library:
const serial = new p5.WebSerial();

// HTML button object:
let portButton;
// timestamp and interval for serial sending, in ms:
let lastTimeSent = 0;
const sendInterval = 500;

function setup() {
    createCanvas(400, 400);
    // Create an MQTT client:
    client = new Paho.MQTT.Client(broker, port, clientID);
    // set callback handlers for the client:
    client.onConnectionLost = onDisconnect;
    client.onMessageArrived = onMessage;
    // connect to the MQTT broker:
    client.connect(options);
    // create a div for local messages:
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);
    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(20, 80);

    // Serial initialization:
    // check to see if serial is available:
    if (!navigator.serial) {
        alert("WebSerial is not supported in this browser. Try Chrome or MS Edge.");
    }
    // if serial is available, add connect/disconnect listeners:
    navigator.serial.addEventListener("connect", portConnect);
    navigator.serial.addEventListener("disconnect", portDisconnect);
    // check for any ports that are available:
    serial.getPorts();
    // if there's no port chosen, choose one:
    serial.on("noport", makePortButton);
    // open whatever port is available:
    serial.on("portavailable", openPort);
    // handle serial errors:
    serial.on("requesterror", portError);
    // handle any incoming serial data:
    serial.on("data", serialEvent);
    serial.on("close", makePortButton);
}

function draw() {
    background(255);
    noStroke();
    // draw a circle when a message is received:
    fill('#2398CE');
    // circle moves with the message:
    circle(xPos, yPos, 30);
}

// if there's no port selected, 
// make a port select button appear:
function makePortButton() {
    // create and position a port chooser button:
    portButton = createButton('choose port');
    portButton.position(10, 10);
    // give the port button a mousepressed handler:
    portButton.mousePressed(choosePort);
}

// make the port selector window appear:
function choosePort() {
    serial.requestPort();
}

// open the selected port, and make the port 
// button invisible:
function openPort() {
    // wait for the serial.open promise to return,
    // then call the initiateSerial function
    serial.open().then(initiateSerial);

    // once the port opens, let the user know:
    function initiateSerial() {
        localDiv.html("port open");
    }
    // hide the port button once a port is chosen:
    if (portButton) portButton.hide();
}

// read any incoming serial data:
function serialEvent() {
    // read a line from the serial port:
    let inData = serial.readLine();
    // send it as an MQTT message to the topic:
    if (inData && millis() - lastTimeSent > sendInterval) {
        sendMqttMessage(inData);
        lastTimeSent = millis();
    }
}

// pop up an alert if there's a port error:
function portError(err) {
    alert("Serial port error: " + err);
    serial.port.forget();
}

// try to connect if a new serial port 
// gets added (i.e. plugged in via USB):
function portConnect() {
    localDiv.html("port connected");
    serial.getPorts();
}

// if a port is disconnected:
function portDisconnect() {
    serial.close();
    serial.port.forget();
    localDiv.html("port disconnected");
}

// called when the client connects
function onConnect() {
    localDiv.html('client is connected');
    client.subscribe(topic);
}

// called when the client loses its connection
function onDisconnect(response) {
    if (response.errorCode !== 0) {
        console.log(response.errorMessage);
        localDiv.html('broker disconnect:' + response.errorMessage);
    }
}

// called when a message arrives
function onMessage(message) {
    remoteDiv.html('I got a message:' + message.payloadString);
    // assume the message payload is a JSON object
    // From the ArduinoJoystick example in this folder:
    // {"x":xPos,"y":yPos,"button":buttonState}
    // parse it and use the X and Y:
    var joyStick = JSON.parse(message.payloadString);
    xPos = joyStick.x;
    yPos = joyStick.y;
}

// called when you want to send a message:
function sendMqttMessage(msg) {
    // if the client is connected to the MQTT broker:
    if (client.isConnected()) {
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