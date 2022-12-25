// Library yang diperlukan
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN D4
#define Fun D3
#define Hum D5
#define LedM D6
#define Buzz D7
#define LedB D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Harus diisi
#define FIREBASE_HOST "https://iot-mikro-f4f7f-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "spO0FjYKk4ocAdWHWpoOCXWsUgA0yGRr1pPXGRnK"
#define WIFI_SSID "Hp"
#define WIFI_PASSWORD "Corei5HP14"

// mendeklarasikan objek data dari FirebaseESP8266
FirebaseData firebaseData;

void setup() {

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  SPI.begin();
  dht.begin();
  pinMode(Fun, OUTPUT);
  pinMode(Hum, OUTPUT);
  pinMode(LedM, OUTPUT);
  pinMode(LedB, OUTPUT);
  pinMode(Buzz, OUTPUT);
  digitalWrite(Fun, HIGH);
  digitalWrite(Hum, HIGH);
  // Koneksi ke Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  // Sensor DHT11 membaca suhu dan kelembaban
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if ((h < 70) || (t > 31)) {
    digitalWrite(LedM, HIGH);
    digitalWrite(Buzz, HIGH);
    digitalWrite(LedB, LOW);
    digitalWrite(Hum, LOW);

    delay(5000);
    digitalWrite(Fun, LOW);
    // Menampilkan suhu dan kelembaban pada serial monitor dan LCD
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("DARURAT");
    delay(50);
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(t);
    lcd.println("*C");
    lcd.setCursor(9, 1);
    lcd.print("H:");
    lcd.print(h);
    lcd.println("%");
    delay(50);
    Serial.println("Sistem Pendingin Menyala");
    Serial.print("Suhu: ");
    Serial.print(t);
    Serial.println("*C");
    Serial.print("Kelembaban: ");
    Serial.print(h);
    Serial.println(" %");
    Serial.println();
  }
  else {
    digitalWrite(LedM, LOW);
    digitalWrite(LedB, HIGH);
    digitalWrite(Buzz, HIGH); delay(80);    digitalWrite(Buzz, LOW);     delay(90);
    digitalWrite(Hum, HIGH);
    digitalWrite(Fun, HIGH);

    // Menampilkan suhu dan kelembaban pada serial monitor dan LCD
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("NORMAL");
    delay(50);
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(t);
    lcd.println("*C");
    lcd.setCursor(9, 1);
    lcd.print("H:");
    lcd.print(h);
    lcd.println("%");
    delay(50);
    Serial.println("Sistem Pendingin Mati");
    Serial.print("Suhu: ");
    Serial.print(t);
    Serial.println("*C");
    Serial.print("Kelembaban: ");
    Serial.print(h);
    Serial.println(" %");
    Serial.println();
  }

  // Memeriksa apakah sensor berhasil mambaca suhu dan kelembaban
  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca sensor DHT11");
    return;
  }

  // Memberikan status suhu dan kelembaban kepada firebase
  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/suhu", t)) {
    Serial.println("Suhu terkirim");
  } else {
    Serial.println("Suhu tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban", h)) {
    Serial.println("Kelembaban terkirim");
    Serial.println();
  } else {
    Serial.println("Kelembaban tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  delay(1000);
}
