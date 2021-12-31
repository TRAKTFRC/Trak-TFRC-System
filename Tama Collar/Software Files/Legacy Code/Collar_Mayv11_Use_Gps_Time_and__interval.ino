#include <Adafruit_SleepyDog.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include<SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#define LORA_RST 10 // ----------------------------- 
#define LORA_NSS 7 // ---------------------------- 

unsigned long NOLOCTIMEOUT = 100000; // --------- if not gps location in this period then go off
unsigned long TIMEAWAKE;

byte SLEEPHOUR = 22;
byte SLEEPMINUTE = 2;
byte WAKEHOUR = 10;  //10.30am for 5 30am in peru
byte WAKEMINUTE = 2; //30


int night_interval = 2400; //
int day_interval = 1800; //* 450
long time_interval = day_interval;// Sets the wakeup intervall in minutes
int GPSBaud = 4800;

      
SoftwareSerial gpsSerial(4, 6); // Rx,tx
int address = 0;
byte value;

char myName[] = "C";


boolean clockSet = false;


//----------------------------------FUNCTION TO SLEEP FOR AN INTERVAL (Using watchdog timer repeatedly)----
void intervalSleep(){
  int count = 0;
  //print interval sleep
  //sleep interval number of times
  if (checkGpsState()){                                    
                                          Serial.println("Turning Off");
                                          gpsPulse();
                                          gpsSerial.end();
                                          }
  
  
  while(count<=time_interval){
    
                                
                                Serial.print("Interval ");
                                Serial.println(time_interval-count);
                                /*
                                                LoRa.idle();
                                                delay(30);
                                                LoRa.beginPacket();
                                                LoRa.setTxPower(20);
                                                LoRa.print(myName); 
                                                LoRa.print(value,DEC);
                                                LoRa.print(",");
                                                LoRa.print("Int");
                                                LoRa.print((time_interval-count));
                                                LoRa.endPacket();
                                                LoRa.sleep();
                                                delay(30);
                                    */            
                                                count++;

                                   delay(50);
                                   int sleepMS = Watchdog.sleep();
                                   delay(20);
                                        
                             Serial.print("Woke after interval! I slept for ");
                             Serial.print(sleepMS, DEC);
                             Serial.println(" milliseconds.");
                             Serial.println();
                                  
                             Serial.print("Woke");
  } 
   LoRa.idle();
    delay(30);
    LoRa.beginPacket();
    LoRa.setTxPower(20);
    LoRa.print(myName); 
    LoRa.print(value,DEC);
    LoRa.print(",");
    LoRa.print("Wake up");
    LoRa.endPacket();
    LoRa.sleep();
    delay(30);
  
}

//----------------FUNCTION TO SET THE TIME INTERVAL BETWEEN WAKE UPS-----
void set_Interval(TinyGPSPlus &gps){
  
    //MAKE SURE GPS IS ON AT THIS POINT
     //create a temporary time variable so we can set the time and read the time from the RTC
    
    delay(1000);
    Serial.println("Going to SLeep");
    if(gps.time.isValid() && gps.date.isValid()){
    
            Serial.println("THE TIME: ");
            Serial.print(gps.time.hour());Serial.print(":");Serial.print(gps.time.minute());Serial.print(":");Serial.print(gps.time.second());Serial.print(" ");Serial.print(gps.date.day());Serial.print("/");Serial.print(gps.date.month());Serial.print("/");Serial.println(gps.date.year());
            delay(2000);


            if(gps.time.hour() > WAKEHOUR && gps.time.hour()<SLEEPHOUR){
                  Serial.println("Its day");
                  time_interval = day_interval;
             }
             else if(gps.time.hour()== WAKEHOUR && gps.time.minute()>=WAKEMINUTE){
                  Serial.println("Its day");
                  time_interval = day_interval;
             }
             else if(gps.time.hour()== SLEEPHOUR && gps.time.minute()< SLEEPMINUTE){
                  Serial.println("Its day");
                  time_interval = day_interval;
             }

             else{
                 Serial.println("Its Night"); 
                 time_interval = night_interval;
                 
                             /*if wakehour - curent hour  > 0 duration to wake up wakehour-cuurent time
                             
                                      int hourDiff;
                                      int minDiff;
                                               //if wakehour- current hour > 0{ 
                                               // wakehour-currenthour
                                      if(gps.time.hour() < WAKEHOUR){
                                             Serial.println("Its night 1");
                                             hourDiff = WAKEHOUR-gps.time.hour();
                                             minDiff = WAKEMINUTE-gps.time.minute();
                                             
                                             time_interval = (  (hourDiff*3600)  +  (minDiff*60)  ) /9;
                                      }
                                      else if(gps.time.hour() == WAKEHOUR && gps.time.minute() < WAKEMINUTE)
                                      {
                                             hourDiff = 0;
                                             minDiff = WAKEMINUTE-gps.time.minute();
                                             time_interval = ( (minDiff*60)  ) / 9;
                                             
                                             Serial.println("Its night 2");
                                      }
                                      else if((gps.time.hour() == SLEEPHOUR) && (gps.time.minute()>SLEEPMINUTE))
                                      {
                                            Serial.println("Its night 3");
                                            
                                              
                                                                 //*(difference of hours in seconds)  -  (difference of minutes in seconds))    
                                           time_interval = time_interval = (  (hourDiff*3600)  +  (minDiff*60)  ) /9;
                                      }
                                      else if(gps.time.hour() > SLEEPHOUR){
                                            Serial.println("Its night 4");
                                            hourDiff = 23 - gps.time.hour();
                                            minDiff = 60 - gps.time.minute();
                                            time_interval = (  (hourDiff*3600)  +  (minDiff*60)  ) /9;
                                      }
            
                                      
                                      Serial.print("See you in"); Serial.print(time_interval); Serial.println(" intervals");
                                      int intervalInSec = time_interval*8; 
                                      Serial.print(intervalInSec);Serial.println(" seconds");
                                      int intervalInMin = intervalInSec/60;
                                      int intervalinHours = intervalInMin/60;
                                      Serial.print(intervalInMin); Serial.println(" Minutes");
                                      Serial.println(intervalinHours); Serial.println(" Hours");
                            */
             
                  }
             
             

             
            /*
            if((gps.time.hour()>SLEEPHOUR)||(gps.time.hour()==SLEEPHOUR && gps.time.minute()>SLEEPMINUTE)){
              //RTC.setAlarm(alarmType, seconds, minutes, hours, dayOrDate);
              Serial.println("Hour is late. Deep Sleep");
              //time_interval = night_interval; //3600*8 
            }
            else if(gps.time.hour()< WAKEHOUR){
              //RTC.setAlarm(alarmType, seconds, minutes, hours, dayOrDate);
              Serial.println("Hour is early. Deep Sleep");
              time_interval = night_interval;
              //intervalSleep();
             // RTC.setAlarm(ALM1_MATCH_HOURS , WAKEMINUTE, WAKEHOUR, 0);
            }
            else if(gps.time.hour()==WAKEHOUR && gps.time.minute()< WAKEMINUTE){
              //RTC.setAlarm(alarmType, seconds, minutes, hours, dayOrDate);
              Serial.println("Hour is same. Minute is early. Deep Sleep");
              time_interval = night_interval;
              //intervalSleep();
             // RTC.setAlarm(ALM1_MATCH_HOURS , WAKEMINUTE, WAKEHOUR, 0);
            }
            else{
              Serial.println("Interval sleep");
              time_interval = day_interval;
            }

            */
            if(checkGpsState())
                {
                    Serial.println("GPS TURNED OFF"); 
                    gpsSerial.end();
                    gpsPulse();
                }
    }
}

void setupLoRa(){
  LoRa.setPins(LORA_NSS, LORA_RST); //----------------------- set LoRa (NSS, RST) pins
  
  if (!LoRa.begin(915E6)) {
    //Serial.println("LoRa failed!");
    Serial.println("LoRa failed");
    while (1);
  }
  
  // LoRa.setSpreadingFactor(12);
  // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3
  LoRa.sleep(); 
  LoRa.setSignalBandwidth(31.25E3);
  LoRa.enableCrc();
  //LoRa.setSyncWord(0xF3);
  delay(50);

  LoRa.sleep(); 
      delay(50);
      LoRa.idle();
      LoRa.beginPacket();   //LoRa.setTxPower(20);
      LoRa.print(myName);
      LoRa.print(value,DEC);
      LoRa.print(",");
      LoRa.print("ON");
      LoRa.endPacket();
      delay(30); 
      LoRa.sleep();
  

}
void gpsPulse(){
    digitalWrite(3,HIGH);
    delay(100);
    digitalWrite(3,LOW);
    delay(1000);
}
int checkGpsState(){
   for (unsigned long start = millis(); millis() - start < 1000;)
   {
      if (gpsSerial.available()){
        Serial.println(F("GPS ON"));
        return 1;
       }
    }
    Serial.println("GPS OFF");
    return 0;
}
void setup()
{

  //gps wake pin
    value = EEPROM.read(address);
        Serial.print(myName); Serial.println(value,DEC);
        
    
        pinMode(3,OUTPUT);
        delay(1000);
  
  gpsSerial.begin(GPSBaud);
  Serial.begin(9600);

  if(checkGpsState())
        {
            Serial.println("GPS TURNED OFF"); 
            gpsSerial.end();
            gpsPulse();
        }
        checkGpsState();
  
  
  //setupRtc();
  //Serial.println("RTC");
  
  setupLoRa();
  Serial.println("Lora");
  

          if(checkGpsState())
        {
            Serial.println("GPS TURNED OFF"); 
            gpsSerial.end();
            gpsPulse();
        }
  //intervalSleep();
  TIMEAWAKE = millis();
}
void loop()
{
  /*
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      sendInfo(gps);
  */
  TinyGPSPlus gps;
  
  
  sendInfo(gps);
  
  if (( (millis() - TIMEAWAKE) > NOLOCTIMEOUT) && clockSet){
              delay(50);
              
              set_Interval(gps);
              
              Serial.println("NoLoc");

              intervalSleep();
              TIMEAWAKE = millis();
  } 
  if (( (millis() - TIMEAWAKE) > NOLOCTIMEOUT) && (!clockSet)){
              delay(50);
          
              Serial.println("Setup");
              intervalSleep();
              TIMEAWAKE = millis();
              
  }   
 }

void sendInfo(TinyGPSPlus &gps)
{ 
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < 1000);
  
  if (millis() > 5000 && gps.charsProcessed() < 10){
                    if (!checkGpsState()){                                    
                                          Serial.println(F("Turning On"));
                                          gpsPulse();
                                          gpsSerial.begin(4800);
                                          }
  }
  
  Serial.println("Searching.");
  if (gps.location.isValid())
  {
    if(gps.time.isValid() && gps.date.isValid()){
                
                  Serial.println("GPS now has valid time");
                  //setSyncProvider(RTC.get); 
                  delay(50);
            
                  clockSet = true;
                  Serial.println("RTC Y");
                  set_Interval(gps);
                  
            
        }
     if (checkGpsState()){                                    
                                          Serial.println("Turning Off");
                                          gpsPulse();
                                          gpsSerial.end();
                                          }

    Serial.print("Lat: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Long: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Alt: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Sat: ");
    Serial.println(gps.satellites.value(), 6);
    
    LoRa.idle();
    delay(30);  
    LoRa.beginPacket();
    LoRa.print(myName);LoRa.print(value,DEC);
    LoRa.print(",");
    LoRa.print(gps.location.lat(), 6);
    LoRa.print(":");
    LoRa.print(gps.location.lng(), 6);
    LoRa.endPacket();

    delay(50);
    LoRa.sleep();
    delay(1000);

    intervalSleep();
    TIMEAWAKE = millis();
              
  }
  
  else
  {
    Serial.println("Searching for signal");
  }
  delay(1000);
}
