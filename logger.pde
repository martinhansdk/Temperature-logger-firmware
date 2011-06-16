// TODO: 
//  * write a program to extract the data into a CSV file
//  * use the AT24C256'es
//  * find out if I should use Alarm.delay() or not



#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <EDB.h>
#include <Messenger.h>
#include <DS1621.h>
#include "logevent.h"


// pins
int greenLEDPin = 2; // set to 2 when using the logger shield
int redLEDPin = 3;
int doorPin = 4;

// status information
int running = 0;
time_t period = 60;
AlarmId timer;

// temperature sensors
DS1621 indoor_sensor  = DS1621((0x90 >> 1) | 0);
DS1621 outdoor_sensor = DS1621((0x90 >> 1) | 1);

// prototypes
void sample_and_store();
void greenLEDoff();
void EEwriter(unsigned long address, byte data);
byte EEreader(unsigned long address);
void messageCompleted();
void print_status();
void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data );
byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress );
int truncate_temp(int tC);
int untruncate_temp(int tC);
void print_hr_temperature(int tC);

// Create an EDB object with the appropriate write and read handlers
#define TABLE_SIZE 65536
EDB db(&EEwriter, &EEreader);
EDB_Status last_db_status = EDB_Status(100);

// Instantiate Messenger object with the message function and the default separator (the space character)
#define MAXSIZE 30 
char string[MAXSIZE]; 
Messenger message = Messenger(); 


void setup() {
  // setup led output
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);
  
  // setup door status pin
  pinMode(doorPin, INPUT);
  digitalWrite(doorPin, HIGH); // enable pullup resistor

  // configure temperature sensors
  indoor_sensor.startConversion(false);         // stop if presently set to continuous
  indoor_sensor.setConfig(DS1621::ONE_SHOT);    // 1-shot mode

  outdoor_sensor.startConversion(false);        // stop if presently set to continuous
  outdoor_sensor.setConfig(DS1621::ONE_SHOT);   // 1-shot mode
  

  // setup logging
  timer = Alarm.timerRepeat(period, sample_and_store);
  Alarm.disable(timer); // don't start yet
  
  // initialize serial communication:
  Serial.begin(9600);
  message.attach(messageCompleted);
  
  // initialize I2C master
  Wire.begin();

  // initalize data storage
  last_db_status=db.open(0); 

  Serial.println("started");
}


void loop () {
  // process serial data
  while ( Serial.available( ) ) { message.process(Serial.read());
/*      int chr = Serial.read( );
      //Serial.write((byte)chr); // echo
      message.process(chr); */
  }
  Alarm.delay(0);
}


void sample_and_store () {
  LogEvent logEvent;  

  logEvent.temperature_indoor  = truncate_temp(indoor_sensor.getHrTemp());
  logEvent.temperature_outdoor = truncate_temp(outdoor_sensor.getHrTemp());
  logEvent.door_open = digitalRead(doorPin);

  last_db_status=db.appendRec(EDB_REC logEvent.pack()); 
  
  // turn green led on for one second to show sampling happened
  digitalWrite(greenLEDPin, HIGH);
  Alarm.timerOnce(1, greenLEDoff);
  
  // show database status on red led
  if(last_db_status==EDB_OK)
    digitalWrite(redLEDPin, LOW);
  else
    digitalWrite(redLEDPin, HIGH);
}

void greenLEDoff () {
   digitalWrite(greenLEDPin, LOW);
}

// The read and write handlers for the database
// we have two EEPROMs on the board
void EEwriter(unsigned long address, byte data)
{
  int deviceaddress;
  if(address < 32768) {
    deviceaddress=0x50;
  } else {
    deviceaddress=0x52;
    address=address-32768; 
  }
      
  i2c_eeprom_write_byte( deviceaddress, address, data );
}

byte EEreader(unsigned long address)
{
  int deviceaddress;
  if(address < 32768) {
    deviceaddress=0x50;
  } else {
    deviceaddress=0x52;
    address=address-32768; 
  }
      
  return i2c_eeprom_read_byte( deviceaddress, address );
}

// Define messenger function
void messageCompleted() {
   
  while ( message.available() ) {
    Serial.println("");
    if ( message.checkString("status") ) {
      Serial.println("OK");      
    } else if ( message.checkString("settime") ) {
      long int newtime = message.readLong();
      setTime(newtime);
      Serial.println("OK");
    } else if ( message.checkString("initdb") ) {
      last_db_status=db.create(0, TABLE_SIZE, sizeof(PackedLogEvent)); 
      Serial.println("OK");     
    } else if ( message.checkString("cleardb") ) {
      db.clear();
      Serial.println("OK");      
    } else if ( message.checkString("start") ) {
      Alarm.enable(timer);
      running=1;
      Serial.println("OK");      
    } else if ( message.checkString("stop") ) {
      Alarm.disable(timer);
      running=0;
      Serial.println("OK");
    } else if ( message.checkString("setperiod") ) {      
      period = message.readLong();
      Alarm.write(timer, period); // this also enables the timer, which may not be what we want
      if(running)
        Alarm.enable(timer);
      else
        Alarm.disable(timer);
        
      Serial.println("OK");      
    } else if ( message.checkString("getdata") ) {
      LogEvent logEvent;
      PackedLogEvent packedLogEvent;

      long unsigned int count = db.count();
      Serial.print("records="); Serial.println(count);
      Serial.println("nr;temperature_indoor;temperature_outdoor;door");
      for(long unsigned int i=1 ; i<count+1 ; i++) {
        db.readRec(i, EDB_REC packedLogEvent);
	logEvent = packedLogEvent.unpack();
        Serial.print(i); 
	Serial.print(";"); print_hr_temperature(untruncate_temp(logEvent.temperature_indoor));
	Serial.print(";"); print_hr_temperature(untruncate_temp(logEvent.temperature_outdoor));
	Serial.print(";"); Serial.print(logEvent.door_open);
	Serial.println();
      }
      
      // don't print the status
      continue;
    } else {
      message.copyString(string,MAXSIZE);
      Serial.print("Unknown command: "); 
      Serial.println(string); // Echo the string
    }
    print_status();
  }
}


void print_status() {
  Serial.print("Time: "); Serial.println(now(), DEC);
  Serial.print("Status: "); Serial.println(running ? "Running" : "Stopped");
  Serial.print("Period: "); Serial.print(period); Serial.println(" seconds");
  Serial.print("Database entries: "); Serial.print(db.count(), DEC); Serial.print(" out of a maximum of "); Serial.println(db.limit(), DEC);
  Serial.print("Last db status: "); 
  switch(last_db_status) {
     case 100: Serial.println("none"); break;
     case EDB_OK: Serial.println("OK"); break;
     case EDB_OUT_OF_RANGE: Serial.println("out of range"); break;
     case EDB_TABLE_FULL: Serial.println("table full"); break;
     default: Serial.println("unknown");
  }
  Serial.print("Indoor temperature: "); print_hr_temperature(indoor_sensor.getHrTemp());
  Serial.print("Outdoor temperature: "); print_hr_temperature(outdoor_sensor.getHrTemp());
  Serial.print("Door open: "); Serial.println(digitalRead(doorPin));
}

// the temperature is stored in the EEPROM with a resolution of 11 bits
// but the DS1621 delivers an integer where 
int truncate_temp(int tC) {
  return tC >> 5;
}

int untruncate_temp(int tC) {
  return tC << 5;
}


void print_hr_temperature(int tC) {
  if (tC < 0) {
    tC = -tC;                                   // fix for integer division
    Serial.print("-");                          // indicate negative
  }
  
  int tFrac = tC % 100;                             // extract fractional part
  tC /= 100;                                    // extract whole part
  
  Serial.print(tC);
  Serial.print(".");
  if (tFrac < 10)
    Serial.print("0");
  Serial.println(tFrac);
}


//----------------- low level functions for talking to the AT24C256 eeproms

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) {
  int rdata = data;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.send(rdata);
  Wire.endTransmission();
  Alarm.delay(11); // give the eeprom time to finish one write before issuing another one. Value was obtained from the datasheet.
  
}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.send((int)(eeaddress >> 8)); // MSB
  Wire.send((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.receive();
  return rdata;
}

