
void setup() {
  Serial.begin(9600);
  
  pinMode(6, INPUT_PULLUP); // enabling internal pull-up resistor
  pinMode(5, OUTPUT);
}

void loop() {
  int val = digitalRead(2);
  Serial.println(val);

  if (val == LOW) {
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
  }
}
