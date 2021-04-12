int lastSensorReading = 0;
int threshold = 5;

void setup() {
  // open serial port and set timeout for reading to 10ms:
  Serial.begin(9600);
  Serial.setTimeout(10);
  // init pin 9 as output:
  pinMode(9, OUTPUT);
}

void loop() {
  // read a sensor, get a range from 0-255:
  int sensor = analogRead(A0) / 4;

  // if it's changed enough, send it:
  if (abs(sensor - lastSensorReading) > threshold) {
    Serial.println(sensor);
  }
  // if there's serial to be read, read and parse
  // for an integer:
  if (Serial.available() > 0) {
    int intensity = Serial.parseInt();
    // if the result is > 0,
    // set the LED on pin 9 with it:
    if (intensity > 0) {
      analogWrite(9, intensity);
    }
  }
}
