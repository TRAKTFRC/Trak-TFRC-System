/*Start
 * Include libraries
 */
#include <Wire.h>
#include <RTClibExtended.h>
#include <LowPower.h>

//#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
//#include<SPI.h>
//#include <LoRa.h>
//#include <EEPROM.h>

//#include "SdFat.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------SET THESE VALUES BASED ON ACTUAL CONNECTIONS

//#define LORA_RST 10 // ----------------------------- 
//#define LORA_NSS 7 // ---------------------------- 
//#define SDchipSelect 10
#define wakePin 2    //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW
//#define GPS_PIN 4 
//int GPSBaud = 9600;

//----------------------------------------------SET THE VALUES TO CHANGE WHEN THE COLLAR SLEEPS OR WAKES UP
/*
char myName[] = "C";
const byte intervalInMin = 10;
const byte UTC_wakeHour = 6;
const byte UTC_wakeMinute = 30;
const byte UTC_sleepHour = 8;
const byte UTC_sleepMinute = 30;
const long noLocTimeOut = 100000; // --------- time in ms to wait till it acquires a location before going to sleep
*/
//------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Global Variables and objects
//SoftwareSerial gpsSerial(4, 6); 
//SdFat SD;
RTC_DS3231 RTC;  

/*
boolean iTrustRTCTime= false;
unsigned long TIMEAWAKE;
int address = 0;
byte value;
*/

//byte intervalInMin = 10;


/*------------Function to send the location/timeout update over Lora, followed by sleep ------------------
*------------In this function the device updates the rtc is the location is found
*/

/*For y seconds search for GPS location 
 *             If found location 
 *                   Save location to variables 
 *                   set foundlocationflag
 *                   break;
 *      
 *      
 *      Turn off gps
 *      send LoRa message ("Location / No location");
 *      Go_to_Sleep 
 *      
 *      
 *      This function eventually calls function to sleep
 *      earlier if signal acquired 
 *      uptill a duration of NoLocTimeOut
 */
 
 /*
 void sendLocation(){

          gpsOn();
          TinyGPSPlus gps;
          TIMEAWAKE=millis();
          DateTime now = RTC.now();
          
          int myyear = 0;
          byte mymonth = 0; 
          byte myday = 0;
          byte myhour = 0;
          byte myminute = 0;
          byte mysecond = 0;
          float mylat= 0;
          float mylon = 0;
          byte satNum = 0;
          byte alti = 0;
          
          //While timeout hasnt occured
          while(((millis() - TIMEAWAKE) < noLocTimeOut)){
                   
                unsigned long start = millis();
                do 
                {
                  while (gpsSerial.available())
                    gps.encode(gpsSerial.read());
                } while (millis() - start < 1000);
      

      
                if (gps.location.isValid())
                {
                            mylat =(gps.location.lat(), 6);
                            mylon =(gps.location.lat(), 6);
                            satNum =(gps.location.lat(), 2);
                            alti = (gps.location.lat(), 2);

                            if(gps.time.isValid() && gps.date.isValid()){
                                     
                                  
                                   myyear = gps.date.year();
                                   mymonth = gps.date.month(); 
                                   myday = gps.date.day();
                                   myhour = gps.time.hour();
                                   myminute = gps.time.minute();
                                   mysecond = gps.time.second();
                        
                                  Serial.println("The GPS time will set the rtc to :");
                                  
                                  Serial.print(myyear);Serial.print("/");Serial.print(mymonth);Serial.print("/");Serial.println(myday);
                                  Serial.print(myhour);Serial.print(":");Serial.print(myminute);Serial.print(":");Serial.println(mysecond);
      
                                  iTrustRTCTime = true;
                                  
                                  //RTC.adjust(DateTime(2014, 1, 21, 2, 9, 2));
                            }
                            
                            break; 
                           
                }
                    
          }

          gpsOff();

         
          now = RTC.now();
          myyear = (now.year());
          mymonth = (now.month()); 
          myday = (now.day());
          myhour = (now.hour());
          myminute = (now.minute());
          mysecond = (now.second());
          

          Serial.println("Send over LoRa");
          Serial.print("Lat: ");
          Serial.println(mylat);
          Serial.print("Long: ");
          Serial.println(mylon);
          Serial.print("Alt: ");
          Serial.println(alti);
          Serial.print("Sat: ");
          Serial.println(satNum);
                            
           
          LoRa.idle();
          delay(30);  
          LoRa.beginPacket();
          LoRa.print(myName);LoRa.print(value,DEC);
          // comma separator
          LoRa.print(",");
          LoRa.print(mylat);
          LoRa.print(":");
          LoRa.print(mylon);
          LoRa.print(":");
          LoRa.print(alti);
          
          LoRa.endPacket();
          delay(50);
          LoRa.sleep();

           File dataFile = SD.open("Data.txt", FILE_WRITE);
          
          if (dataFile) {
                dataFile.print(myName); dataFile.print(value,DEC);
                // comma separator
                dataFile.print(",");
                dataFile.print(mylon, 6);
                // comma separator
                dataFile.print(",");
                dataFile.print(mylat, 6);
                // comma separator
                dataFile.print(",");
                dataFile.print(myhour); dataFile.println(":");
                dataFile.print(myminute);dataFile.println(":");
                dataFile.print(mysecond);
                // comma separator
                dataFile.print(",");
                dataFile.print(myday);dataFile.println("/");
                dataFile.print(mymonth);dataFile.println("/");
                dataFile.print(myyear);
                // comma separator
                dataFile.print(",");
                dataFile.println(iTrustRTCTime);
                // comma separator
                dataFile.println();
                dataFile.close();
          }
          
          else
          {
                Serial.println("Sd Fail"); 
                        
          }
          
 } 
 */

 /*
 boolean isItNight(DateTime &now){

         //Sync the interal RTC with the External RTC. 
        //This is done to reduce querying the external one and to simplify calculation with time.
     
         //Getting Data and Time from RTC
         myyear = (now.year());
         mymonth = (now.month()); 
         myday = (now.day());
         myhour = (now.hour());
         myminute = (now.minute());
         mysecond = (now.second());
        
          //Printing Date and Time
         Serial.println("Its night at UTC");
         Serial.println("I trust the RTC time");
         Serial.println("The date and time in UTC is ");
         Serial.print(now.year());Serial.print("/");Serial.print(now.month());Serial.print("/");Serial.println(now.day());
         Serial.print(now.hour());Serial.print(":");Serial.print(now.minute());Serial.print(":");Serial.println(now.second());

         
         //IF IT ITS DAY
         if(myhour > UTC_wakeHour &&  myhour < UTC_sleepHour)
                return false;
         else if (myhour == UTC_wakeHour && myminute >= UTC_wakeMinute)
                return false;
         else if (myhour == UTC_sleepHour && myhour < UTC_sleepMinute)
                return false;
         
         //ELSE IT IS NIGHT
         else 
                return true;           
         
 }
 */
 //-------------------------------------------------
/*
void wakeUp()        // here the interrupt is handled after wakeup
{
  //On wakeup
  //detachInterrupt(digitalPinToInterrupt(wakePin));   
}
*/
/*
 void goToSleep(){

        //use interrupt 0 (pin 2) and run function wakeUp when pin 2 gets LOW 
        attachInterrupt(digitalPinToInterrupt(wakePin), wakeUp, LOW);                       
        
        //Set to sleep till interrupt   
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);   //arduino enters sleep mode here

                                         //execution resumes from here after wake-up
    
        //When exiting the sleep mode we clear the alarm
        RTC.armAlarm(1, false);
        RTC.clearAlarm(1);
        RTC.alarmInterrupt(1, false);
 }
*/
 
                          
   //This will set the alarm to the set wakeup time in the morning
   /*
void setNightAlarm(){
  
    Serial.print("Setting Alarm to ");
    Serial.print(UTC_wakeHour);Serial.print(":");Serial.println(UTC_wakeMinute);
   
    //Set the Alarm to the morning    
    RTC.setAlarm(ALM1_MATCH_HOURS, UTC_wakeHour, UTC_wakeMinute, 0);   //set your wake-up time here

    
    Serial.print("The alarm is set for "); 
    Serial.print(UTC_wakeHour);Serial.print(":");Serial.println(UTC_wakeMinute);
    
    RTC.alarmInterrupt(1, true);
    Serial.println("Alarm set");
  
}
*/

/*
// Sets the next alarms after a number of minutes x 
void setDayAlarm(){

   
}
*/
 /*Setup
 */

  /*
void setupRTC(){
    //clear any pending alarms
      RTC.armAlarm(1, false);
      RTC.clearAlarm(1);
      RTC.alarmInterrupt(1, false);
      RTC.armAlarm(2, false);
      RTC.clearAlarm(2);
      RTC.alarmInterrupt(2, false);
      RTC.writeSqwPinMode(DS3231_OFF);
}
*/
/*void setupLoRa(){
        LoRa.setPins(LORA_NSS, LORA_RST); //----------------------- set LoRa (NSS, RST) pins
        
        if (!LoRa.begin(915E6)) {
          //Serial.println("LoRa failed!");
          //Serial.println("LoRa failed");
          while (1);
        }
        
         
        // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
        LoRa.sleep(); 
        LoRa.setSignalBandwidth(31.25E3);
        //LoRa.setSpreadingFactor(12);
        LoRa.enableCrc();
        //LoRa.setSyncWord(0xF3);
        delay(50);
        //LoRa.idle();
        LoRa.idle();
        LoRa.beginPacket();
        LoRa.setTxPower(20);
        LoRa.print(myName);LoRa.print(value,DEC);;
        LoRa.print(",");
        LoRa.print("ON");
        LoRa.endPacket();
        delay(30);
        LoRa.sleep();
 
}
*/
//------------------------GPS SETUP--------------------------------------------------------
/*
void gpsOn(){
  //Serial.println("GPS ON"); 
   pinMode(GPS_PIN, OUTPUT);
   digitalWrite(GPS_PIN,LOW);
   delay(1000);
   gpsSerial.begin(GPSBaud);
   gpsSerial.flush();
}

void gpsOff(){
  //Serial.println("GPS OFF");
  pinMode(GPS_PIN, OUTPUT);
  digitalWrite(GPS_PIN,HIGH);
  delay(1000);
  //gps.standby();
  gpsSerial.flush();
   gpsSerial.end();
}*/
//----------------------------SD card setup------------------------------------------------
/*
void setupSdCard(){
  if (!SD.begin(SDchipSelect)) {
    //Serial.println("initialization failed!");
    return;
  }
}
*/
 void Setup(){
      
      Serial.begin(9600);
      //gpsSerial.begin(GPSBaud);
     // value = EEPROM.read(address);
      
      
      delay(3000); // wait for console opening
      Serial.println("Begin");
      pinMode(wakePin, INPUT_PULLUP);
      //Serial.print(myName); //Serial.println(value,DEC);
      
      Wire.begin();
      Wire.setClock(100000);
      
      RTC.begin();
      RTC.writeSqwPinMode(DS3231_OFF);
  
      DateTime now = RTC.now();


     int myyear = (now.year());
   byte mymonth = (now.month()); 
   byte myday = (now.day());
   byte myhour = (now.hour());
   byte myminute = (now.minute());
   byte mysecond = (now.second());

   Serial.println("The time and date is :");

   //Serial.print(myyear);Serial.print("/");Serial.print(mymonth);Serial.print("/");Serial.println(myday);
   //Serial.print(myhour);Serial.print(":");Serial.print(myminute);Serial.print(":");Serial.println(mysecond);

//   Serial.println("Next Alarm at "); Serial.println((myminute + intervalInMin)%60);Serial.println(" minute");
   
    //Set SQW pin to OFF (in my case it was set by default to 1Hz)
    //The output of the DS3231 INT pin is connected to this pin
    //It must be connected to arduino D2 pin for wake-up
    RTC.armAlarm(1, false);
    RTC.clearAlarm(1);
    RTC.alarmInterrupt(1, false);
    RTC.armAlarm(2, false);
    RTC.clearAlarm(2);
    RTC.alarmInterrupt(2, false);
    

  //Wake up time is intervalInMin from now
  RTC.setAlarm(ALM1_MATCH_MINUTES, 0, ((myminute + 10)%60), 0, 0);
  RTC.alarmInterrupt(1, true);
 
 // Serial.print("The Alarm set for "); Serial.print(intervalInMin); Serial.println("minutes from now");
 // Serial.print("That is, when the minute turns ");Serial.println((myminute + intervalInMin)%60);
                
        //goToSleep();
                
      
      //setupRTC();
      //setupLoRa();
      //setupSdCard();
      //setupGps();
      
 }

///////////////////////////////////////////////////////////////////////////////////
 /*
         * If (itrustRTCTime)
       *      Get time
       *      If it is night
       *          Set Alarm to WakeUp time;
       *      If it is day
       *          Set Alarm to an IntervalMinute from now
       *          run sendLocation
       */ 
/////////////////////////////////////////////////////////////////////////////////// 
void loop() {     
      
      /*
       //If i trust time
        /*
        if (iTrustRTCTime){
                   if(isItNight()){
                          setNightAlarm();
                          //sendLocation(now);
                          goToSleep();
                   }
                   else{
                         //sendLocation();
                         //setDayAlarm(now);
                   }
        }
       
       *  Else Set Alarm for Setup_Timeout min from now
       *  run sendLocation
        
      // else{
                /*The send location funtion will set the device to deep sleep once it has
                   * acquired, send and saved its gps location or a no location timeout has occured 
                   
                sendLocation();
                /*The setDayAlarm will set the Alarm on the RTC to the 
                 *Interval Time away from now (the local time (synced with the RTC at the start of the loop)) 
                 *The RTC is not queried again
                  
                
       }       
      */
}
