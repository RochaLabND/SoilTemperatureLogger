/*---------info-------------
 Code for Arduino data logger
 Created: 10/01/2015 by Salvatore R Curasi
 Last modified: 7/8/2019
 This is an updated version of the code which 
 has been tested in the lab but not in the field
 ----------------------------
 
 --------Directions----------
 BASIC FUNCTIONALITY:
 Collects data from Dallas single wire temperature sensors and 
 logs it to an SD card.
 
 SETUP:
 Load the code to the appropriate board with a properly programed
 pic12f683. Device logging interval should have been set on the 
 pic.

 NOTES:
 This code requires a number of libraries in order to function 
 properly namely OneWire, DallasTemperature, Wire, SD, SPI, 
 RTClib, EEPROM.

 The settings section below will control the data logger behavior
 and the number of sensors supported.

 To setup the probes and run the logger self-diagnostic connect 
 the device to a computer and open the terminal. Hold the red 
 button on startup until the red led illuminated, release, enter
 commands to the Terminal. When prompted insert 1 probe, press 
 the button, the green led will light. Repeat this process until 
 both the green and red leds go out.
----------------------------*/

/*------load libraries------*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <EEPROM.h>
/*--------------------------*/

/*------------------settings-----------------------*/
//#define ten_probe //comment in for 10 probes
//#define two_probe //comment in for 2 probes
#define four_probe //comment in for 4 probes
#define bvcc //comment in to record battery voltage
//#define soilm //comment in to record soil moisture (experimental)
/*--------------------------------------------------*/

const char SENSOR_PIN = 2; //dallas one wire bus
const char chipSelect = 10; //sd  chip select pin
const char led1 =  3;      
const char led2 = 4;
const char buttonPin = 6;
const char shtdwn = 5; //pin which communicates with the pic
#ifdef bvcc
const char voltPin = 0; //pin which measures battery voltage
#endif
#ifdef soilm //setup for soil moisture sensor
const char soilmPin = 1;
const char pwrPin = 9;
#endif

OneWire  oneWire(SENSOR_PIN); 
DallasTemperature sensors(&oneWire);
RTC_DS1307 RTC;
File myFile; //test file

void setup(){
  
 pinMode(led1, OUTPUT); 
 pinMode(led2, OUTPUT);  
 pinMode(chipSelect,OUTPUT);
 pinMode(buttonPin, INPUT);  
 pinMode(shtdwn, OUTPUT);
 #ifdef soilm
 pinMode(pwrPin, OUTPUT);
 #endif
 Wire.begin();
 RTC.begin();

/*-----------button press?------------*/ 
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
     Serial.println(F("Type any character to start..."));
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
      myFile = SD.open("test.txt", FILE_WRITE);
  
      // if the file opened write to it
      if (myFile) {
        Serial.print(F("Writing to test.txt..."));
      myFile.println(F("testing 1, 2, 3."));
	// close the file:
      myFile.close();
      Serial.println(F("done."));
      } else {
      // if the file didn't open print error
      Serial.println(F("error opening test.txt"));
      }
    
      // re-open the file
      myFile = SD.open("test.txt");
      if (myFile) {
      Serial.println("test.txt:");
    
      // read from the file
      while (myFile.available()) {
    	Serial.write(myFile.read());
      }
      // close the file
      myFile.close();
    } else {
  	// if the file didn't open print error
    Serial.println(F("error opening test.txt"));
    }
    SD.remove("test.txt");
/*-------------------------end test SD---------------------*/
/*-------------------------test RTC-----------------------*/
    if (! RTC.isrunning()) {                                         
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
    Serial.println(F("Done"));                                     
/*-----------------------end test RTC----------------------*/
/*-----------------------test temp probes------------------*/
    discoverOneWireDevices(); //print hex id's
    sensors.begin(); //start library
    delay(250);
    sensors.setResolution(12); //set resolution
    Serial.print(F("Requesting temperatures..."));
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println(F("DONE"));
    #ifdef ten_probe                                                       
    for (int x = 0 ; x < 11 ; x++) {
    Serial.print(F("Temperature for Device "));
    Serial.print( x );
    Serial.print(F(" is: "));
    Serial.print(sensors.getTempCByIndex(x));
    Serial.println(F(""));
    }
    #endif
    #ifdef two_probe
    for (int x = 0 ; x < 3 ; x++) {
    Serial.print(F("Temperature for Device "));
    Serial.print( x );
    Serial.print(F(" is: "));
    Serial.print(sensors.getTempCByIndex(x));
    Serial.println(F(""));
    }
    #endif
    #ifdef four_probe
    for (int x = 0 ; x < 5 ; x++) {
    Serial.print(F("Temperature for Device "));
    Serial.print( x );
    Serial.print(F(" is: "));
    Serial.print(sensors.getTempCByIndex(x));
    Serial.println(F(""));
    }
    #endif
    Serial.print(F("Done"));                                                   
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F("to add probes type any character to skip power off..."));
    Serial.println(F(""));
    while (Serial.read() <= 0) {}
    delay(1000); 
    digitalWrite(led1, HIGH);
    retrieveOneWireDevices();
    digitalWrite(led1, LOW);                                                 
  }
      else{
      SD.begin(chipSelect);
      if (SD.exists("log.csv")) {
      sensors.begin(); //start library
      //delay(250);
      sensors.setResolution(12); 
      }
      else{ 
       myFile = SD.open ("log.csv", FILE_WRITE);
       #ifdef ten_probe
       myFile.println(F("date(yyyy/mm/dd),time(hh:mm:ss),Vcc,BVcc,Soilm,internal,probe1,probe2,probe3,probe4,probe5,probe6,probe7,probe8,probe9,probe10"));   
       #endif
       #ifdef two_probe
       myFile.println(F("date(yyyy/mm/dd),time(hh:mm:ss),Vcc,BVcc,Soilm,internal,probe1,probe2"));   
       #endif
       #ifdef four_probe
       myFile.println(F("date(yyyy/mm/dd),time(hh:mm:ss),Vcc,BVcc,Soilm,internal,probe1,probe2,probe3,probe4"));   
       #endif
       myFile.close();
       sensors.begin(); //start library
       //delay(250);
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
   printFloat(readVcc(), 2);
   myFile.print(F(","));
   #ifdef bvcc
   printFloat(readBVcc(), 2);
   myFile.print(F(","));
   #else
   myFile.print(F("NA"));
   myFile.print(F(","));
   #endif
   #ifdef soilm
   int soilmoisture;
   digitalWrite(pwrPin, HIGH);
   soilmoisture = analogRead(soilmPin);
   digitalWrite(pwrPin, LOW);
   myFile.print(soilmoisture);
   myFile.print(F(","));
   #else
   myFile.print(F("NA"));
   myFile.print(F(","));
   #endif
   myFile.close();
   sensors.requestTemperatures();
   //delay(250);
   
  #ifdef two_probe
  logtemp(0);
  printcom();
  logtemp(8);
  printcom();
  logtemp(16);
  printre();
  #endif
   #ifdef four_probe
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
  #endif
  #ifdef ten_probe
  logtemp(0);
  printcom();
  logtemp(8);
  printcom();
  logtemp(16);
  printcom();
  logtemp(24);
  printcom();
  logtemp(32);
  printcom();
  logtemp(40);
  printcom();
  logtemp(48);
  printcom();
  logtemp(56);
  printcom();
  logtemp(64);
  printcom();
  logtemp(72);
  printcom();
  logtemp(80);
  printre();
  #endif
  
  //delay(100);
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
  while(oneWire.search(addr)) {
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
  oneWire.reset_search();
  return;
}

/*------------------------address retirval--------------------*/
void retrieveOneWireDevices(void) {
  #ifdef ten_probe
  byte i;
  byte dic1[8];
  byte dic2[8];
  byte dic3[8];
  byte dic4[8];
  byte dic5[8];
  byte dic6[8];
  byte dic7[8];
  byte dic8[8];
  byte dic9[8];
  byte dic10[8];
  byte dic11[8];
  byte addr[8];
  byte present = 0;
  byte data[12];
  #endif
  #ifdef two_probe
  byte i;
  byte dic1[8];
  byte dic2[8];
  byte dic3[8];
  byte addr[8];
  byte present = 0;
  byte data[12];
  #endif
  #ifdef four_probe
  byte i;
  byte dic1[8];
  byte dic2[8];
  byte dic3[8];
  byte dic4[8];
  byte dic5[8];
  byte addr[8];
  byte present = 0;
  byte data[12];
  #endif

#ifdef two_probe  
//adress  1 here  
  while(oneWire.search(addr)) {
    for( i = 0; i < 8; i++) {
      dic1[i] = addr[i];
      }
    }
  wait4push();
////adress 2 here
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  #endif
  #ifdef ten_probe
  //adress  1 here  
  while(oneWire.search(addr)) {
    for( i = 0; i < 8; i++) {
      dic1[i] = addr[i];
      }
    }
  wait4push();
////adress 2 here
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  wait4push();
  ///adress 6 here
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true)
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
      dic6[i] = addr[i];
  }
  } 
  }
   wait4push();
  ///adress 7 here
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true||(ByteArrayCompare(dic6,addr))==true)
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
      dic7[i] = addr[i];
  }
  } 
  }
  wait4push();
  ///adress 8 here
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true||(ByteArrayCompare(dic6,addr))==true||(ByteArrayCompare(dic7,addr))==true)
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
      dic8[i] = addr[i];
  }
  } 
  }
  wait4push();
  ///adress 9 here
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true||(ByteArrayCompare(dic6,addr))==true||(ByteArrayCompare(dic7,addr))==true||(ByteArrayCompare(dic8,addr))==true)
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
      dic9[i] = addr[i];
  }
  } 
  }
   wait4push();
   //probe 10 address here
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true||(ByteArrayCompare(dic6,addr))==true||(ByteArrayCompare(dic7,addr))==true||(ByteArrayCompare(dic8,addr))==true ||(ByteArrayCompare(dic9,addr))==true)
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
      dic10[i] = addr[i];
  }
  } 
  }
   wait4push();
  while(oneWire.search(addr)){
    if((ByteArrayCompare(dic1,addr))==true ||(ByteArrayCompare(dic2,addr))==true ||(ByteArrayCompare(dic3,addr))==true||(ByteArrayCompare(dic4,addr))==true||(ByteArrayCompare(dic5,addr))==true||(ByteArrayCompare(dic6,addr))==true||(ByteArrayCompare(dic7,addr))==true||(ByteArrayCompare(dic8,addr))==true ||(ByteArrayCompare(dic9,addr))==true ||(ByteArrayCompare(dic10,addr))==true)
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
      dic11[i] = addr[i];
  }
  } 
  }
  #endif
  #ifdef four_probe
  //adress  1 here  
  while(oneWire.search(addr)) {
    for( i = 0; i < 8; i++) {
      dic1[i] = addr[i];
      }
    }
  wait4push();
////adress 2 here
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  while(oneWire.search(addr)){
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
  #endif
  #ifdef two_probe
  eeprom_array_write_null(0);
  eeprom_array_write(dic1,0);
  eeprom_array_write_null(8);
  eeprom_array_write(dic2,8);
  eeprom_array_write_null(16);
  eeprom_array_write(dic3,16);
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
  #endif
  #ifdef ten_probe
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
  eeprom_array_write_null(40);
  eeprom_array_write(dic6,40);
  eeprom_array_write_null(48);
  eeprom_array_write(dic7,48);
  eeprom_array_write_null(56);
  eeprom_array_write(dic8,56);
  eeprom_array_write_null(64);
  eeprom_array_write(dic9,64);
  eeprom_array_write_null(72);
  eeprom_array_write(dic10,72);
  eeprom_array_write_null(80);
  eeprom_array_write(dic11,80);
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
  Serial.println();
  Serial.print(F("probe6: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic6[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic6[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe7: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic7[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic7[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe8: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic8[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic8[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe9: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic9[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic9[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe10: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic10[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic10[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  Serial.println();
  Serial.print(F("probe11: "));
  for( i = 0; i < 8; i++) {
      Serial.print(F("0x"));
      if (dic11[i] < 16) {
        Serial.print('0');
      }
      Serial.print(dic11[i], HEX);
      if (i < 8) {
        Serial.print(F(", "));
      }
  }
  #endif
  #ifdef four_probe
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
  #endif
  Serial.println(F(""));
  Serial.println(F("Done"));
}
//-----user defined function for delay in aquiring adresses--discoverOneWireDevices---//
void wait4push (void) {
  oneWire.reset_search();
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

/*----user defined function to log temp----*/
void logtemp(byte k)
{
  byte addr[8];
  byte j;
  for(j=0;j<8;j++)
 {
   addr[j]=EEPROM.read(j+k);
 } 
  float tempC = sensors.getTempC(addr); 
  //delay(250);

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

float readVcc() { 
long result; 
float voltage;
ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
delay(2); 
ADCSRA |= _BV(ADSC); 
while (bit_is_set(ADCSRA,ADSC)); 
result = ADCL; result |= ADCH<<8;
result = 1126400L / result;
voltage = result*0.001; 
return voltage; }

#ifdef bvcc
float readBVcc() {
float voltage;  
voltage = analogRead(voltPin);
voltage = (voltage / 1024) * 5.0;
return voltage;
}
#endif

/*------------user defined function to round float---------*/
void printFloat(float value, int places) {
  // this is used to cast digits 
  int digit;
  float tens = 0.1;
  int tenscount = 0;
  int i;
  float tempfloat = value;
 

  float d = 0.5;
  if (value < 0)
    d *= -1.0;
 
  for (i = 0; i < places; i++)
    d/= 10.0;    
  
  tempfloat +=  d;
  

  if (value < 0)
    tempfloat *= -1.0;
  while ((tens * 10.0) <= tempfloat) {
    tens *= 10.0;
    tenscount += 1;
  }

  
  if (value < 0)
    myFile.print('-');

  if (tenscount == 0)
    myFile.print(0, DEC);

  for (i=0; i< tenscount; i++) {
    digit = (int) (tempfloat/tens);
    myFile.print(digit, DEC);
    tempfloat = tempfloat - ((float)digit * tens);
    tens /= 10.0;
  }
  
  if (places <= 0)
    return;
  
  myFile.print('.');  
 
  for (i = 0; i < places; i++) {
    tempfloat *= 10.0; 
    digit = (int) tempfloat;
    myFile.print(digit,DEC);  
    // once written, subtract off that digit
    tempfloat = tempfloat - (float) digit; 
  }
}
