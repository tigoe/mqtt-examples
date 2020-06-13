/*
    p5.js MQTT Client example
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that sends and receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://shiftr.io/try),
    but has also been tested on https://test.mosquitto.org

    created 12 June 2020
    by Tom Igoe
*/

// MQTT client details:
let broker = {
    hostname: 'broker.shiftr.io/try',
    port: 80
};
// MQTT client:
let client;
// client credentials:
let creds = {
    clientID: 'p5Client',
    userName: 'try',
    password: 'try'
}
// topic to subscribe to when you connect:
let topic = 'time';

function setup() {
    createCanvas(640, 640);
    // run draw() every two seconds:
    frameRate(0.5);
    textSize(32);
    fill(0);
    // Create an MQTT client 
    client = new Paho.MQTT.Client(broker.hostname, Number(broker.port), creds.clientID);
    // set callback handlers for the client:
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    // connect to the MQTT broker:
    client.connect(
        {
            onSuccess: onConnect,       // callback function for when you connect
            userName: creds.userName,   // username
            password: creds.password    // password
        }
    );
}

function draw() {
    background(255);
    // if the client is connected to the MQTT broker:
    if (client.isConnected()) {
        // make a string with the current time:
        let msg = 'The time: ' + hour() + ':' + minute() + ':' + second();
        // start an MQTT message:
        message = new Paho.MQTT.Message(msg);
        // choose the destination topic:
        message.destinationName = topic;
        // send it:
        client.send(message);
        // print what you sent:
        text('I sent: ' + message.payloadString, 10, 50);
    }
}

// called when the client connects
function onConnect() {
    console.log('client is connected');
    client.subscribe(topic);
}

// called when the client loses its connection
function onConnectionLost(response) {
    if (response.errorCode !== 0) {
        console.log('onConnectionLost:' + response.errorMessage);
    }
}

// called when a message arrives
function onMessageArrived(message) {
    text('I got a message:' + message.payloadString, 10, 100);
    console.log('I got a message: ' + message.payloadString);
}