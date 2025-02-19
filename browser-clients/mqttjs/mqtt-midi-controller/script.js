/*
    p5.js MQTT and MIDI Client
    This example uses the mqtt.js library: https://www.npmjs.com/package/mqtt
    and the Web MIDI API (https://www.w3.org/TR/webmidi/)
    to create an MQTT client that sends and receives MQTT messages
    that are MIDI messages. 
    You can use keyboard input as well, as shown in the HTML. 
    The client is set up for use on the shiftr.io test MQTT broker (https://public.cloud.shiftr.io),
    but has also been tested on https://test.mosquitto.org

    created 11 Nov 2020
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
  clientId: 'jsMidiClient-' + Math.floor(Math.random() * 1000000),
  // add these in for public.cloud.shiftr.io:
  username: 'public',
  password: 'public'
}

// topic to subscribe to when you connect
// For shiftr.io, use whatever word you want for the subtopic
// unless you have an account on the site. This will reduce 
// the traffic from others. 
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
// local echo checkbox input:
let localEcho;

// an HTML div for messages:
let messageDiv;

function setup() {
  // put the divs in variables for ease of use:
  localDiv = document.getElementById('local');
  remoteDiv = document.getElementById('remote');

  // set text of localDiv:
  localDiv.innerHTML = 'trying to connect';
  // attempt to connect:
  client = mqtt.connect(broker, options);
  // set listeners:
  client.on('connect', onConnect);
  client.on('close', onDisconnect);
  client.on('message', onMessage);
  client.on('error', onError);

  // create keyPress and keyRelease listeners:
  document.addEventListener('keydown', keyPressed);
  document.addEventListener('keyup', keyReleased);

  // get the local echo checkbox:
  localEcho = document.getElementById("echo");

  // get the select menus:
  inputSelect = document.getElementById("inputs");
  inputSelect.addEventListener('change', selectInput);
  inputSelect[0] =
    new Option('--Choose a MIDI input:--', 0, false, false);

  outputSelect = document.getElementById("outputs");
  outputSelect.addEventListener('change', selectOutput);
  outputSelect[0] =
    new Option('--Choose a MIDI output:--', 0, false, false);
  // get the message div:
  messageDiv = document.getElementById('messages');

  // initialize MIDI and get device lists:
  navigator.requestMIDIAccess()
    .then(getDevices);
}

function keyPressed(event) {
  keyAction(event.keyCode, 'down');
}

function keyReleased(event) {
  keyAction(event.keyCode, 'up');
}

// sends a noteon or noteoff MIDI message for different keys:
function keyAction(key, direction) {
  // keycodes for notes. See 
  // https://sonicbloom.net/en/ableton-live-tutorial-computer-keyboard-as-midi-controller/ 
  // for layout:
  let keys = [65, 87, 83, 69, 68, 70, 84, 71, 89, 72, 85, 74, 75, 79, 76, 91];
  // first note is A3 (MIDI value 48):
  let baseNote = 48;
  // if they typed a key that's not one of the array, 
  // quit the function:
  if (!keys.includes(key)) return;
  // calculate MIDI note value:
  let thisNote = keys.indexOf(key) + baseNote;
  // make a MIDI command array:
  let midiCmd = new Uint8Array(3);
  // The note value is the second byte of the MIDI command:
  midiCmd[1] = thisNote;
  // up = noteOff (0x80), down = noteOn (0x90)
  // third byte is velocity (0x70 = reasonably loud):
  if (direction === 'up') {
    midiCmd[0] = 0x80;
    midiCmd[2] = 0x00;
  } else {
    midiCmd[0] = 0x90;
    midiCmd[2] = 0x70;
  }
  // print the MIDI bytes as hexadeciimal values:
  messageDiv.innerHTML = "MIDI message:"
  for (var i = 0; i < midiCmd.length; i++) {
    messageDiv.innerHTML += " 0x" + midiCmd[i].toString(16);
  }
  // if there is a current MIDIoutput, 
  // and localEcho is checked send it there:
  if (currentOutput != null && localEcho.checked) {
    currentOutput.send(midiCmd);
  }
  // send it by MQTT as well:
  sendMqttMessage(midiCmd.buffer);
}

// start everything when the DOM content loads:
window.addEventListener('DOMContentLoaded', setup);

/////////////////////////////// MQTT functions

// handler for mqtt connect event:
function onConnect() {
  // update localDiv text:
  localDiv.innerHTML = 'connected to broker. Subscribing...'
  // subscribe to the topic:
  client.subscribe(topic, onSubscribe);
}

// handler for mqtt disconnect event:
function onDisconnect() {
  // update localDiv text:
  localDiv.innerHTML = 'disconnected from broker.'
}

// handler for mqtt error event:
function onError(error) {
  // update localDiv text:
  localDiv.innerHTML = error;
}

// handler for mqtt subscribe event:
function onSubscribe(response, error) {
  if (!error) {
    // update localDiv text:
    localDiv.innerHTML = 'Subscribed to broker.';
  } else {
    // update localDiv text with the error:
    localDiv.innerHTML = error;
  }
}

// called when an MQTT  message arrives
function onMessage(topic, payload, packet) {
  // print what you sent in a fancy hexadecimal string:
  let result = 'I got a message: ';
  // convert the array to a hex string:
  for (var i = 0; i < payload.length; i++) {
    result += ' 0x' + payload[i].toString(16);
  };
  remoteDiv.innerHTML = result;
  // set up an arrayBuffer and a Uint8 array for the incoming:
  let incomingBuffer = new ArrayBuffer(payload.length);
  let midiCmd = new Uint8Array(incomingBuffer);

  // move the message payload into the UInt8 array:
  for (var i = 0; i < payload.length; i++) {
    midiCmd[i] = payload[i];
  }

  // if the message is intended for the current MIDIoutput, 
  //  send it there:
  if (currentOutput != null) {
    currentOutput.send(midiCmd);
  }
}

// called when you want to send a message:
function sendMqttMessage(msg) {
  // if the client is connected to the MQTT broker:
  // convert the message to a byte array for printing:
  let bytes = new Uint8Array(msg);

  if (client.connected) {
    client.publish(topic, bytes);
    // update localDiv text
    // print what you sent in a fancy hexadecimal string:
    let result = 'I sent: ';
    // function to convert a number to a hex string:
    for (var i = 0; i < bytes.length; i++) {
      result += ' 0x' + bytes[i].toString(16);
    };
    localDiv.innerHTML = result;
  }

  // if (client.isConnected()) {
  //   // start an MQTT message:
  //   message = new Paho.MQTT.Message(msg);
  //   // choose the destination topic:
  //   message.destinationName = topic;
  //   // send it:
  //   client.send(message);
  //   // convert the message to a byte array for printing:
  //   let bytes = new Uint8Array(message.payloadBytes);
  //   // print what you sent in a fancy hexadecimal string:
  //   let result = 'I sent: ';
  //   // function to convert a number to a hex string:
  //   for (var i = 0; i < bytes.length; i++) {
  //     result += ' 0x' + bytes[i].toString(16);
  //   };
  //   localDiv.innerHTML = result;
  // }
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
    messageDiv.innerHTML = item.port.name + "  "
      + item.port.state;
  };
}

// add new MIDI devices:
function addMidiItem(midiItem) {
  // add to the appropriate select menu,
  // but make sure it's not already in the list:
  if (midiItem.type == 'input' && inputDevices.indexOf(midiItem) < 0) {
    let optionNumber = inputSelect.options.length;
    inputSelect[optionNumber] =
      new Option(midiItem.name, false, false);
    // add to the devices array too:
    inputDevices.push(midiItem);
  }
  if (midiItem.type == 'output' && outputDevices.indexOf(midiItem) < 0) {
    let optionNumber = outputSelect.options.length;
    outputSelect[optionNumber] =
      new Option(midiItem.name, false, false);
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
    // remove the item that's disconnected:
    inputDevices.splice(inputDevices.indexOf(midiItem), 1);
  }
  if (midiItem.type == 'output') {
    selectMenu = outputSelect;
    // remove the item that's disconnected:
    outputDevices.splice(outputDevices.indexOf(midiItem), 1);
  }

  // clear the message listener:
  midiItem.onmidimessage = null;

  // delete the item from the menu:
  for (let i = 0; i < selectMenu.options.length; i++) {
    if (selectMenu.options[i].innerHTML === midiItem.name) {
      selectMenu.options[i].remove();
    }
  }
}

// select the current input or output from the select menus:
function selectInput(evt) {
  let selected = evt.target.selectedIndex;
  // iterate over the list of devices:
  for (let i of inputDevices) {
    if (i.name === inputSelect.options[selected].innerHTML) {
      currentInput = i;
    }
  }
  // if they chose the default position, clear the current input:
  if (inputSelect.options[selected] == 0) {
    currentInput = null;
  }
}

function selectOutput(evt) {
  let selected = evt.target.selectedIndex;
  // iterate over the list of devices:
  for (let o of outputDevices) {
    if (o.name === outputSelect.options[selected].innerHTML) {
      currentOutput = o;
    }
  }
  // if they chose the default position, clear the current output:
  if (outputSelect.options[selected] == 0) {
    currentOutput = null;
  }
}

// MIDI message listener function:
function getMIDIMessage(message) {
  // if the message came from a device other than the current input, you're done:
  if (currentInput !== message.currentTarget) return;

  // print the message (print the MIDI bytes as hexadeciimal values):
  messageDiv.innerHTML = "MIDI message:"
  for (var i = 0; i < message.data.length; i++) {
    messageDiv.innerHTML += " 0x" + message.data[i].toString(16);
  }

  // if the message is intended for the current output, send it there:
  if (currentOutput != null) {
    currentOutput.send(message.data);
  }

  // if connected to MQTT, send message as MQTT message:
  if (client.isConnected()) {
    sendMqttMessage(message.data.buffer);
  }
}