/*
    Catatan: Library JSON terbaru mungkin tidak berfungsi dengan kode tersebut.
    Jadi, Anda mungkin perlu menurunkan versi perpustakaan ke versi v5.13.5

    Create By ASEP SUCIPTO INDRA SUKMA

    GITHUB: https://github.com/aurroracell/asep-sucipto-program
*/
//-----------------------------------------------------------------------------------
//FirebaseESP8266.h harus disertakan sebelum ESP8266WiFi.h
#include <FirebaseESP8266.h>  //https://github.com/mobizt/Firebase-ESP8266
#include <WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
//Instal Library ArduinoJson
//Catatan: Pustaka JSON terbaru mungkin tidak berfungsi dengan kode tersebut.
//Jadi, Anda mungkin perlu menurunkan versi perpustakaan ke versi v5.13.5
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
#define FIREBASE_HOST "esp-32-gps-firebase-7f8f3-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCEjTrLJD2UYWo9SKVoQySk94DJdDEK0bo"
#define WIFI_SSID "MV007-6AF2"
#define WIFI_PASSWORD "64138003"
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//Tentukan objek data FirebaseESP8266
FirebaseData firebaseData;
FirebaseJson json;
//-----------------------------------------------------------------------------------
uint8_t Relay1 = 15;
int load1, Button;
//-----------------------------------------------------------------------------------
//Modul GPS pin RX ke NodeMCU D1
//Modul GPS pin TX ke NodeMCU D2
const int RXPin = 16, TXPin = 17;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;
//-----------------------------------------------------------------------------------
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void setup()
{


  Serial.begin(115200);
  neo6m.begin(9600);
  pinMode(Relay1, OUTPUT); 
  digitalWrite(Relay1, HIGH); 
  wifiConnect();
  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase OK.");
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void loop() {
  smartdelay_gps(1000);
  if(gps.location.isValid())
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
   
    //-------------------------------------------------------------
    //Kirim ke Serial Monitor untuk Debugging
    Serial.print("LATITUDE:  ");
    Serial.println(latitude);  // float ke x tempat desimal
    Serial.print("LONGITUDE: ");
    Serial.println(longitude);
    //-------------------------------------------------------------
    if(Firebase.get(firebaseData, "/GPS/BUTTON")) {
    if (firebaseData.dataType() == "string") {
    load1 = firebaseData.stringData().toInt();
    digitalWrite(Relay1, load1); 
    Serial.println("------------------------------------");
    Serial.println("Berhasil membaca dari Firebase: " + firebaseData.dataPath() + ": " + load1 + " (" + firebaseData.dataType() +")"); 
    Serial.println("------------------------------------");
    Serial.println();
  }
}else{
    digitalWrite(Relay1, HIGH);
    Serial.println("------------------------------------");
    Serial.println("RELAY MATI: " + firebaseData.errorReason());  
    Serial.println("------------------------------------");
    Serial.println();
 }
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude))
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude))
      {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
  }
  else
  {
    Serial.println("No valid GPS data found.");
  }
  delay(5000);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void firebaseReconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
