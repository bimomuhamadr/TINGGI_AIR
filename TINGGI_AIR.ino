#include <WiFi.h>
#include <HTTPClient.h>
#include "CTBot.h";
CTBot myBot;
const char* ssid = "ASUSX555Q";
const char* pass = "welcomestark";
const char* host = "tinggiair.promonissanbogor.com";
//const char* host = "192.168.137.1";
String token = "1936792579:AAFfoU2RpxCr5BaJi-W-7ai0tNsJlnK5lW4";
const int id = 1255034361; // ID ROBY
// const int id = 1062799651; // ID Bimo
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <NewPing.h>
#define TRIGGER_PIN  12
#define ECHO_PIN     14
#define MAX_DISTANCE 30
#define buzzer 23
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
long ultra;
int In1 = 26, In2 = 27;
int flag = false;
int flag2 = false;
int waktubuzzer1 = 0, buzzer1, flagbuzzer1 = false;
int datakirim = 0;
int period = 200;
unsigned long time_now = 0;
int flagt1 = false, flagt2 = false, flagt3 = false, flagt4 = false;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //-----------------------------------------//
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);
  if (myBot.testConnection()) {
    Serial.println("Koneksi Bot Telegram Berhasil");
  } else {
    Serial.println("Koneksi Bot Telegram Gagal");
  }
  //-----------------------------------------//
  lcd.setCursor(0, 0);
  lcd.print("Connecting Wifi ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Connecting to  ");
  lcd.setCursor(0, 1);
  lcd.print(host);
  delay(2000);
  lcd.clear();
  //-----------------------------------------//
  lcd.setCursor(0, 0);
  lcd.print("   MONITORING   ");
  lcd.setCursor(0, 1);
  lcd.print("   TINGGI AIR   ");
  delay(2000);
  lcd.clear();
  //-----------------------------------------//
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  //-----------------------------------------//
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}

void loop() {
  if (millis() > time_now + period) {
    time_now = millis();
    //baca pesan masuk dari telegram
    TBMessage msg;
awal :
    SensorUltra();
    Kontrol();
    Serial.print("ultra : "); Serial.print(ultra); Serial.println(" cm");
    lcd.setCursor(0, 0);
    lcd.print("  Status Air :  ");
    lcd.setCursor(0, 1);

    if (ultra >= 1 && ultra <= 3) {
      lcd.print("      AWAS !    ");
      flagt2 = false;
      flagt3 = false;
      flagt4 = false;
      if (flagt1 == false) {
        flagt1 = true;
        myBot.sendMessage(id, "Ketinggian Air : AWAS ! | website : tinggiair.promonissanbogor.com");
      }
      buzzer1 = 1;
    } else if (ultra >= 4 && ultra <= 7) {
      lcd.print("     SIAGA !    ");
      flagt1 = false;
      flagt3 = false;
      flagt4 = false;
      if (flagt2 == false) {
        flagt2 = true;
        myBot.sendMessage(id, "Ketinggian Air : SIAGA ! | website : tinggiair.promonissanbogor.com");
      }
    } else if (ultra >= 8 && ultra <= 15) {
      lcd.print("    WASPADA !   ");
      flagt1 = false;
      flagt2 = false;
      flagt4 = false;
      if (flagt3 == false) {
        flagt3 = true;
        myBot.sendMessage(id, "Ketinggian Air : WASPADA | website : tinggiair.promonissanbogor.com");
      }
    } else {
      lcd.print("     NORMAL !   ");
      flagt1 = false;
      flagt2 = false;
      flagt3 = false;
      if (flagt4 == false) {
        flagt4 = true;
        myBot.sendMessage(id, "Ketinggian Air : NORMAL | website : tinggiair.promonissanbogor.com");
      }
    }

    if (buzzer1 == 1 && waktubuzzer1 < 5) {
      if (flagbuzzer1 == false) {
        flagbuzzer1 = true;
        digitalWrite(buzzer, HIGH);
        delay(50);
      } else {
        digitalWrite(buzzer, LOW);
        flagbuzzer1 = false;
        delay(50);
      }
    }
    if (waktubuzzer1 == 5 && buzzer1 == 1) {
      digitalWrite(buzzer, LOW);
      buzzer1 = 0;
      waktubuzzer1 = 0;
      goto awal;
    }
    if (buzzer1 == 1) {
      waktubuzzer1++;
    }

    datakirim++;
    if (datakirim == 5) {
      KirimData();
      datakirim = 0;
    }
    
  }
}

void KirimData() {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection Failed");
    return;
  }
  String Link;
  HTTPClient http;
  Link = "http://" + String(host) + "/tambah_proses.php?ultra=" + String(ultra);
  http.begin(Link);
  http.GET();
  String respon = http.getString();
  Serial.println(Link);
  //  Serial.println(respon);
  http.end();
}

void Kontrol() {
  String LinkKontrol;
  HTTPClient httpKontrol;
  LinkKontrol = "http://" + String(host) + "/bacapintuair.php";
  httpKontrol.begin(LinkKontrol);
  httpKontrol.GET();
  String responseKontrol = httpKontrol.getString();
  Serial.print("Pintu Air : "); Serial.println(responseKontrol);
  int pintuair = responseKontrol.toInt();
  httpKontrol.end();
  if (pintuair == 1) {
    flag2 = false;
    if (flag == false) {
      flag = true;
      myBot.sendMessage(id, "Pintu Air : TERBUKA | website : tinggiair.promonissanbogor.com");
      pintuBuka();
      delay(650);
      pintuDiam();
    } else {
      pintuDiam();
      delay(500);
    }
  } else if (pintuair == 0) {
    flag = false;
    if (flag2 == false) {
      flag2 = true;
      myBot.sendMessage(id, "Pintu Air : TERTUTUP | website : tinggiair.promonissanbogor.com");
      pintuTutup();
      delay(650);
      pintuDiam();
    } else {
      pintuDiam();
      delay(500);
    }
  }
}

void SensorUltra() {
  ultra = sonar.ping_cm();
}

void pintuBuka() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  delay(20);
}

void pintuTutup() {
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  delay(20);
}

void pintuDiam() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  delay(20);
}
