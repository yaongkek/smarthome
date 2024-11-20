// Arduino Code
#include <Servo.h>
#include <DHT.h>

// Pin konfigurasi
#define LDR_PIN A0
#define GAS_PIN A1
#define DHT_PIN 2
#define LED1_PIN 3
#define LED2_PIN 4
#define BUZZER_PIN 5
#define SERVO_PIN 6

// DHT konfigurasi
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Servo
Servo servo;

// Variabel
int ldrValue;
int gasValue;
float temperature;
float humidity;

// Threshold untuk LDR
#define LDR_THRESHOLD 50

void setup() {
  Serial.begin(9600); // Untuk komunikasi dengan ESP8266
  dht.begin();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  servo.attach(SERVO_PIN);
  servo.write(0); // Gerbang tertutup secara default

  Serial.println("Arduino siap.");
}

void loop() {
  // Membaca data sensor
  ldrValue = analogRead(LDR_PIN);
  gasValue = analogRead(GAS_PIN);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Kontrol otomatis LED dan Servo berdasarkan LDR
  if (ldrValue < LDR_THRESHOLD) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    servo.write(90);
  } else {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    servo.write(0);
  }

  // Kirim data ke ESP8266
  String sensorData = "LDR:" + String(ldrValue) +
                     ",Temp:" + String(temperature) +
                     ",Gas:" + String(gasValue);
  Serial.println(sensorData);

  // Cek perintah dari ESP8266
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "OPEN") {
      servo.write(90);
    } else if (command == "CLOSE") {
      servo.write(0);
    } else if (command == "LED_ON") {
      digitalWrite(LED1_PIN, HIGH);
      digitalWrite(LED2_PIN, HIGH);
    } else if (command == "LED_OFF") {
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
    }
  }

  // Kontrol buzzer
  if (gasValue > 250 || temperature > 33) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500);
}