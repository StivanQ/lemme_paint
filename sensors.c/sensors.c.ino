#define ECHO1 2
#define TRIG1 3

#define ECHO2 6
#define TRIG2 7

#define ECHO_PIN 0
#define TRIG_PIN 1

// defines variables
long duration1; // variable for the duration of sound wave travel
int distance1; // variable for the distance measurement

long duration2; // variable for the duration of sound wave travel
int distance2; // variable for the distance measurement

unsigned long count = 0;

void setup() {
  pinMode(TRIG1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ECHO1, INPUT); // Sets the echoPin as an INPUT

  pinMode(TRIG2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ECHO2, INPUT); // Sets the echoPin as an INPUT
  
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
}

void readSensor(const unsigned char sensor_number) {
   const unsigned int echo_pin = sensor_number * 2 + ECHO_PIN;
   const unsigned int trig_pin = sensor_number * 2 + TRIG_PIN;

   digitalWrite(trig_pin, LOW);
   delayMicroseconds(2);
   digitalWrite(trig_pin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trig_pin, LOW);

   unsigned long duration = pulseIn(echo_pin, HIGH);
//   Serial.print(duration);
   unsigned long distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  Serial.print("Sensor:[");
  Serial.print(sensor_number);
  Serial.print("]: ");
  Serial.print(distance);
  Serial.print(" cm");
}

void loop() {
  unsigned long ts1 = micros();
  readSensor(1);

  unsigned long ts2 = micros();

//  Serial.println(ts2 - ts1);
//  delayMicroseconds(30000);
  readSensor(3);
  Serial.println("");
//  delayMicroseconds(30000);
  count++;
//  delay(100);
}
