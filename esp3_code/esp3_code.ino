#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <time.h>

// WiFi credentials
const char* ssid = "wota_wifi"; // SSID WiFi 
const char* password = "bayarsek"; // password WiFi 

// Firebase credentials
const char* firebaseHost = "smarthome-efa61-default-rtdb.firebaseio.com"; // URL database tanpa "https://"
const char* firebaseApiKey = "AIzaSyDJiZBKm8HW5nRUmcYBGSQ9Ue03A2DwhE0";

// Firebase objects
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
FirebaseData firebaseData;

// NTP server details
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 7; // GMT+7 for WITA
const int daylightOffset_sec = 0;

// Pin Definitions
#define LAMP1_PIN D3
#define LAMP2_PIN D4
#define DHT_PIN D1
#define BUZZER_PIN D7
#define GAS_SENSOR_PIN A0

// Thresholds
const int gasThreshold = 600;
const float suhuThreshold = 33.0;

// Servo setup
Servo servoGerbang;
Servo servoKunci;

// Lamp status
bool lampu1Status = false;
bool lampu2Status = false;
bool autoLampuEnabled = false;

// DHT sensor
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Web server
ESP8266WebServer server(80);

// Helper functions
void setupNTP() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("NTP initialized.");
}

String getFormattedTime() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("Failed to obtain time");
    return "00:00:00"; // Fallback time
  }
  char timeString[50];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
  return String(timeString);
}

void updateFirebase(float suhu, float kelembapan, int gasValue, const String& time) {
  if (Firebase.RTDB.setString(&firebaseData, "/data/suhu", String(suhu))) {
    Serial.println("Data suhu berhasil dikirim ke Firebase");
  } else {
    Serial.println("Gagal mengirim data suhu ke Firebase");
  }

  if (Firebase.RTDB.setString(&firebaseData, "/data/kelembapan", String(kelembapan))) {
    Serial.println("Data kelembapan berhasil dikirim ke Firebase");
  } else {
    Serial.println("Gagal mengirim data kelembapan ke Firebase");
  }

  if (Firebase.RTDB.setString(&firebaseData, "/data/gas", String(gasValue))) {
    Serial.println("Data gas berhasil dikirim ke Firebase");
  } else {
    Serial.println("Gagal mengirim data gas ke Firebase");
  }

  if (Firebase.RTDB.setString(&firebaseData, "/data/waktu", time)) {
    Serial.println("Data waktu berhasil dikirim ke Firebase");
  } else {
    Serial.println("Gagal mengirim data waktu ke Firebase");
  }
}

void controlLampu(int lampPin, bool& status, int brightness = 0) {
  if (status) {
    analogWrite(lampPin, brightness); // Hanya di pin yang mendukung PWM
  } else {
    digitalWrite(lampPin, LOW);
  }
}

void checkGasAndTemperature(float suhu, int gasValue) {
  if (suhu > suhuThreshold || gasValue > gasThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);  // Bunyi buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);   // Matikan buzzer
  }
}

void handleWebRequests() {
  server.on("/ping", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    server.send(200, "text/plain", "pong");
  });

  server.on("/lampu1/on", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    lampu1Status = true;
    server.send(200, "text/plain", "Lampu 1 ON");
  });

  server.on("/lampu1/off", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    lampu1Status = false;
    server.send(200, "text/plain", "Lampu 1 OFF");
  });

  server.on("/lampu2/on", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    lampu2Status = true;
    server.send(200, "text/plain", "Lampu 2 ON");
  });

  server.on("/lampu2/off", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    lampu2Status = false;
    server.send(200, "text/plain", "Lampu 2 OFF");
  });

  server.on("/gerbang/open", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    servoGerbang.write(180);
    delay(1000);  // Memberi waktu untuk gerbang terbuka
    server.send(200, "text/plain", "Gerbang terbuka");
  });

  server.on("/gerbang/close", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    servoGerbang.write(0);
    delay(1000);  // Memberi waktu untuk gerbang tertutup
    server.send(200, "text/plain", "Gerbang tertutup");
  });

  server.on("/kunci/open", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    servoKunci.write(180);
    delay(1000);  // Memberi waktu untuk kunci terbuka
    server.send(200, "text/plain", "Kunci terbuka");
  });

  server.on("/kunci/close", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Menambahkan header CORS
    servoKunci.write(0);
    delay(1000);  // Memberi waktu untuk kunci tertutup
    server.send(200, "text/plain", "Kunci tertutup");
  });

server.on("/lampu1/brightness", []() {
    Serial.println("Permintaan brightness diterima.");
    if (server.hasArg("value")) {
        int brightness = server.arg("value").toInt();
        Serial.printf("Brightness: %d\n", brightness);
        // Logika kontrol lampu
    } else {
        Serial.println("Parameter 'value' tidak ditemukan.");
    }
});

server.on("/lampu2/brightness", []() {
    Serial.println("Permintaan brightness diterima.");
    if (server.hasArg("value")) {
        int brightness = server.arg("value").toInt();
        Serial.printf("Brightness: %d\n", brightness);
        // Logika kontrol lampu
    } else {
        Serial.println("Parameter 'value' tidak ditemukan.");
    }
});


  server.begin();
}


void setup() {
  Serial.begin(115200);
  dht.begin();

  // Setup WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("Terhubung ke WiFi!");

  // Setup Firebase
  firebaseConfig.host = firebaseHost;
  firebaseConfig.api_key = firebaseApiKey;
  firebaseAuth.user.email = "klepon123@gmail.com"; // Email Firebase Authentication
  firebaseAuth.user.password = "123456"; // Password Firebase Authentication
  Firebase.begin(&firebaseConfig, &firebaseAuth);

  // Setup NTP
  setupNTP();

  // Setup Pins
  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);

  // Setup Servo
  servoGerbang.attach(D5);
  servoGerbang.write(0);
  servoKunci.attach(D6);
  servoKunci.write(0);

  // Setup Web Server
  handleWebRequests();

  Serial.println("Setup selesai.");
}

void loop() {
  server.handleClient();

  // Read sensors
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  int gasValue = analogRead(GAS_SENSOR_PIN);

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Error membaca DHT sensor!");
    return; // Hindari melanjutkan jika pembacaan DHT gagal
  }

  String time = getFormattedTime();
  updateFirebase(suhu, kelembapan, gasValue, time);
  Serial.printf("Suhu: %.2f, Kelembapan: %.2f, Gas: %d\n", suhu, kelembapan, gasValue);

  // Control lampu
  controlLampu(LAMP1_PIN, lampu1Status, 1023);
  controlLampu(LAMP2_PIN, lampu2Status, 1023);

  // Check alarms
  checkGasAndTemperature(suhu, gasValue);

  delay(5000); // 1 detik untuk pembacaan ulang
}


