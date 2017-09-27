
#define LED 13
#define XMIT 4

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(XMIT, OUTPUT);
}

void loop() {
  digitalWrite(XMIT, HIGH);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(XMIT, LOW);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(XMIT, HIGH);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(XMIT, LOW);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(XMIT, HIGH);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(XMIT, LOW);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(XMIT, HIGH);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(XMIT, LOW);
  digitalWrite(LED, LOW);
  delay(4000);
}

