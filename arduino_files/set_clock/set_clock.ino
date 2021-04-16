/*---------------I ASK YOU DO NOT DISTRIBUTE OR MODIFY---------------------

 Data logger set clock

 created 02/13/2016
 last modified 02/13/2016

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
 Upload to set the time.
 
-----------------I ASK YOU DO NOT DISTRIBUTE OR MODIFY--------------------------*/
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

/*------------------settings------------------------*/
#define time_set //set the time

//#define clear_flag  //clear the eeprom flag
//#define EEPROM_clear //clear all the eeprom
/*--------------------------------------------------*/

// Here are the libraries you will need to install

#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <RTCDS1307.h>
#include <EEPROM.h>

//#include <RTC_DS1307.h>

RTC_DS1307 RTC;


void setup () {
  
  Wire.begin();
  RTC.begin();
  
  #ifdef time_set
    RTC.adjust(DateTime(__DATE__, __TIME__));
  #endif
  
  #ifdef EEPROM_clear
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  #endif
  
  #ifdef clear_flag
  EEPROM.write(1000, 0);
  #endif
  
}

void loop () {
   Serial.begin(9600);
   if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!"); }
    // following line sets the RTC to the date & time this sketch was compiled
    DateTime now = RTC.now();
    
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
    Serial.println();
    delay(3000);
}
