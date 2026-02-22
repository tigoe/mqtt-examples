/*
  MQTT Client sender/receiver with will and NTP time calc

  This sketch demonstrates an MQTT client that connects to a broker, subscribes to a topic,
  and both listens for messages on that topic and sends messages to it.
  When the client receives a message, it parses it, and PWMs the built-in LED.
  
  This client also has a last will and testament property. 
  With each reading sent, it  updates the will topic with 
  the current network time. If another reading does not 
  show up within the keepAlive interval, the broker 
  will publish the will topic.

  The network time code is based on the  WiFi library's
  UdpNtpClient, originally written by Michael Margolis, and
  modified by Tom Igoe:
  https://github.com/arduino-libraries/WiFi/blob/master/examples/WiFiUdpNtpClient/WiFiUdpNtpClient.ino

  This sketch uses https://public.cloud.shiftr.io as the MQTT broker, but others will work as well.
  See https://tigoe.github.io/mqtt-examples/#broker-client-settings for connection details. 

Libraries used:
  * http://librarymanager/All#WiFiNINA or
  * http://librarymanager/All#WiFi101 
  * http://librarymanager/All#ArduinoMqttClient
 
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 21 Feb 2026
  by Tom Igoe
*/

#include <WiFiNINA.h>  // use this for Nano 33 IoT, MKR1010, Nano RP2040 Connect
// #include <WiFi101.h>    // use this for MKR1000
#include <ArduinoMqttClient.h>
#include <WiFiUdp.h>  // used to send and receive NTP messages
#include "arduino_secrets.h"

// initialize WiFi connection as SSL.
// If you are not using MQTTS, use WiFiClient:
WiFiSSLClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 8883;
char topic[] = "aardvarks";
String clientID = "arduinoMqttClient-";
// properties for the MQTT last will and testament:
String willPayload;
const bool willRetain = true;
const int willQos = 1;
String willTopic = String(topic) + "/last_seen";

// last time the client sent a message, in ms:
long lastTimeSent = 0;
// message sending interval:
int interval = 10 * 1000;
// keepAlive interval for the will:
int keepAliveInterval = 60 * 1000;
// connection timeout for the MQTT client:
int connectionTimeout = 30 * 1000;

// details for the UDP/NTP transmissions:
const unsigned int localPort = 2390;  // local port to listen for UDP packets
const int NTP_PACKET_SIZE = 48;       // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];   //buffer to hold incoming and outgoing packets
// A UDP instance to send and receive packets over UDP
WiFiUDP Udp;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // wait for serial monitor to open:
  if (!Serial) delay(3000);
  // initialize built-in LED:
  pinMode(LED_BUILTIN, OUTPUT);
  // connect to WiFi:
  connectToNetwork();
  // make the clientID unique by adding the last three digits of the MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 3; i < 6; i++) {
    clientID += String(mac[i], HEX);
  }

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  // if needed, login to the broker with a username and password:
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  // after keepAliveInterval, the broker will publish the will:
  mqttClient.setKeepAliveInterval(keepAliveInterval);
  // after connectionTimeout the client will produce a connection error:
  mqttClient.setConnectionTimeout(connectionTimeout);

  // start the UDP port for the NTP time setting:
  Udp.begin(localPort);
}

void loop() {
  // if you disconnected from the network, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
    // skip the rest of the loop until you are connected:
    return;
  }

  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("attempting to connect to broker");
    connectToBroker();
  }

  // poll for new messages from the broker:
  mqttClient.poll();

  // once every interval, send a message:
  if (millis() - lastTimeSent > interval) {
    // get a random analog reading, divide by 4 to set range to 0-255:
    int sensorReading = analogRead(A0) / 4;
    if (mqttClient.connected()) {
      // start a new message on the topic:
      mqttClient.beginMessage(topic);
      // print the body of the message:
      mqttClient.print(sensorReading);
      // send the message:
      mqttClient.endMessage();

      // send a serial notification:
      Serial.print("published a message: ");
      Serial.println(sensorReading);
      // timestamp this message:
      lastTimeSent = millis();
    }
  }
}

boolean connectToBroker() {
  // before connecting, update the update the will topic
  // with the last successful timestamp:
  updateWill();
  // Try to connect. if the connection fails:
  if (!mqttClient.connect(broker, port)) {
    // print out the error message:
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    // return that you're not connected:
    return false;
  }

  // set the message receive callback:
  mqttClient.onMessage(onMqttMessage);
  // subscribe to a topic:
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  mqttClient.subscribe(topic);

  // once you're connected, you
  // return that you're connected:
  return true;
}


void updateWill() {
  // you can't update the will if connected,
  // so if you are connected, quit this function:
  if (mqttClient.connected()) return;

  /* get the seconds since 1/1/1970 from a network time server
   For more on time setting, see the WIFiUdpNtpClient example at:
    https://github.com/arduino-libraries/WiFi/blob/master/examples/WiFiUdpNtpClient/WiFiUdpNtpClient.ino
   or the Clock Club examples at:
   https://itpnyu.github.io/clock-club/
   */
  // get the network time as a single number:
  // willPayload = String(getNetworkTime());
  // alternately, you can send the time as an ISO8601 string:
  willPayload = getISOString(getNetworkTime());
  Serial.println(willPayload);

  // set the will message:
  mqttClient.beginWill(willTopic, willTopic.length(), willRetain, willQos);
  // send it to the broker:
  mqttClient.print(willPayload);
  // close the connection:
  mqttClient.endWill();
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic ");
  Serial.print(mqttClient.messageTopic());
  Serial.print(", length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  String incoming = "";
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    incoming += (char)mqttClient.read();
  }
  // convert the incoming string to an int so you can use it:
  int result = incoming.toInt();
  // use the result to dim the builtin LED:
  if (result > 0) {
    analogWrite(LED_BUILTIN, result);
  }
  // print the result:
  Serial.println(result);
}

void connectToNetwork() {
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }

  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());
}


unsigned long getNetworkTime() {
  unsigned long epoch = 0;
  sendNTPpacket();
  delay(2000);
  // assumes a packet has arrived:
  if (Udp.parsePacket()) {
    // Read the reply from the NTP server:
    Udp.read(packetBuffer, NTP_PACKET_SIZE);
    //the timestamp starts at byte 40 of the received packet
    // and is four bytes, or two words, long.
    // First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // next combine the four bytes (two words)
    // into a long integer this is NTP time
    // (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    //  convert NTP time into Unix time:
    // Unix time starts on Jan 1 1970.
    // That's 2208988800 seconds from Jan 1 1900:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;
    // Now you have the  Unix time epoch (secs since 1/1/1970).
    return epoch;
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket() {
  // Form an NTP packet. The details of this packet format
  // can be found at
  // https://en.wikipedia.org/wiki/Network_Time_Protocol#NTP_packet_header_format:
  // there are more bytes in the NTP packet header,
  // and this is a very rough calculation,
  // but you can get a good time response
  // with just these four bytes set:
  // First byte contains Leap Indicator, Version, Mode, in binary format:
  packetBuffer[0] = 0b11100011;
  // Next byte indicates the distance from the reference clock.
  // 0 = invalid:
  packetBuffer[1] = 0;
  // next byte is the max. polling interval
  // between successive messages, in log₂(seconds).
  // Typical range is 6 to 10:
  packetBuffer[2] = 6;
  // Nect byte is the signed log₂(seconds)
  // of system clock precision (e.g., –18 ≈ 1 microsecond):
  packetBuffer[3] = 0xEC;
  // zero out the rest of the array:
  for (int b = 4; b < NTP_PACKET_SIZE; b++) {
    packetBuffer[b] = 0;
  }

  // send a packet to port 123 requesting a timestamp:
  Udp.beginPacket("pool.ntp.org", 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


String getISOString(long epoch) {
  //ISO8601 time is the standard time format.
  // see https://en.wikipedia.org/wiki/ISO_8601 for details.
  // the string looks like this:
  String result = "YYYY-MM-DD hh:mm:ss";

  // calculate the hour (86400 secs per day):
  int hour = (epoch % 86400L) / 3600;
  // calculate the minute (3600 per minute):
  int minute = (epoch % 3600) / 60;
  // calculate the second:
  int second = epoch % 60;
  // a variable for the days in a given year:
  unsigned int daysPerYear = 365;
  // the day and month of this particular epoch:
  unsigned int day;
  unsigned int month = 0;
  // days in this epoch. seconds per day = 846400:
  unsigned long days = epoch / 86400UL;
  // year in this epoch. Minumum is 1970:
  unsigned int year = 1970;
  // days in the months:
  unsigned int monthLengths[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
  };
  // add a day to count for 0-indexing of days:
  unsigned int daysLeft = days + 1;
  // count down the days to get the month and year:
  while (daysLeft >= daysPerYear) {
    // first, check if it's a leap year:
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
      monthLengths[1] = 29;  // Feb. gets an extra day
      daysPerYear = 366;
    } else {
      monthLengths[1] = 28;
      daysPerYear = 365;
    }

    // next, subtract this years' length
    // from the total number of days in the epoch:
    daysLeft -= daysPerYear;
    // next, increment the year:
    year++;

    // when there are less than a year of days,
    // calculate the current month:
    if (daysLeft < daysPerYear) {
      // if the days left is greater than current month length,
      // subtract the length of the month, then increment the month:
      while (daysLeft > monthLengths[month]) {
        daysLeft -= monthLengths[month];
        month++;
      }
    }
  }



  // add 1 to month to adjust (months start with 1):
  month++;
  // day of month = daysLeft + 1 (days start with 1):
  day = daysLeft++;
  // replace the placeholders in the ISO8601 String
  // with the actual values:
  result.replace("YYYY", String(year));
  result.replace("MM", numToString(month));
  result.replace("DD", numToString(day));
  result.replace("hh", numToString(hour));
  result.replace("mm", numToString(minute));
  result.replace("ss", numToString(second));
  // return the string:
  return result;
}

// this function adds leading zeroes to any
// single-digit numbers, and converts them to strings:
String numToString(int number) {
  String resultString = String(number);
  if (number < 10) {
    resultString = "0" + resultString;
  }
  return resultString;
}
