/*
   MQTT Node.js client writes to filesystem. 
  Connects to shiftr.io's public broker, and
  writes any messages received to a file called
  data.txt

  created 10 Apr 2021
  modified 15 Nov 2022
  by Tom Igoe
*/

// include the libraries:
const mqtt = require('mqtt');
const fs = require('fs')

// the broker you plan to connect to. 
// transport options: 
// 'mqtt', 'mqtts', 'tcp', 'tls', 'ws', or 'wss':
const broker = 'mqtt://test.mosquitto.org';

// client options:
const options = {
  clientId: 'nodeClient',
  username: 'public',
  password: 'public',
  clean: true,
  connectTimeout: 4000,
  reconnectPeriod: 1000
}
// topic and message payload:
let myTopic = 'undnet/#';
let payload;

// connect handler:
function setupClient() {
  client.subscribe(myTopic);
}

// new message handler:
function readMqttMessage(topic, message) {
  // make a timestamp string:
  let now = new Date();
  let timeStamp = now.toISOString();
  // message is a Buffer, so convert to a string:
  let msgString = message.toString();
  // check if it's a valid JSON string:
  let msgData = isJsonString(msgString);
  if (msgData) {
      // if it is, add the timestamp and re-stringify:
    msgData.timeStamp = now;
    msgString = JSON.stringify(msgData);
  } else {
    // if not, add the timestamp after the first character:
    msgString = msgString.slice(0, 1) +
      '"timestamp": "' + timeStamp + '",' +
      msgString.slice(1, msgString.length - 1);
  }
// if the string doesn't end in a newline, add one:
  if (msgString.charAt(msgString.length - 1) != '\n') {
    msgString += '\n';
  }// dave the data:
  saveData(topic, msgString);
}

function isJsonString(thisString) {
  let result;
  try {
    result = JSON.parse(thisString);
  } catch (err) {
    result = false;
  }
  return result;
}


function saveData(topic, data) {
  // get the path to the data file: 
  let filePath = __dirname + '/data.txt';

  // this function is called by by the writeFile and appendFile functions 
  // below:
  function fileWriteResponse() {
    console.log("writing: " + topic + ": " + data);
  }
  /* 
    write to the file asynchronously. The third parameter of 
    writeFile is the callback function that's called when
    you've had a successful write. 
  */
  fs.exists(filePath, function (exists) {
    if (exists) {
      fs.appendFile(filePath, data, fileWriteResponse);
    } else {
      fs.writeFile(filePath, data, fileWriteResponse);
    }
  });
}
//////////////////////////

// make a client and connect:
let client = mqtt.connect(broker, options);
client.on('connect', setupClient);
client.on('message', readMqttMessage);
