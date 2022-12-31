/*
    p5.js MQTT Client example with JSON parsing
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    to create an MQTT client that receives MQTT messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://next.shiftr.io/try),
    but has also been tested on https://test.mosquitto.org

    created 12 June 2020
    modified 27 Sept 2020
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
let topic = 'try/AQISensor';

// a pushbutton to send messages
let sendButton;
let localDiv;
let remoteDiv;
let data = {
    CO2: 0,
    TVOC: 0
};

// intensity of the circle in the middle
let intensity = 255;

function setup() {
    // noCanvas();
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
    localDiv = createDiv('local messages will go here');
    localDiv.position(20, 50);
    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(20, 80);
}

function draw() {
    // background(255);
    noFill();
    stroke(0, 204, 255, 2);
    circle_size = data.CO2 / 2;
    circle(width / 2, height / 2, circle_size)
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
    // get the JSOn string from the incoming message and parse it:
    data = JSON.parse(message.payloadString);
    // put it in the HTML:
    remoteDiv.html("CO2:" + data.CO2 + "<br>TVOC: " + data.TVOC);
}