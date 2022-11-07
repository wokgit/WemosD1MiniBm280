#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "htmls.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define EEPROM_SIZE 1024
#define SEALEVELPRESSURE_HPA (1013.25)

//default pass
String newHostname = "Czujnik BM280";
String ssid = "defSsid";  
String password = "defPass";
const char *ssidAp = "bm280_config_ap";
const char *passwordAp = "12345678";
//ustawienie serw
ESP8266WebServer server(80);
ESP8266WiFiMulti WiFiMulti;
int WifiReTry=0;
int  ApMode=0;
int counterReset=0;
Adafruit_BME280 bme;
void setup() {
  Serial.begin(115200);
  getFromEEPROMPass();
   delay(2000);
    bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid.c_str(),password.c_str());
 
  Serial.println("");
  Serial.println("Laczenie WiFi");
  for(int i=0;i<2;i++){
    if(WiFiMulti.run()==WL_CONNECTED){
       Serial.println("STA Mode "); 
      ApMode=1;
      break;  
    }  
  }
  if(ApMode==0){
    delay(100);
    WiFi.mode(WIFI_AP_STA);
    Serial.println("ap mode");
    WiFi.softAP(ssidAp, passwordAp);
    server.on("/",ChangePassWifi);
    server.on("/pomiary",mainPage);
    
  }
  else{
     server.on("/",mainPage);
     server.on("/wifi",ChangePassWifi);
    
    
  }
  server.on("/processpass",ChangePassWifiProccess);
   server.on("/temp",getTemperature);
   server.on("/pressure",getPressure);
   server.on("/human",getHuman);
   server.on("/aprox",getApprox);
  WiFi.hostname(newHostname.c_str());
  server.begin();
}
void loop() {
 if(ApMode!=0){
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      server.handleClient();
    }
  }
  else {
     counterReset+=1;
     if(counterReset>30000) ESP.restart();
     server.handleClient();
  }
   
  delay(100);

}
void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}
void getPressure(){
  printValues();
   String val;
   val=String((bme.readPressure() / 100.0F));
   server.send(200, "text/html",val);
}
void getTemperature(){
  printValues();
   String val;
   val=String((bme.readTemperature()));
   server.send(200, "text/html",val);
}
void getHuman(){
   printValues();
   String val;
   val=String((bme.readHumidity()));
   server.send(200, "text/html",val);
}
void getApprox(){
   printValues();
   String val;
   val=String((bme.readAltitude(SEALEVELPRESSURE_HPA)));
   server.send(200, "text/html",val);
}
void ChangePassWifiProccess(){
  if(server.hasArg("ssid") && server.hasArg("password") && server.hasArg("hostname") ){
    putInEEPROM(server.arg("ssid"),server.arg("password"),server.arg("hostname"));
     server.send(200, "text/html","Zapisano dane, restartowanie ESP");
     ESP.restart();
   }
   else{
    server.send(200, "text/html","Blad danych");
   } 
}
void ChangePassWifi(){
        String s=PASSCHANGE_PAGE;
        server.send(200, "text/html",s);
   
}
void mainPage(){
        String s=MAIN_page;
        server.send(200, "text/html",s);
   
}
void putInEEPROM(String ssidNew,String pass, String hostnameesp){
  EEPROM.begin(EEPROM_SIZE);
  int address = 0;
  int controlAdress=7;
  //Zapisanie liczby kontrolnej 
  EEPROM.put(address,controlAdress );
  address += sizeof(controlAdress);
  // Zapisanie dlugosci ssida
  int sizeofssid=ssidNew.length();
  EEPROM.put(address,sizeofssid );
  address += sizeof(sizeofssid);
  // zapisanie dlugosci hasla
  int sizeofpass=pass.length();
  EEPROM.put(address,sizeofpass );
  address += sizeof(sizeofpass);
  // zapisanie dlugosci hasla
  int sizeofhostname=hostnameesp.length();
  EEPROM.put(address,sizeofhostname );
  address += sizeof(sizeofhostname);
  // zapisanie ssida
  for(int i=0;i<sizeofssid;i++){
      char n=ssidNew[i];
      EEPROM.put(address, n);
      address += sizeof(n);
   }
   for(int i=0;i<sizeofpass;i++){
      char n=pass[i];
      EEPROM.put(address, n);
      address += sizeof(n);
   }
   for(int i=0;i<sizeofhostname;i++){
      char n=hostnameesp[i];
      EEPROM.put(address, n);
      address += sizeof(n);
   }
  EEPROM.commit();
  EEPROM.end(); 
}
void getFromEEPROMPass(){
    EEPROM.begin(EEPROM_SIZE);
     int address = 0;
     // jeżeli adres początkowy jest 5 to  jest coś zapisane i czytaj haslo z eepromu 
    int controlAdress = 3;
    EEPROM.get(address, controlAdress);
    if(controlAdress==7){
      Serial.println("Sa zapisane dane w EEPROM");
      address += sizeof(controlAdress);
      // odczyt dlugosci ssida
      int ssidSize=0;
      EEPROM.get(address, ssidSize);
      Serial.println(ssidSize);
      address += sizeof(ssidSize);

     // odczyt dlugosci hasla
      int passSize=0;
      EEPROM.get(address, passSize);
      Serial.println(passSize);
      address += sizeof(passSize);
      //odczyt dlugosci hostname
      int hostnamesize=0;
      EEPROM.get(address, hostnamesize);
      Serial.println(hostnamesize);
      address += sizeof(hostnamesize);
      //odczyt ssida
      String ssidret="";
      for(int i=0;i<ssidSize;i++){
        char n='x';
        EEPROM.get(address, n);
        ssidret+=n;
        address += sizeof(n);
      }
      Serial.println(ssidret);

      String passret="";
      for(int i=0;i<passSize;i++){
        char n='x';
        EEPROM.get(address, n);
        passret+=n;
        address += sizeof(n);
      }
      Serial.println(passret);
      
      String hostnameret="";
      for(int i=0;i<hostnamesize;i++){
        char n='x';
        EEPROM.get(address, n);
        hostnameret+=n;
        address += sizeof(n);
      }
      Serial.println(hostnameret);

      newHostname = hostnameret;
      ssid = ssidret;  
      password = passret;
    } 
    else {
        Serial.println("Danych nie ma  w EEPROM");
    }
    EEPROM.end();
}
