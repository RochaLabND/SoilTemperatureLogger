/*This is a simplified 4 probe only version of the 
  data logger script contributed by Jeffrey Crompton 
  (NRCan). This script has been tested in the lab
  but not in the field */

/*---------------I ASK YOU DO NOT DISTRIBUTE OR MODIFY---------------------

 Data logger arduino code

 created 10/01/2015
 last modified 10/23/2022

 by Sal
 opentemplog@gmail.com
 (contact me with comment thoughts and sugestions)

 Copyright 2015 Salvatore R. Curasi
 This program is distributed under the terms of the GNU General Public License

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 BASIC FUNCTIONALITY:
 Collectd data from dallas single wire temperature sensors and logs it to 
 an sd card.
 
 SETUP:
 Load the code to the appropriate board with a properly programed pic12f683
 Device time should be set on the first boot if not you must reset the eeprom
 and reprogram the board. 
 
 To setup the probes hold the red button on startup
 untill the red led illuminated, reslease, insert 1 probe, press the button,
 green led will light, repeat once the green led goes out, untill the red led
 goes out.

 If connected to the terminal enter any character within 10 seconds of holding
 the button to print disgnostic information. 

-----------------I ASK YOU DO NOT DISTRIBUTE OR MODIFY--------------------------*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTCDS1307.h>
#include <EEPROM.h>

//#include <RTC_DS1307.h>
/*------------------settings------------------------*/
//#define four_probe //comment in for 2 probe, comment out for 10
/*--------------------------------------------------*/

const char SENSOR_PIN = 2;
const char chipSelect = 10; //pin for sd
const char led1 =  3; //led      
const char led2 = 4; //led
const char buttonPin = 6; //button
const char shtdwn = 5; //talks to pic

OneWire  ourWire(SENSOR_PIN); 
DallasTemperature sensors(&ourWire);
RTC_DS1307 RTC;
File myFile; //test file


void setup(){
  
 pinMode(led1, OUTPUT); 
 pinMode(led2, OUTPUT);  
 pinMode(chipSelect,OUTPUT);
 pinMode(buttonPin, INPUT);  
 pinMode(shtdwn, OUTPUT);
 Wire.begin();
 RTC.begin();
 
 /*if(EEPROM.read(1000) == 36)
 {
  RTC.adjust(DateTime(__DATE__, __TIME__)); 
  EEPROM.write(1000, 36);
 }
 */
/*-----------button press ?------------*/ 
 unsigned int startTime = millis();
 unsigned int heldTime = 0;
  
 while (digitalRead(buttonPin) == HIGH)
 {
   heldTime = millis() - startTime;
   if (heldTime > 2000)
   {
       break;
     }
   }
 if (heldTime > 2000){ 
   digitalWrite(led1, HIGH);
   delay(1000);
     Serial.begin(9600);
     Serial.println(F("Type any character to begin adding sensors!"));
     Serial.println(F(""));
     while (Serial.read() <= 0) {}
     delay(400);
     digitalWrite(led2, HIGH);
     delay(1000);              
     digitalWrite(led1, LOW);
     digitalWrite(led2, LOW);
     if (!SD.begin(chipSelect)) {
      Serial.println(F("initialization failed!"));
      }
      Serial.println(F("initialization done."));
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      myFile = SD.open("test.txt", FILE_WRITE);
  
      // if the file opened okay, write to it:
      if (myFile) {
        Serial.print(F("Writing to test.txt..."));
      myFile.println(F("testing 1, 2, 3."));
	// close the file:
      myFile.close();
      Serial.println(F("done."));
      } else {
      // if the file didn't open, print an error:
      Serial.println(F("error opening test.txt"));
      }
    
      // re-open the file for reading:
      myFile = SD.open("test.txt");
      if (myFile) {
      Serial.println("test.txt:");
    
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
    	Serial.write(myFile.read());
      }
      // close the file:
      myFile.close();
    } else {
  	// if the file didn't open, print an error:
    Serial.println(F("error opening test.txt"));
    }
    SD.remove("test.txt");
/*-------------------------end test SD---------------------*/
/*-------------------------test RTC-----------------------*/
    if (! RTC.isrunning()) {                                          //5 comm ????
    Serial.println(F("RTC is NOT running!"));
      }
    else {
    DateTime now = RTC.now();  
    Serial.println(F("time now..."));
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    }
    Serial.println(F("Done"));                                      //5:6 comm here fail ?????
/*-----------------------end test RTC----------------------*/
/*-----------------------test temp probes------------------*/
    discoverOneWireDevices(); //print hex id's
    sensors.begin(); //start library
    sensors.setResolution(12); //set resolution
    Serial.print(F("Requesting temperatures..."));
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println(F("DONE"));
   
    //#ifdef four_probe
    for (int x = 0 ; x < 5 ; x++) {
    Serial.print(F("Temperature for Device "));
    Serial.print( x );
    Serial.print(F(" is: "));
    Serial.print(sensors.getTempCByIndex(x));
    Serial.println(F(""));
    }
    //#endif
    Serial.print(F("Done"));                                                     //0:1 comm here fail ???????
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F("to add probes type any character to skip power off..."));
    Serial.println(F(""));
    while (Serial.read() <= 0) {}
    delay(1000); 
    digitalWrite(led1, HIGH);
    retrieveOneWireDevices();
    digitalWrite(led1, LOW);                                                  //0 comm here fail ???????????
  }
      else{
      SD.begin(chipSelect);
      if (SD.exists("log.csv")) {
      sensors.begin(); //start library
      sensors.setResolution(12); 
      }
      else{
       Serial.begin(9600); 
       myFile = SD.open ("log.csv", FILE_WRITE);
       //#ifdef four_probe
       myFile.println(F("date(yyyy/mm/dd),time(hh:mm:ss),internal,probe1,probe2,probe3,probe4"));   
       //#endif
       myFile.close();
       sensors.begin(); //start library
       sensors.setResolution(12); 
      }  
      } 
  
    }

void loop(){
 
   DateTime now = RTC.now();
   myFile = SD.open ("log.csv", FILE_WRITE);  
   myFile.print(now.year(), DEC);
   myFile.print(F("/"));
   myFile.print(now.month(), DEC);
   myFile.print(F("/"));
   myFile.print(now.day(), DEC);
   myFile.print(F(","));
   myFile.print(now.hour(), DEC);
   myFile.print(F(":"));
   myFile.print(now.minute(), DEC);
   myFile.print(F(":"));
   myFile.print(now.second(), DEC);
   myFile.print(F(","));
   myFile.close();
   sensors.requestTemperatures();
   
  //#ifdef four_probe
  logtemp(0);
  printcom();
  logtemp(8);
  printcom();
  logtemp(16);
  printcom();
  logtemp(24);
  printcom();
  logtemp(32);
  printre();
  //#endif
  
  delay(100);
  digitalWrite(shtdwn, HIGH);
  delay(50);
  digitalWrite(shtdwn, LOW);  
  delay(2000);
}

/*-------------User function to read hex address------*/
void discoverOneWireDevices(void){ 
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Looking for 1-Wire devices...\n\r");// "\n\r" is NewLine 
  while(ourWire.search(addr)) {
    Serial.print("\n\r\n\rFound \'1-Wire\' device with address:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(F(", "));
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n\r");
      return;
    }
  }
  Serial.println();
  Serial.println(F("Done"));
  Serial.println(F(""));
  ourWire.reset_search();
  return;
}

/*------------------------address retirval--------------------*/
void retrieveOneWireDevices(void) {

  //#ifdef four_probe
  byte i;
  byte dic1[8];
  byte dic2[8];
  byte dic3[8];
  byte dic4[8];
  byte dic5[8];
  byte addr[8];
  byte present = 0;
  byte data[12];
  //#endif

  //#ifdef four_probe
  //adress  1 here  
  while(ourWire.search(addr)) {
    for( i = 0; i < 8; i++) {
      dic1[i] = addr[i];
      }
    }
  wait4push();
////adress 2 here
  while(ourWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true)
    {
      #ifdef Seriald
      Serial.print(F("t"));
      #endif
    }
    else{
      #ifdef Seriald
      Serial.print(F("f"));
      #endif
      for( i = 0; i < 8; i++) {
      dic2[i] = addr[i];
  } 
  }
  }
  wait4push();
  ///adress 3 here
  while(ourWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true)
    {
      #ifdef Seriald
      Serial.print(F("t"));
      #endif
    }
    else{
      #ifdef Seriald
      Serial.print(F("f"));
      #endif
      for( i = 0; i < 8; i++) {
      dic3[i] = addr[i];
  }
  } 
  }
   wait4push();
  ///adress 4 here
  while(ourWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true)
    {
      #ifdef Seriald
      Serial.print(F("t"));
      #endif
    }
    else{
      #ifdef Seriald
      Serial.print(F("f"));
      #endif
      for( i = 0; i < 8; i++) {
      dic4[i] = addr[i];
  }
  } 
  }
  wait4push();
  ///adress 5 here
  while(ourWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true)
    {
      #ifdef Seriald
      Serial.print(F("t"));
      #endif
    }
    else{
      #ifdef Seriald
      Serial.print(F("f"));
      #endif
      for( i = 0; i < 8; i++) {
      dic5[i] = addr[i];
  }
  } 
  }
  //wait4push();
  //#endif
  
 
  //#ifdef four_probe
  eeprom_array_write_null(0);
  eeprom_array_write(dic1,0);
  eeprom_array_write_null(8);
  eeprom_array_write(dic2,8);
  eeprom_array_write_null(16);
  eeprom_array_write(dic3,16);
  eeprom_array_write_null(24);
  eeprom_array_write(dic4,24);
  eeprom_array_write_null(32);
  eeprom_array_write(dic5,32);
   Serial.print(F("probe1(internal): "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic1[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic1[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe2: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic2[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic2[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe3: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic3[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic3[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe4: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic4[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic4[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe5: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic5[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic5[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  
  //#endif
  Serial.println(F(""));
  Serial.println(F("Done"));
}
//-----user defined function for delay in aquiring adresses--discoverOneWireDevices---//
void wait4push (void) {
  ourWire.reset_search();
  delay(1000);
  digitalWrite(led2, HIGH);
   while (digitalRead(buttonPin)==LOW)
  {
    delay(10);                        //  debounce
  }
  digitalWrite(led2, LOW);
}

//--user defined fucntion to save adresses to eeprom--discoverOneWireDevices--//
void eeprom_array_write(byte ar[],byte v)
{
byte k;  
for( k = 0; k < 8; k++) {
EEPROM.write(k+v, ar[k]);
}
}
void eeprom_array_write_null(byte v)
{
byte k;
for(k=0;k<8;k++)
{
  EEPROM.write(k+v, 225);
}
}

//---user defined address comparison--discoverOneWireDevices--//
boolean ByteArrayCompare(byte a[],byte b[])
{
     if(a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3]  && a[4] == b[4] && a[5] == b[5]  && a[6] == b[6]  && a[7] == b[7])
     {
       return(true);
     }
   else
   {
   return(false);
   }
}

//----user defined prints carrier return----//
 void printre()
{
  File myFile;
  myFile = SD.open ("log.csv", FILE_WRITE);
  myFile.println(F(" "));
  myFile.close();
}

//----user defined prints comma-----// 
void printcom()
{
   File myFile;
   myFile = SD.open ("log.csv", FILE_WRITE);
  myFile.print(F(","));
  myFile.close();
}

//----log temp----//
void logtemp(byte k)
{
  byte addr[8];
  byte j;
  for(j=0;j<8;j++)
 {
   addr[j]=EEPROM.read(j+k);
 } 
  float tempC = sensors.getTempC(addr); 

 if (tempC == -127.00) 
   {
   File myFile;
   myFile = SD.open ("log.csv", FILE_WRITE);  
   myFile.print(F("Error getting temperature  "));
   myFile.close();
   } 
   else
   {
   File myFile;
   myFile = SD.open ("log.csv", FILE_WRITE);  
   myFile.print(tempC);
   myFile.close();
   }
}
