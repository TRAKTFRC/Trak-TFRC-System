/* Program for 
 * Required hardware : LT1
 * Please set pins as per your connections if using combination of modules. IE RTC, SD RW, RFM95, OLED display 
 */


//#include <AltSoftSerial.h>    // Allows two Serial connections

#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include "SdFat.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "RTClib.h"


SSD1306AsciiAvrI2c oled;

#define I2C_ADDRESS 0x3C // ----------------------- oled I2C address
#define RST_PIN -1 //------------------------------ oled reset pin
#define SD_CS 10 // -------------------------------- sd card module chip select
#define LORA_RST A1 // ----------------------------- 
#define LORA_NSS A0 // ---------------------------- 
#define LORA_INTR 2 // ---------------------------- 
#define BUTTON1_PIN 5    //----------------------- Push Button Pin
#define BUTTON2_PIN 6    //----------------------- Push Button Pin
#define GPS_PIN 4   // ---------------------------GPS Power Pin
#define GPS_RX 9 //
#define GPS_TX 8 //
char myName[] = "B7"; //---------------device name---------------------------------------------------------------
SdFat SD;


int button1_PushCounter = 0;
int button1_State = 0;         // current state of the button
int lastButton1_State = 0;     // previous state of the button

unsigned long oledRefreshTime = 10000; // --------- time between oled refresh
unsigned long lastRefresh;
String senderName;
String payload1;
char fileName[20];
int rssi;
boolean newMsg = false;
boolean newCollarMsg = false;
boolean newSMTMsg = false;
boolean newTowerMsg = false;
boolean relayed = false;
int invert = 0;

TinyGPS gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
RTC_DS3231 rtc;

void setup() {
  
  //Serial.begin(9600);
  setupOled();
  
  oled.clear();
  oled.println(myName);
  
  //Serial.println(myName);
  
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  gpsOn();
  
  oled.println("GPS ready");
  
  setupLoRa();
  
  //Serial.println("LoRa ready");
  oled.println("LoRa ready");

  setupRTC();

  delay(200);
  oled.println("Clock set");
  
  setupSDcard();
  delay(50);
  updateFileName();
  
  delay(50);
  //Serial.println("SD card ready");
  oled.println("Sdcard ready");
  delay(50);
  oled.println("Setup Complete");
  //Serial.println("Ready");

  delay(2000);
  
  oled.clear();
  
}
//------------------------GPS SETUP--------------------------------------------------------
void gpsOn(){
  pinMode(GPS_PIN, OUTPUT);
  digitalWrite(GPS_PIN,LOW);
  gpsSerial.begin(9600);
}
void gpsOff(){
  pinMode(GPS_PIN, OUTPUT);
  digitalWrite(GPS_PIN,HIGH);
}
//------------------------OLED SETUP----------------------------------------------------------------
void setupOled(){
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);
  oled.setScrollMode(SCROLL_MODE_AUTO);
  //oled.setScrollMode(SCROLL_MODE_APP);//SCROLL_MODE_APP SCROLL_MODE_AUTO
  //oled.displayRemap(true);
}
//------------------------SD CARD SETUP ------------------------------------------------------------
void setupSDcard(){
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    //Serial.println("Sdcard failed");
    oled.println("Sdcard failed");
    // don't do anything more:
    while (1);
  }
  //Serial.println("Sdcard");
}
// -----------------------RTC SETUP ----------------------------------------------------------------
void setupRTC(){
   if (! rtc.begin()) {
    ////Serial.println("Couldn't find RTC");
    oled.println("RTC Fail");
    while (1);
  }
}
//-----------------------LORA SETUP ---------------------------------------------------------------
void setupLoRa(){
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_INTR); //----------------------- set LoRa (NSS, RST) pins
  
  if (!LoRa.begin(915E6)) {
    //Serial.println("LoRa failed!");
    oled.println("LoRa failed");
    while (1);
  }
  
  
  // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
  LoRa.sleep(); 
  LoRa.setSignalBandwidth(31.25E3);
  LoRa.enableCrc();
  //LoRa.setSpreadingFactor(12);
  //LoRa.setSyncWord(0xF3);
  delay(50);
  LoRa.idle();
  LoRa.beginPacket();   LoRa.setTxPower(20);
  LoRa.print(myName);
  LoRa.print(",");
  LoRa.print("ON");
  LoRa.endPacket();
  delay(30); 

}
// --------------------------------------- FILE NAMING ----------------------------------------
void updateFileName(){
  delay(1000);
  DateTime now = rtc.now();
  delay(1000);
  sprintf(fileName, "LA%02d%02d%02d.txt", now.month(), now.day(), now.hour());
  delay(1000);
  
}
// --------------------------------------- PRINT TIME ON OLED -------------------------------------------
void printTime(){
        
        DateTime now = rtc.now();
        
        if (now.hour() < 10){
          oled.print("0");
        }
        oled.print(now.hour(), DEC);
        oled.print(":");
        
        if (now.minute() < 10){
          oled.print("0");
        }
        oled.print(now.minute(), DEC);
        oled.print(":");
         if (now.second() < 10){
          oled.print("0");
        }
        oled.print(now.second(), DEC);
}
// ---------------------------------------- PRINT DATE ON OLED ----------------------------------------------
void printDate(){
  DateTime now = rtc.now();
    if (now.day() < 10){
    oled.print("0");
    }
    oled.print(now.day(), DEC);
    oled.print("/");
    if (now.month() < 10){
      oled.print("0");
    }
    oled.print(now.month(), DEC);
    oled.print("/");
    oled.print(now.year(), DEC);
}
//----------------------------------------- SEND LATITUDE AND LOGITUDE OVER LORA -----------------------------

void sendLatLong(){
 
    bool validLocation = false;
    unsigned long chars;
    unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values -------- during this process we check for valid location
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        validLocation = true;
    }
  }
  
  // ------------ If so serial data is coming in. Pop error of no GPS-----------------------------
  gps.stats(&chars, &sentences, &failed);
  if (chars == 0)
    oled.println("No GPS connected**");
  
  
  // -------------------------------- Else If location is Valid
  else if (validLocation)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    if(button1_PushCounter == 5){
        oled.setCursor(0, 1);
        printTime();
        oled.setCursor(70,1);
        printDate();
        oled.println("                     ");
        oled.println("                     ");
        oled.println("                     ");
        oled.println("                     ");
        oled.setCursor(0,2);
        oled.println("SIGNAL ACQ");
        oled.setCursor(85,2);
        oled.print("SAT=");
        oled.println(gps.satellites());
        oled.print("LAT=");
        oled.println(flat, 6);
        oled.print("LNG=");
        oled.println( flon, 6);     
    }
    LoRa.idle();
    delay(50);
    LoRa.beginPacket();   LoRa.setTxPower(20);
    LoRa.print(myName);
    LoRa.print(",");
    LoRa.print(flat, 6);
    delay(30);
    LoRa.print(":");
    LoRa.print(flon, 6);
    LoRa.endPacket();
    delay(30);
    LoRa.sleep();
  }
  
  //---- else the gps hasn't acquired location ---------
  else{
    LoRa.idle();
    delay(50);
    LoRa.beginPacket();   LoRa.setTxPower(20);
    LoRa.print(myName);
    LoRa.print(",");
    LoRa.print("ACQLOC");
    LoRa.endPacket();
    delay(30);
    
    if(button1_PushCounter == 5){
        oled.setCursor(0, 1);
        printTime();
        oled.setCursor(70,1);
        printDate();
        oled.setCursor(0,1);
        oled.println("Acquiring Signal");  
    }
  }
  
}

// ------------------------------- SAVE TO SD CARD ------------------------------------------------------
void saveToSd(String incommingData){
  File dataFile = SD.open(fileName,FILE_WRITE ); //O_CREAT | O_WRITE
  DateTime now = rtc.now();
  if (dataFile) {
  
        dataFile.print(incommingData);
        dataFile.print(",");
        if (now.hour() < 10){
          dataFile.print("0");
        }
        dataFile.print(now.hour(), DEC);
        dataFile.print(":");
  
        if (now.minute() < 10){
          dataFile.print("0");
        }
        dataFile.print(now.minute(), DEC);
        dataFile.print(":");
         if (now.second() < 10){
          dataFile.print("0");
        }
        dataFile.print(now.second(), DEC);
        dataFile.print(",");
        if (now.day() < 10){
          dataFile.print("0");
        }
        dataFile.print(now.day(), DEC);
         dataFile.print("/");
        
        if (now.month() < 10){
          dataFile.print("0");
        }
        dataFile.print(now.month(), DEC);
        dataFile.print("/");
        dataFile.print(now.year(), DEC); 

        ////Serial.println("File Saved");
        
    }
    // if the file isn't open, pop up an error:
    else {
      ////Serial.println("error writing file");
      oled.clear();
      oled.println("SD card error");
      delay(1000);
    }
    dataFile.println();
    dataFile.close();
}
/*
void parseLoRaMsg(String message)
{
  int ind1 = message.indexOf(',');  //finds location of first ,
  senderName = message.substring(0, ind1);   //captures first data String

  delay(50);
  //int ind2 = message.indexOf(',', ind1+1 );
  payload1 = message.substring(ind1+1);

  if (senderName.indexOf('C') == 0 ){
    //Serial.println("This is a collar");
    newCollarMsg = true;
  } 
  else if (senderName.indexOf('S') == 0){
    //Serial.println("This is a Simple Mammal Trap");
    newSMTMsg = true;
  }
  else if (senderName.indexOf('B') == 0){
    ////Serial.println("This is a Basestation");
    newTowerMsg = true;
  }
}
*/
//----------------- LISTENING TO LORA RADIO FREQUENCY -------------------------------------------------------------------
void listenForMsg(){
  String incomingMessage = "";
  char data;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    newMsg = true; // ----------------- set new msg true
    ////Serial.println(LoRa.packetRssi());
    rssi = (LoRa.packetRssi());
    while (LoRa.available()) {
          
          data = (char)LoRa.read();
          incomingMessage +=data;
          //Serial.print(data);
          //dataFile.print(data);
          
        }
    //Serial.print(" with RSSI ");
    //Serial.println(rssi);
     saveToSd(incomingMessage);   //--------------------------------------------SAVING TO SD CARD-------------------------------------
     delay(50);
     
     //------------------------------------PARSING MSG FOR OLED----------------------------------------
     int ind1 = incomingMessage.indexOf(',');  //finds location of first ,
     senderName = incomingMessage.substring(0, ind1);   //captures first data String

    delay(50);
    //int ind2 = incomingMessage.indexOf(',', ind1+1 );
    payload1 = incomingMessage.substring(ind1+1);

    if (senderName.indexOf('C') == 0 ){
      //Serial.println("This is a collar");
      newCollarMsg = true;
    } 
    else if (senderName.indexOf('S') == 0){
      //Serial.println("This is a Simple Mammal Trap");
      newSMTMsg = true;
    }
    else if (senderName.indexOf('B') == 0){
      ////Serial.println("This is a Basestation");
      newTowerMsg = true;
    }
  
     delay(50);
    if(senderName>myName){ //-------------------------------CHECKING HEIRARCHY AND RELAYING----------------------------------------
      delay(random(0, 500));
      LoRa.idle();
      LoRa.beginPacket();   
      LoRa.setTxPower(20);
      LoRa.print(myName);
      LoRa.print(incomingMessage);
      LoRa.endPacket();
      delay(30);
    // --------------------------------------RELAY--------------------------------------------------
      relayed = true;
    }
    else {relayed = false;} // --------------------------ELSE DO NOT RELAY--------------------------------------------
  }
}
// ------------------------------ UPDATE OLED -------------------------------------------
void oledBasicInfo(){
      //Serial.println("Oled refresh");
      oled.setCursor(0, 1);
      printTime();
      oled.setCursor(70,1);
      printDate();
      oled.setCursor(0,2);
      oled.print("Name: "); oled.print(myName);
      oled.setCursor(0,3);
      oled.println("Last msg rec. from : ");
      
      if(newMsg){
        oled.println("                     ");
        oled.println("                     ");
        oled.println("                     ");
        oled.println("                     ");
        oled.setCursor(0,4);
        oled.print(senderName); 
        oled.print(" at ");
        printTime();
        oled.println();
        oled.print("Rssi :");
        oled.println(rssi);
        if(relayed){oled.println("Relayed");}
        else{oled.println("Not Relayed");}
        
        oled.set2X();
        if(invert){
            oled.print(".");
            invert = 0;
        }
        else{
          oled.print("  ");
          invert = 1;
        }
        oled.set1X();
      }
      
      lastRefresh = millis();
      delay(10);     
}
void loop() {
  int button1_State = digitalRead(BUTTON1_PIN);
  // compare the buttonState to its previous state
  if (button1_State != lastButton1_State) {
       if (button1_State == HIGH) {
            button1_PushCounter++;
            oled.clear();
            if(button1_PushCounter == 1){
              oled.setScrollMode(SCROLL_MODE_OFF);
              oled.println();
              oled.println("Basic Info");
              delay(500);
              oled.clear();
              oledBasicInfo();
            }
            else if(button1_PushCounter == 2){
              oled.setScrollMode(SCROLL_MODE_AUTO);
              oled.println();
              oled.println("Filter for Collars");
            }
            else if(button1_PushCounter == 3){
              oled.setScrollMode(SCROLL_MODE_AUTO);
              oled.println();
              oled.println("Filter for SMT");
            }
            else if(button1_PushCounter == 4){
              oled.setScrollMode(SCROLL_MODE_AUTO);
              oled.println();
              oled.println("Filter for Towers");
            }
            else if(button1_PushCounter == 5){
              oled.setScrollMode(SCROLL_MODE_OFF);
              oled.println();
              oled.println("GPS Mode");     
            }
        } 
      else {delay(50);} //delay to stop bouncing
  
    //If counter is more that 4 reset to 1
  if(button1_PushCounter > 5){ button1_PushCounter = 1;}
      // save the current state as the last state, for next time through the loop
      lastButton1_State = button1_State;
  }
  
  listenForMsg();
  switch (button1_PushCounter){
    case 1: 
            if (( (millis() - lastRefresh) > oledRefreshTime)){
              oledBasicInfo(); 
              delay(50);
              LoRa.idle();
              LoRa.beginPacket();
              LoRa.setTxPower(20);
              LoRa.print(myName);
              LoRa.print(",");
              LoRa.print("LIVE");
              LoRa.endPacket();
              delay(30); 
              lastRefresh = millis();
            }
            else if(newMsg)
            {
              oledBasicInfo();
              lastRefresh = millis();
            }
            delay(50);
            break;
            
     case 2:  if(newCollarMsg)
              {
                parseToOled();
              }
              break; 

      case 3: 
              if(newSMTMsg)
              {
                parseToOled();
              }
              break;

      case 4: 
              if(newTowerMsg)
              {
                parseToOled();
              }
              break;
      case 5:
              gpsOn();
              if (( (millis() - lastRefresh) > oledRefreshTime)){
              sendLatLong();
              lastRefresh = millis();
            }
            break;        
    }
    newMsg = false; 
    newCollarMsg = false;
    newSMTMsg = false;
    newTowerMsg = false;
}
void parseToOled(){
    oled.print(senderName);oled.print(" @ rssi ");oled.print(rssi);oled.println(":");
    oled.println(payload1);
    oled.println();
}
