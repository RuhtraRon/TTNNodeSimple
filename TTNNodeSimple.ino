#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>
#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include <ArduinoJson.h>

SoftwareSerial softSerial(13, 15); // RX, TX

#define debugSerial Serial
#define loraSerial softSerial

#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D

TheThingsNetwork ttn;

/* Node 1 */
const byte devAddr[4] = { 0x00, 0x1A, 0x5F, 0xBA };
const byte appSKey[16] = { 0xAD, 0x50, 0xB9, 0xE8, 0x9D, 0x46, 0xA5, 0xC8, 0x97, 0x74, 0x74, 0x48, 0x3E, 0x00, 0xB6, 0x15 };
const byte nwkSKey[16] = { 0xA1, 0xC1, 0xF3, 0x7F, 0x92, 0xD5, 0x49, 0xF7, 0xC7, 0x92, 0xDE, 0x71, 0x86, 0x69, 0x3E, 0xB8 };

/* Node 2 
const byte devAddr[4] = { 0x7A, 0xFD, 0xAD, 0x24 };
const byte appSKey[16] = { 0xFB, 0x9F, 0xF5, 0x04, 0x47, 0xF8, 0x5C, 0x68, 0x2D, 0xDD, 0xD6, 0x51, 0x19, 0x68, 0x4E, 0x27 };
const byte nwkSKey[16] = { 0x27, 0x28, 0x73, 0xCB, 0x96, 0xE1, 0x88, 0x1F, 0x40, 0xA4, 0xCB, 0xAB, 0xB9, 0x31, 0xA4, 0xEB };
*/

/* Node 3 
const byte devAddr[4] = { 0xF2, 0x20, 0xAE, 0xAB };//{ 0x7A, 0xFD, 0xAD, 0x24 };
const byte appSKey[16] = { 0xA8, 0xBC, 0x92, 0x76, 0xCB, 0x43, 0x16, 0x6F, 0x38, 0xB4, 0xC5, 0x86, 0xE9, 0x56, 0xA8, 0xFC };
const byte nwkSKey[16] = { 0xDD, 0x76, 0xD0, 0x47, 0xB0, 0xB7, 0x83, 0x93, 0xAB, 0x73, 0x7D, 0xC1, 0x5B, 0xD4, 0xFD, 0x97 };
*/

int counter = 1;

String readDevice(String cmd){
  loraSerial.println(cmd);
  String lineRx = loraSerial.readStringUntil('\n');
  return lineRx.substring(0,lineRx.length()-1);
}

void ttnPost(String body){
  debugSerial.println("Sleeping for 2 seconds before starting sending out test packets.");
  delay(2000);
  //body = "Test";
  // Create a buffer with three bytes  
  //byte data[3] = { 0x01, 0x02, 0x03 };
  byte data[body.length()+1];
  body.getBytes(data, body.length()+1);
  // Send it to the network
  int result = ttn.sendBytes(data, sizeof(data));
  debugSerial.print("TTN Message Sent: ");
  debugSerial.println(counter);
  String msgString = "TTN Sent";
  msgString += "\nBytes:"+String(sizeof(data));
  msgString += "\nTries:"+String(counter);
  if (result < 0){
    msgString += "\nFailure";
  }
  else {
    debugSerial.print("Success but no downlink");
    msgString += "\nSuccess";
  }
  if (result > 0) {
    debugSerial.println("Received " + String(result) + " bytes");
    // Print the received bytes
    msgString += "\n";
    for (int i = 0; i < result; i++) {
      debugSerial.print(String(ttn.downlink[i]) + " ");
      msgString += String(ttn.downlink[i]);
    }
    debugSerial.println();
  }
}

void setup()
{
  debugSerial.begin(115200);
  loraSerial.begin(57600);//57600,9600
  
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  ttn.init(loraSerial, debugSerial);
  ttn.reset();
  ttn.personalize(devAddr, nwkSKey, appSKey);
  
  #ifdef DEBUG
  ttu.debugStatus();
  #endif
  Serial.println("Setup for The Things Network.");
  delay(100);
}

void loop()
{
  debugSerial.println();
//  debugSerial.println();
//  debugSerial.println("Device Information");
//  debugSerial.println();
//  ttn.showStatus();

  ttnPost(String(counter));
  debugSerial.println();
  counter++;
  delay(10000);
}
