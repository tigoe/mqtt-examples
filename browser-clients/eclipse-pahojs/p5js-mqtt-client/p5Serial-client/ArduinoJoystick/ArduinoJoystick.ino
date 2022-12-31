/*
  A Joystick client
  Reads a joystick whose two potentiometers are connected
  to analog 0 and analog 1.
*/

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
}

void loop()
{
  int buttonState = digitalRead(2);
  // read analog 0, convert to a byte:
  int x = analogRead(A0) / 4;
  // 1ms delay to stabilize ADC:
  delay(1);
  // read analog 1, convert to a byte:
  int y = analogRead(A1) / 4;
  // send values out serial port as a JSON string,
  // {"x":xPos,"y":yPos,"button":buttonState}
  Serial.print("{\"x\":");
  Serial.print(x);
  Serial.print(",\"y\":");
  Serial.print(y);
  Serial.print(",\"button\":");
  Serial.print(buttonState);
  Serial.println("}");
}
