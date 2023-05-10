#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>
#include "WiFi.h"
boolean kondisikonek = false;
#define FIRMWARE_VERSION 2.0                                                       //versi program
#define UPDATE_URL "https://guarantee-prayer-ruby-distinct.trycloudflare.com/p/"  //link untuk HTTP GET
#define LED_PIN 13
unsigned const long interval = 6000;           //Jeda memanggil fungsi auto connect
unsigned long zero = 0;
TaskHandle_t task0;

void checkUpdate();
void ledBlink(void *pvParameter);
void autoconnet();
t_httpUpdate_return updateOverHttp(String url_update);
void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);         //menghubungkan ESP ke wifi terdekat
  WiFi.disconnect();          // menganti jaringan yang lain bisa tidak konek
  delay(100);
  Serial.println("Setup done");
  pinMode(LED_PIN, OUTPUT);
  xTaskCreatePinnedToCore(ledBlink, "ledBlink", 10000, NULL, 1, &task0, 0);   //menjalankan program blink pada core 0
}

void loop()
{
  if (millis() - zero > interval) {   //membuat timer selama 6000ms
    autorun();                       //proggram scan wifi dan update firmware
    zero = millis();
  }
}


void ledBlink(void *pvParameter) // void program menyalakan dan menghidupkan LED
{
  while (true)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED 14 HIGH");
    delay(500);
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED 14 LOW");
    delay(500);

  }
}



void autorun() {
  if (kondisikonek == true) {             //saat terkoneksi wifi
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());      //menampilkan IP
    Serial.println("-----------------------");
    checkUpdate();                           // memanggil fungsi update firmware
    if (WiFi.status() != WL_CONNECTED) {    //saat tidak terkoneksi wifi
      kondisikonek = false;                //variable kondisikonek menjadi false
    }
  }
  if (kondisikonek == false) {          // saat variable kondisikonek false
    WiFi.mode(WIFI_STA);              //scan wifi terdekat
    WiFi.disconnect();              //lakukan disconnect wifi
    Serial.println("scan start");
    int n = WiFi.scanNetworks();    //hitung jumlah wifi yang terdeteksi
    Serial.println("scan done");
    if (n == 0) {                  //saat tidak ada wifi yang terdeteksi
      Serial.println("no networks found");

    } else {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));     //menampilkan nama Wifi
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));   //menampilkan kekuatan sinyal wifi
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*"); 
        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {   //saat ada jaringan wifi yang tidak ada password
          Serial.print("Connecting to ");
          Serial.print(WiFi.SSID(i));                   //menampilkan SSID wifi
          Serial.println("...");
          WiFi.begin(WiFi.SSID(i).c_str());         //menghubungkan dengan jaringan wifi tersebut
          if (WiFi.status() != WL_CONNECTED) {    //saat koneksi terputus
            kondisikonek = false;               //mengubah isi variable kondisikonek menjadi false
          }
          kondisikonek = true;  
          Serial.println("");
          Serial.println("WiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());  //menampilkan IP Wifi
          Serial.println("-----------------------");
          break;
        }
      }
    }

  }
}


void checkUpdate()
{
  Serial.println("Checking update");
  HTTPClient http;  
  String response;
  String url = UPDATE_URL;
  http.begin(url);                          //memulai koneksi ke server pada link url
  http.GET();                              // melakukan request pada sever
  response = http.getString();            //mengambil isi (body) dari respon HTTP dalam bentuk String
  Serial.println(response);
  StaticJsonDocument<1024> doc;              //Deklarasi sebuah objek dengan nama "doc"
  deserializeJson(doc, response);           //mem-parsing data JSON yang diterima dari server
  JsonObject obj = doc.as<JsonObject>();   //mengambil objek JSON yang disimpan pada objek "doc" 
  String version = obj[String("version")];      //mengambil data yang ada di index version
  String url_update = obj[String("url")];      //mengambil data yang ada di index url
  Serial.println(version);
  Serial.println(url_update);                     
  if (version.toDouble() > FIRMWARE_VERSION)    //saat ada versi firmware terbaru
  {
    Serial.println("Update Available");
    Serial.println(url_update);
    if (updateOverHttp(url_update) == HTTP_UPDATE_OK)     //saat proses update selesai
    {
      Serial.println("Update Success");                  //menampilkan notifikasi update success
    }
    else                                                 //saat update gagal
    {
      Serial.println("Update Failed");                  //menampilkan notifikasi update failed
    }
    Serial.println("Update Success");
  }
  else                                                 //saat tidak ada versi terbaru
  {
    Serial.println("No Update Available");            //menampilkan notifikasi no update available
  }
}

t_httpUpdate_return updateOverHttp(String url_update)  
{
  t_httpUpdate_return ret;

  if ((WiFi.status() == WL_CONNECTED))        // apabila terhubung dengan wifi
  {

    ret = ESPhttpUpdate.update(url_update);  //mendownload firmware baru 

    switch (ret)
    {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        return ret;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        return ret;
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        return ret;
        break;
    }
  }
}
