#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>
#include "WiFi.h"
#include "EEPROM.h"
#define FIRMWARE_VERSION 1.0                                                     // versi program
#define UPDATE_URL "https://fastapiskripsi-be199a487d88.herokuapp.com/p/"
String urlgetperintah="https://fastapiskripsi-be199a487d88.herokuapp.com/getperintah";
int c = 0;
int Data = 0;
const int udpPort = 8000;
boolean responseOKEBoolean = false;
String urlpostversi = "";
String paketDataString = "";
String paketSSID = "";
String paketPSWD = "";
String paketUUIDDEVICE = "";
const char* dataepromSSID = "12";
const char* dataepromPSWD = "12";
const char* dataepromUUIDDEVICE = "12";

boolean kondisiBtn = true;
boolean kondisikonek_UDP = false;
boolean ledState = false;
boolean koneksiWifi = false;
boolean kondisidownload = false;
unsigned long interval = 1000; // the time we need to wait
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long interval1 = 20; // the time we need to wait
unsigned long previousMillis1 = 0;
unsigned long currentMillis1 = 0;
unsigned const long interval_update = 5000;                                           //Jeda memanggil fungsi auto connect
unsigned long zero = 0;

char charSSID[20];
char charSSIDpharse[20];

char charUUIDDEVICE[40];
char charUUIDDEVICEpharse[40];

char charPSWD[20];
char charPSWDpharse[20];
WiFiUDP udp;
HTTPClient http;
TaskHandle_t task0;
void ledBlink(void *pvParameter);
String EEPROM_get(char add);

t_httpUpdate_return updateOverHttp(String url_update);

void setup() {
  EEPROM.begin(512);
  Serial.begin(9600);
  EEPROM.commit();
  pinMode(15, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(25,OUTPUT);
  digitalWrite(2, ledState);
  dataepromSSID = EEPROM.get(0, charSSID);
  dataepromPSWD = EEPROM.get(20, charPSWD);
  dataepromUUIDDEVICE = EEPROM.get(60, charUUIDDEVICE);
  WiFi.begin(dataepromSSID, dataepromPSWD);
  xTaskCreatePinnedToCore(ledBlink, "ledBlink", 50000, NULL, 1, &task0, 1);
}



void loop() {
  //  Serial.println(dataepromUUIDDEVICE);
  //Serial.print("SSID ");
  //Serial.println(dataepromPSWD);
  String link = dataepromUUIDDEVICE;
  if (kondisidownload == true) {
    digitalWrite(2, HIGH);
    delay(300);
    digitalWrite(2, LOW);
    delay(300);
  }

  urlpostversi = "https://fastapiskripsi-be199a487d88.herokuapp.com/edit";
  if (kondisikonek_UDP == false && koneksiWifi == true) {
    String response;

    String url = UPDATE_URL;
    // memulai koneksi ke server pada link url
    //    http.addHeader("Content-Type", "application/json");
    if (responseOKEBoolean == false) {
      http.begin(urlpostversi);
      // Membuat JSON body
      String jsonData = "{\"name\": [\"" + link + "\", \"" + "1" + "\"]}";
      http.POST(jsonData);// melakukan request pada sever
      response = http.getString();            // mengambil isi (body) dari respon HTTP dalam bentuk String
      Serial.println(response);
      StaticJsonDocument<1024> doc;              // Deklarasi sebuah objek dengan nama "doc"
      deserializeJson(doc, response);           // mem-parsing data JSON yang diterima dari server
      JsonObject obj = doc.as<JsonObject>();   // mengambil objek JSON yang disimpan pada objek "doc"
      String responseOKE = obj[String("DATA")];      // mengambil data yang ada di index version
      Serial.println("responseOKE: " + responseOKE);
      if (responseOKE == "OKE") {
        responseOKEBoolean = true;
      }
    }
    if(kondisidownload==false){
    http.begin(urlgetperintah);
    String jsonData = "{\"name\": [\"" + link + "\"]}";
    http.POST(jsonData);// melakukan request pada sever
    response = http.getString();    
    // mengambil isi (body) dari respon HTTP dalam bentuk String
    Serial.println(response);
    }
    StaticJsonDocument<1024> doc;              // Deklarasi sebuah objek dengan nama "doc"
    deserializeJson(doc, response);           // mem-parsing data JSON yang diterima dari server
    JsonObject obj = doc.as<JsonObject>();
    
    // mengambil objek JSON yang disimpan pada objek "doc"
    String id1 = obj[String("id1")];
    String id2 = obj[String("id2")];
    String id3 = obj[String("id3")];
    String id4 = obj[String("id4")];
    String id5 = obj[String("id5")];
    String id6 = obj[String("id6")];
    
    if(id1=="1"){
      Serial.println("13 ON");
      digitalWrite(13,LOW);
    }
    if(id1=="0"){
      digitalWrite(13,HIGH);
    }
    if(id2=="1"){
      digitalWrite(26,LOW);
    }
    if(id2=="0"){
      digitalWrite(26,HIGH);
    }
    if(id3=="1"){
      digitalWrite(14,LOW);
    }
    if(id3=="0"){
      digitalWrite(14,HIGH);
    }
    if(id4=="1"){
      digitalWrite(27,LOW);
    }
    if(id4=="0"){
      digitalWrite(27,HIGH);
    }
    if(id5=="1"){
      digitalWrite(26,LOW);
    }
    if(id5=="0"){
      digitalWrite(26,HIGH);
    }
    if(id6=="1"){
      digitalWrite(25,LOW);
    }
    if(id6=="0"){
      digitalWrite(25,HIGH);
    }
    //    http.begin(url);                          // memulai koneksi ke server pada link url
    //    http.GET();                              // melakukan request pada sever
    //    response = http.getString();            // mengambil isi (body) dari respon HTTP dalam bentuk String
    //    Serial.println(response);
  }
  if (kondisikonek_UDP == true) {
    udp.begin(udpPort);
    responseOKEBoolean = false;
    Serial.println(WiFi.localIP());
    char  replyPacket[10] = "terima";
    udp.write((uint8_t *)replyPacket, sizeof(replyPacket));
    udp.endPacket();
    int packetSize = udp.parsePacket();
    if (packetSize) {
      char packetData[packetSize + 1];
      udp.read(packetData, packetSize);
      packetData[packetSize] = '\0';;
      paketDataString = packetData;
      paketSSID  = getSubString(paketDataString, ',', 0);
      paketPSWD = getSubString(paketDataString, ',', 1);
      paketUUIDDEVICE = getSubString(paketDataString, ',', 2);
      paketSSID.toCharArray(charSSIDpharse, paketSSID.length() + 1);
      paketPSWD.toCharArray(charPSWDpharse, paketPSWD.length() + 1);
      paketUUIDDEVICE.toCharArray(charUUIDDEVICEpharse, paketUUIDDEVICE.length() + 1);
      EEPROM.commit();
      EEPROM.put(0, charSSIDpharse);
      EEPROM.put(20, charPSWDpharse);
      EEPROM.put(60, charUUIDDEVICEpharse);
      EEPROM.commit();
      delay(100);
      dataepromSSID = EEPROM.get(0, charSSID);
      dataepromPSWD = EEPROM.get(20, charPSWD);
      dataepromUUIDDEVICE = EEPROM.get(60, charUUIDDEVICE);

      WiFi.begin(dataepromSSID, dataepromPSWD);
      kondisikonek_UDP = false;
      //      datae3 = getSubString(paketDataString, ',', 2);
    }

  }


  Data = digitalRead(15);
  if (Data == 1) {
    kondisiBtn = false;
  }
  else if (Data == 0) {
    if (kondisiBtn == false) {
      c++;
      kondisikonek_UDP = false;
      currentMillis1 = 0;
      previousMillis1 = 0;
      kondisiBtn = true;
    }
  }
  if (c >= 1) {
    currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) {
      previousMillis = millis();
      Serial.println(previousMillis );
    }
  }

  while (previousMillis >= 10000 && kondisikonek_UDP == false) {

    currentMillis1 = millis();
    if ((currentMillis1 - previousMillis1) >= interval1) {
      previousMillis1 = millis();
      ledState = !ledState;
      digitalWrite(2, ledState);
      Serial.println(previousMillis1);
    }
    if (WiFi.status() != WL_CONNECTED) {

      Serial.println("Connecting to WiFi...");
      kondisikonek_UDP = false;
    }

    if (kondisikonek_UDP == false) {          // saat variable kondisikonek_UDP false
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
        for (int i = 0; i < n; ++i) {     //Membuat perulangan sejumlah nilai n
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
              kondisikonek_UDP = false;               //mengubah isi variable kondisikonek_UDP menjadi false
            }
            digitalWrite(2, LOW);
            kondisikonek_UDP = true;                //Mengubah isi variable kondisikonek_UDP menjadi true
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());               //menampilkan IP Wifi
            Serial.println("-----------------------");
            c = 0;
            currentMillis = 0;
            previousMillis = 0;
            break;                                      //Keluar dari fungsi for
          }
        }
      }

    }
    Serial.println(WiFi.localIP());

  }
}

String getSubString(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


String EEPROM_get(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}



void ledBlink(void *pvParameter) {
  while (true)
  {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(dataepromSSID, dataepromPSWD);
      koneksiWifi = false; //saat tidak terkoneksi wifi
      Serial.println("Tidak KONEK.....");
      WiFi.begin(dataepromSSID, dataepromPSWD);
      while (WiFi.status() != WL_CONNECTED) {
//         WiFi.disconnect();
//         WiFi.reconnect();
        digitalWrite(2,HIGH);
        delay(500);
        Serial.print(".");
          digitalWrite(2,LOW);
          delay(500);
      }

      koneksiWifi = true; //variable kondisikonek menjadi false
    }
    if (WiFi.status() == WL_CONNECTED) {
      if (millis() - zero > interval_update) {  //membuat timer selama 6000ms
        checkUpdate();                    //proggram scan wifi dan update firmware
        Serial.println("cek v1");
        zero = millis();
      }

    }
  }
}



void checkUpdate()
{
  Serial.println("Checking update");
  Serial.println("Versi Program V11");
  HTTPClient http;
  String responseupdate;                       //Membuat variable response
  String urlR = "https://fastapiskripsi-be199a487d88.herokuapp.com/getversiESP";
  String jsonDatae = "{\"name\": [\"" + String(dataepromUUIDDEVICE) + "\"]}";
  http.begin(urlR);
  http.POST(jsonDatae);// melakukan request pada sever
  responseupdate = http.getString(); //Menampung link pada variable url
  // memulai koneksi ke server pada link url
  Serial.println(responseupdate);
  StaticJsonDocument<1024> doc;              // Deklarasi sebuah objek dengan nama "doc"
  deserializeJson(doc, responseupdate);           // mem-parsing data JSON yang diterima dari server
  JsonObject obj = doc.as<JsonObject>();   // mengambil objek JSON yang disimpan pada objek "doc"
  String version3 = obj[String("aku")];      // mengambil data yang ada di index version

  Serial.println("version:" + version3);

  if (version3.toDouble() != FIRMWARE_VERSION)    //saat ada versi firmware terbaru
  {
    kondisidownload = true;
    String url_update = "https://fastapiskripsi-be199a487d88.herokuapp.com/c/" + version3;
    Serial.println("Update Available");
    Serial.println(url_update);
    if (updateOverHttp(url_update) == HTTP_UPDATE_OK)     //saat proses update selesai
    {
      Serial.println("Update Success");                  // menampilkan notifikasi update success
    }
    else                                                 //saat update gagal
    {
      Serial.println("Update Failed");                  // menampilkan notifikasi update failed
    }
    Serial.println("Update Success");
  }
  else                                                 // saat tidak ada versi terbaru
  {
    Serial.println("No Update Available");            // menampilkan notifikasi no update available
  }
}


t_httpUpdate_return updateOverHttp(String url_update) //Membuat fungsi dengan nama updateOverhttp
{
  t_httpUpdate_return ret;

  if ((WiFi.status() == WL_CONNECTED))        // apabila terhubung dengan wifi
  {

    ret = ESPhttpUpdate.update(url_update);  //mendownload firmware baru

    switch (ret)
    {
      case HTTP_UPDATE_FAILED:  //Saat response HTTP_UPDATE_FAILED
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());  //Menampilkan pesan eror
        return ret;
        break;

      case HTTP_UPDATE_NO_UPDATES:                  //Saat response HTTP_UPDATE_NO_UPDATES
        Serial.println("HTTP_UPDATE_NO_UPDATES"); //Menampilkan pesan HTTP_UPDATE_NO_UPDATES
        return ret;
        break;

      case HTTP_UPDATE_OK:                  //Saat response OK
        Serial.println("HTTP_UPDATE_OK"); //Menampilkan tulisan HTTP UPDATE OK
        return ret;
        break;
    }
  }
}
