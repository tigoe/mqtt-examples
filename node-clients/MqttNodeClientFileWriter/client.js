/*
   MQTT Node.js client writes to filesystem. 
  Connects to shiftr.io's public broker, and
  writes any messages received to a file called
  data.txt

  created 10 Apr 2021
  modified 25 Feb 2023
  by Tom Igoe
*/

// include the libraries:
const mqtt = require('mqtt');
const fs = require('fs');
const { time } = require('console');
const { validateHeaderName } = require('http');
const { type } = require('os');

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


// client options:
const options = {
  // add a random number for a unique clientId:
  clientId: 'nodeClient-' + Math.floor(Math.random() * 1000000),
  username: 'public',
  password: 'public',
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000
}
// topic and message payload:
let myTopic = 'conndev/#';
let payload;

let activeClients = new Array();
let sendInterval = 5;
let saveInterval = 250;

// connect handler:
function setupClient() {
  console.log('client connected');
  client.subscribe(myTopic);
}

// new message handler:
function readMqttMessage(topic, message, packet) {
  // // make a timestamp string:
  let now = new Date();
  // deal with the topic:
  let project = {};
  let subTopics = topic.split('/');
  // first subTopic is the creator name:
  project.creator = subTopics[1];
  // if there's a second subTopic, that's the data label:
  let dataLabel = subTopics[2];
  // if it's empty, use the label 'data':
  if (!dataLabel) dataLabel = 'data';
  project.timeStamp = now.toISOString();

  let known = false;
  // scan the list if currently active clients:
  for (c of activeClients) {
    // if one matches, use it as the current client:
    if (c.creator == project.creator) {
      // assume that if two messages came in within
      // sendInterval ms of each other, that they
      // were all read at one time:
      if (now - new Date(c.timeStamp) < sendInterval) {
        project = c;
        known = true;
      }
    }
  }
  // if there's no matching active client, 
  // add this one to the array:
  if (!known) {
    activeClients.push(project);
  }

  // see if the message parses validly:
  try {
    let result = JSON.parse(message.toString());
    // if it parses, it's JSON or a valid number or array. 
    // if it's not an object:
    if (typeof result != 'object') {
      project[dataLabel] = result;
    } else {
      // if JSON, Extract the object properties
      // and put them in the project object: 
      for (i in result) {
        project[i] = result[i];
      }
    }
  } catch (err) {
    // if it fails parsing, just put the string in the dataLabel property:
    project[dataLabel] = message.toString();
  }
}


function saveData(data) {
  // get the path to the data file: 
  let filePath = __dirname + '/data.json';

  // this function is called by by the writeFile and appendFile functions 
  // below:
  function fileWriteResponse() {
    // once you've written, remove it:
    activeClients.splice(activeClients.indexOf(data));
  }
  /* 
    write to the file asynchronously. The third parameter of 
    writeFile is the callback function that's called when
    you've had a successful write. 
  */
  fs.exists(filePath, function (exists) {
    if (exists) {
      fs.appendFile(filePath, JSON.stringify(data) + '\n', fileWriteResponse);
    } else {
      fs.writeFile(filePath, JSON.stringify(data) + '\n', fileWriteResponse);
    }
  });
}
/*
  Because MQTT messages on multiple subtopics can come in
  asynchronously, you never know if you're getting a whole
  set of records or not. 
*/
function saveActiveRecords() {
  let now = new Date();
  // scan the list of active clients:
  for (c of activeClients) {
    // if this record is older than the saveInterval, it's probably complete.
    // save it to the file:
    if (now - new Date(c.timeStamp) > saveInterval) {
      saveData(c);
    }
  }
}
//////////////////////////

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on('connect', setupClient);
client.on('message', readMqttMessage);
// set an interval for saving any active clients:
setInterval(saveActiveRecords, saveInterval);