/*
   MQTT Node.js client writes to filesystem. 
  Connects to a broker, subscribes to a broad range of topics,
  and writes any messages received to a local file called
  data.json

  created 10 Apr 2021
  modified 3 Nov 2024
  by Tom Igoe
*/

// include the libraries:
const mqtt = require('mqtt');
const fs = require('fs');

// All these brokers work with this code. 
// Uncomment the one you want to use. 

////// emqx. Works in both basic WS and TLS WS:
// const broker = 'wss://broker.emqx.io:8084/mqtt'
// const broker = 'ws://broker.emqx.io:8083/mqtt'

//////// shiftr.io desktop client. 
// Fill in your desktop IP address for localhost:
// const broker = 'ws://localhost:1884';     

//////// shiftr.io, using username and password 
// (see options variable below):
// const broker = 'wss://public.cloud.shiftr.io';

//////// test.mosquitto.org, uses no username and password:
// const broker = 'wss://test.mosquitto.org:8081';

// or use  your own:
const broker = 'mqtt://mysite.com';

  // the path to the data file: 
  let filePath = __dirname + '/data.json';

// client options:
const options = {
  // add the current epoch time for a unique clientId:
  clientId: 'nodeClient-' + Date.now(),
  username: 'user',
  password: 'password!',
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000
}
// topic:
let myTopic = '#';

// connect handler:
function setupClient() {
  console.log('client connected');
  client.subscribe(myTopic);
}

// new message handler:
function readMqttMessage(topic, message, packet) {
  // // make a timestamp string:
  let now = new Date();
  // create a new record from the topic and subtopics:
  let record = {};
  let subTopics = topic.split('/');
  // assume first subTopic is the creator name:
  record.creator = subTopics[0];
  // if there's a second subTopic, assume that's the data label:
  let dataLabel = subTopics[1];
  // if it's empty, use the label 'data':
  if (!dataLabel) dataLabel = 'data';
  // make a timestamp:
  record.timeStamp = now.toISOString();

  // see if the message parses as valid JSON:
  try {
    let data = JSON.parse(message.toString());
    // if it parses, it's JSON or a valid number or array. 
    // if it's not, just put it in the data category as is:
    if (typeof data != 'object') {
      record[dataLabel] = data;
    } else {
      // if JSON, Extract the object properties
      // and put each in the record as its own property: 
      for (i in data) {
        record[i] = data[i];
      }
    }
  } catch (err) {
    // if it fails parsing, just put the string in the dataLabel property:
    record[dataLabel] = message.toString();
  }
  // save to the file:
  saveData(record);
}


function saveData(data) {
  // this function is called by  the writeFile and appendFile functions 
  // below:
  function fileWriteResponse() {
    console.log("wrote to file at: " + data.timeStamp);
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

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on('connect', setupClient);
client.on('message', readMqttMessage);