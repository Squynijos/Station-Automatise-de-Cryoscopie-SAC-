#define RXD2 18
#define TXD2 19

#define GPS_BAUD 115200

HardwareSerial gpsSerial(2);
int counter = 0;

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);

}

void loop() {
  if (gpsSerial.available()) {
    // Read data and display it
    String message = gpsSerial.readStringUntil('\n');
    Serial.println("Received: " + message);
  }

}
