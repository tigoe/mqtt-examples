/*
    p5.js MQTT Client example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://next.shiftr.io/try),
    but has also been tested on https://test.mosquitto.org

    created 12 June 2020
    modified 20 Aug 2020
    by Tom Igoe
*/

// MQTT client details:
let broker = {
    hostname: 'public.cloud.shiftr.io',
    port: 443
};
// MQTT client:
let client;
// client credentials:
let creds = {
    clientID: 'p5Client',
    userName: 'public',
    password: 'public'
}
// topic to subscribe to when you connect:
let topic = 'notes';

// a pushbutton to send messages
let sendButton;
let localDiv;
let remoteDiv;

// intensity of the circle in the middle
let intensity = 255;

function setup() {
    createCanvas(400, 400);
    // Create an MQTT client:
    client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
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
    // create the send button:
    sendButton = createButton('send a message');
    sendButton.position(20, 20);
    sendButton.mousePressed(sendMqttMessage);
    // create a div for local messages:
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);
    localDiv.style('color', '#fff');
    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(20, 80);
    remoteDiv.style('color', '#fff');
}

function draw() {
    background(50);
    // draw a circle whose brightness changes when a message is received:
    fill(intensity);
    circle(width/2, height/2, width/2);
// subtract one from the brightness of the circle:
    if (intensity > 0) {
        intensity--;
    }
}

// called when the client connects
function onConnect() {
    localDiv.html('client is connected');
    client.subscribe(topic);
}

// called when the client loses its connection
function onConnectionLost(response) {
    if (response.errorCode !== 0) {
        localDiv.html('onConnectionLost:' + response.errorMessage);
    }
}

// called when a message arrives
function onMessageArrived(message) {
    remoteDiv.html('I got a message:' + message.payloadString);
    let  incomingNumber = parseInt(message.payloadString);
    if (incomingNumber > 0) {
        intensity = 255;
    }
}

// called when you want to send a message:
function sendMqttMessage() {
    // if the client is connected to the MQTT broker:
    if (client.isConnected()) {
        // make a string with a random number form 0 to 15:
        let msg = String(round(random(15)));
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