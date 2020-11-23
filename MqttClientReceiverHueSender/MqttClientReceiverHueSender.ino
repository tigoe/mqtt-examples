/*
  MQTT Client with a Philips Hue Client

  This sketch demonstrates an MQTT client that connects to a broker, subsrcibes to a topic,
  listens for messages on that topic and uses them to control a Philips Hue light 
  
  This sketch uses https://next.shiftr.io/try as the MQTT broker.

  the circuit:
  
  the arduino_secrets.h file:
  #define SECRET_SSID ""    // network name
  #define SECRET_PASS ""    // network password
  #define SECRET_MQTT_USER "public" // broker username
  #define SECRET_MQTT_PASS "public" // broker password

  created 27 Sep 2020
  updated 23 Nov 2020
  by Tom Igoe
*/

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

// initialize WiFi connection:
WiFiClient wifi;
MqttClient mqttClient(wifi);

// details for MQTT client:
char broker[] = "public.cloud.shiftr.io";
int port = 1883;
char topic[] = "lights";
char clientID[] = "arduinoHueClient";

// intensity of the Hue light:
int intensity = 0;

char hueHubIP[] = "192.168.0.3";  // IP address of the HUE bridge
String hueUserName = "huebridgeusername"; // hue bridge username
HttpClient httpClient = HttpClient(wifi, hueHubIP);


void setup() {
  // initialize serial:
  Serial.begin(9600);
 // initialize I/O pins: 
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize WiFi, if not connected:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // print IP address once connected:
  Serial.print("Connected. My IP address: ");
  Serial.println(WiFi.localIP());

  // set the credentials for the MQTT client:
  mqttClient.setId(clientID);
  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);

  // try to connect to the MQTT broker once you're connected to WiFi:
  while (!connectToBroker()) {
    Serial.println("attempting to connect to broker");
    delay(1000);
  }
  Serial.println("connected to broker");
}

void loop() {
  // turn on built-in LED when you are connected to WiFi:
  digitalWrite(LED_BUILTIN, WiFi.status());
  // if not connected to the broker, try to connect:
  if (!mqttClient.connected()) {
    Serial.println("reconnecting");
    connectToBroker();
    delay(2000);
  }

  // if a message comes in, read it:
  if (mqttClient.parseMessage() > 0) {
    Serial.print("Got a message on topic: ");
    Serial.println(mqttClient.messageTopic());
    // read the message:
    while (mqttClient.available()) {
      // convert numeric string to an int:
      int message = mqttClient.parseInt();
      Serial.println(message);
      // if the message matches client's number, set the LED to full intensity:
      if (message > 0 && message < 255) {
        intensity = message;
      }
    }
  }
  // if the LED is on:
  if (intensity > 0) {
     sendRequest(3, "bri", intensity);   // turn light on
  }
}

boolean connectToBroker() {
  // if the MQTT client is not connected:
  if (!mqttClient.connect(broker, port)) {
    // print out the error message:
    Serial.print("MOTT connection failed. Error no: ");
    Serial.println(mqttClient.connectError());
    // return that you're not connected:
    return false;
  }
  // once you're connected, you can proceed:
  mqttClient.subscribe(topic);
  // return that you're connected:
  return true;
}


void sendRequest(int light, String cmd, int value) {
  // make a String for the HTTP request path:
  String request = "/api/" + hueUserName;
  request += "/lights/";
  request += light;
  request += "/state/";

  String contentType = "application/json";

  // make a string for the JSON command:
  String hueCmd = "{\"" + cmd;
  hueCmd += "\":";
  hueCmd += String(value);
  hueCmd += "}";
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(request);
  Serial.print("JSON command to server: ");

  // make the PUT request to the hub:
  httpClient.put(request, contentType, hueCmd);
  
  // read the status code and body of the response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  Serial.println(hueCmd);
  Serial.print("Status code from server: ");
  Serial.println(statusCode);
  Serial.print("Server response: ");
  Serial.println(response);
  Serial.println();
}
