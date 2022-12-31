/*
    p5.js MQTT MIDI Client
    This example uses p5.js: https://p5js.org/
    and the Eclipse Paho MQTT client library: https://www.eclipse.org/paho/clients/js/
    and the Web MIDI API (https://www.w3.org/TR/webmidi/)
    to create an MQTT client that sends and receives MQTT messages
    that are MIDI messages.
    The client is set up for use on the shiftr.io test MQTT broker (https://public.cloud.shiftr.io),
    but has also been tested on https://test.mosquitto.org

    created 11 Nov 2020
    modiified 23 Nov 2020
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
// For shiftr.io, use try for both username and password
// unless you have an account on the site. 
let creds = {
    clientID: 'p5MidiClient',
    userName: 'public',
    password: 'public'
}
// topic to subscribe to when you connect
// For shiftr.io, use whatever word you want for the subtopic
// unless you have an account on the site. 
let subTopic = '';
let topic = 'midi';

// HTML divs for local and remote messages
let localDiv;
let remoteDiv;

// select menus for MIDI inputs and outputs:
let inputSelect, outputSelect;

// arrays for the MIDI devices:
let outputDevices = new Array();
let inputDevices = new Array();

// variables for the currently selected ones:
let currentOutput, currentInput;

// an HTML div for messages:
let messageDiv;


function setup() {
    noCanvas();
    noLoop();

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
    localDiv.position(10, 110);

    // create a div for the response:
    remoteDiv = createDiv('waiting for messages');
    remoteDiv.position(10, 140);


    // create the select menus and position them:
    inputSelect = createSelect();
    inputSelect.position(10, 10);
    inputSelect.changed(selectInput);
    inputSelect.option('--Choose an input:--', 0);

    outputSelect = createSelect();
    outputSelect.position(10, 50);
    outputSelect.changed(selectOutput);
    outputSelect.option('--Choose an output:--', 0);

    // create the message div and position it:
    messageDiv = createDiv('messages will go here');
    messageDiv.position(10, 80);

    // initialize MIDI and get device lists:
    navigator.requestMIDIAccess()
        .then(getDevices);
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

// called when an MQTT  message arrives
function onMessageArrived(message) {
    let payload = message.payloadBytes;
    remoteDiv.html('I got a message ' + payload);
    // set up an arrayBuffer and a Uint8 array for the incoming:
    let incomingBuffer = new ArrayBuffer(payload.length);
    let midiCmd = new Uint8Array(incomingBuffer);

    // move the message payload into the UInt8 array:
    for (var i = 0; i < payload.length; i++) {
        midiCmd[i] = payload[i];
    }

    // if the message is intended for the current MIDIoutput, 
    // send it there:
    if (currentOutput != null) {
        currentOutput.send(midiCmd);
    }
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

// MIDI Functions /////////////////////////////////////////////////////

// Get lists of available MIDI controllers
function getDevices(midiAccess) {
    const inputs = midiAccess.inputs.values();
    const outputs = midiAccess.outputs.values();

    // add inputs and outputs to the global arrays and the select menus:
    for (let i of inputs) {
        addMidiItem(i);
    }
    for (let o of outputs) {
        addMidiItem(o);
    }

    // if any of the devices change state, add or delete it:
    midiAccess.onstatechange = function (item) {
        // if an item changes state, add it or delete it from the select menus:
        if (item.port.state == 'connected') {
            addMidiItem(item.port);
        }
        if (item.port.state == 'disconnected') {
            removeMidiItem(item.port);
        }

        // Print information about the changed MIDI controller:
        messageDiv.html(item.port.name + "  "
            + item.port.state);
    };
}

// add new MIDI devices:
function addMidiItem(midiItem) {
    // add to the appropriate select menu,
    // but make sure it's not already in the list:
    if (midiItem.type == 'input' && inputDevices.indexOf(midiItem) < 0) {
        inputSelect.option(midiItem.name);
        // add to the devices array too:
        inputDevices.push(midiItem);
    }
    if (midiItem.type == 'output' && outputDevices.indexOf(midiItem) < 0) {
        outputSelect.option(midiItem.name);
        // add to the devices array too:
        outputDevices.push(midiItem);
    }

    // add a message listener:
    midiItem.onmidimessage = getMIDIMessage;
}

// remove items when they go away:
function removeMidiItem(midiItem) {
    // choose the right select menu:
    if (midiItem.type == 'input') {
        selectMenu = inputSelect;
        inputDevices.splice(inputDevices.indexOf(midiItem), 1);
    }
    if (midiItem.type == 'output') {
        selectMenu = outputSelect;
        outputDevices.splice(outputDevices.indexOf(midiItem), 1);
    }

    // clear the message listener:
    midiItem.onmidimessage = null;

    // delete the item from the menu:
    for (let i = 0; i < selectMenu.elt.length; i++) {
        if (selectMenu.elt[i].innerHTML === midiItem.name) {
            selectMenu.elt[i].remove();
        }
    }
}

// select the current input or output from the select menus:
function selectInput() {
    for (let i of inputDevices) {
        if (i.name === inputSelect.selected()) {
            currentInput = i;
        }
    }
    // if they chose the default position, clear the current input:
    if (inputSelect.value() == 0) {
        currentInput = null;
    }
}

function selectOutput() {
    // iterate over the list of devices:
    for (let o of outputDevices) {
        if (o.name === outputSelect.selected()) {
            currentOutput = o;
        }
    }
    // if they chose the default position, clear the current output:
    if (outputSelect.value() == 0) {
        currentOutput = null;
    }
}

// MIDI message listener function:
function getMIDIMessage(message) {
    // if the message came from a device other than the current input, you're done:
    if (currentInput !== message.currentTarget) return;

    // print the message (print the MIDI bytes as hexadeciimal values):
    messageDiv.html("MIDI message: 0x"
        + message.data[0].toString(16)
        + ", 0x" + message.data[1].toString(16)
        + ", 0x" + message.data[2].toString(16));

    // if the message is intended for the current output, send it there:
    if (currentOutput != null) {
        currentOutput.send(message.data);
    }

    // if connected to MQTT, send message as MQTT message:
    if (client.isConnected()) {
        sendMqttMessage(message.data.buffer);
    }
}