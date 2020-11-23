/*
    p5.js MQTT Client and p5.serialport example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    It takes input via websocket from p5.serialport, 
    allowing you to connect microcontroller projects together
    using p5.js, p5.serialport, and MQTT.
    The client is set up for use on the shiftr.io test MQTT broker (https://shiftr.io/try),
    but has also been tested on https://test.mosquitto.org

    created 12 June 2020
    modified 16 Oct 2020
    by Tom Igoe
*/

// MQTT client details:
let broker = {
    hostname: 'broker.shiftr.io',
    port: 443
};
// MQTT client:
let client;
// client credentials:
// For shiftr.io, use try for both username and password
// unless you have an account on the site. 
let creds = {
    clientID: 'p5Client',
    userName: 'public',
    password: 'public'
}
// topic to subscribe to when you connect
// For shiftr.io, use try/ then whatever word you want
// unless you have an account on the site. 
let topic = 'try/circle';

// HTML divs for local and remote messages
let localDiv;
let remoteDiv;
// position of the circle
let xPos, yPos;
// variable to hold an instance of the serialport library
let serial;

// fill in your serial port name here
let portName = '/dev/cu.usbmodem1422301';

let lastTimeSent = 0;
const sendInterval = 1000;

function setup() {
    createCanvas(400, 400);
    // Create an MQTT client:
    client = new Paho.MQTT.Client(broker.hostname, broker.port, creds.clientID);
    // set callback handlers for the client:
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    // connect to the MQTT broker:
    client.connect(
        {
            onSuccess: onConnect,       // callback function for when you connect
            userName: creds.userName,   // username
            password: creds.password,   // password
            useSSL: true                // use SSL
        }
    );
    // create a div for local messages:
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);

    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(20, 80);

    serial = new p5.SerialPort();
    serial.on('open', portOpen);        // callback for the port opening
    serial.on('data', serialEvent);     // callback for when new data arrives
    serial.on('error', serialError);    // callback for errors
    serial.open(portName);              // open a serial port
}

function draw() {
    background(255);
    noStroke();
    // draw a circle when a message is received:
    fill('#2398CE');
    // circle moves with the message:
    circle(xPos, yPos, 30);
}

function portOpen() {
    console.log('the serial port has opened.')
}

function serialEvent() {
    // read a byte from the serial port, convert it to a number:
     let inData = serial.readLine();
  // send it as an MQTT message to the topic:
    if (inData && millis() - lastTimeSent > sendInterval) {
        sendMqttMessage(inData);
        lastTimeSent = millis();             
    }
}

function serialError(err) {
    console.log('Something went wrong with the serial port. ' + err);
}

// called when the client connects
function onConnect() {
    localDiv.html('client is connected');
    client.subscribe(topic);
}

// called when the client loses its connection
function onConnectionLost(response) {
    if (response.errorCode !== 0) {
        console.log(response.errorMessage);
        localDiv.html('onConnectionLost:' + response.errorMessage);
    }
}

// called when a message arrives
function onMessageArrived(message) {
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