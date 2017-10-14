#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiUDP.h>
#include <Wire.h>

#include"apprx.h"

unsigned long currentTime = 0;
unsigned long previousTime = 0;

const char *ssid = "ESPap";
const char *password = "1234567890";

// PPM OUT //
const boolean   CFG_IS_RECEIVER = false;     //true=receiver, false=transmitter
const int       CFG_PROTOCOL = 0;           //0=PPM/Timer0, 1=PPM/bitbang, 2=MSP (Multiwii)
const int       CFG_PPM_IN = 14;            //pin number for PPM input, can be same as CFG_PPM_OUT
const int       CFG_PPM_OUT = 14;           //pin number for PPM output
const int       CFG_MAX_CHANNELS = 6;      //limit channels in ppm output, cleanflight=12

volatile unsigned int pulses[16] = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1000, 2000 } ;
volatile int pulseIndex = 0;
volatile unsigned long pulseStart = 0;
volatile boolean pulseFlag = false;


inline void timer0_rearm(uint32_t us) {
  us = clockCyclesPerMicrosecond() * us;
  timer0_write(ESP.getCycleCount() + us);
}

/*
   timing diagram of ppm pulse train
         0.3ms                 1-2ms              GAP 30ms
   PPM """||"""""""""||""""||"""""""""||""""""""""""""""""""""""""""""""||"
   S1 ____|""""""""""|__________________________________________________|""
   S2 _______________|"""""|_______________________________________________
   S3 _____________________|""""""""""|____________________________________
*/
void sendPpmRxInt0() {
//  if (errorCount > errorLimit) {
//    digitalWrite(CFG_PPM_OUT, HIGH);
//    timer0_rearm(50);
//    return;
//  }

  //raising edge?
  if (!pulseFlag) {
    timer0_rearm(300);
    digitalWrite(CFG_PPM_OUT, LOW);
    pulseFlag = true;
    return;
  }

  //reached gap?
  if (pulseIndex >= CFG_MAX_CHANNELS) {
    timer0_rearm(6640);
    digitalWrite(CFG_PPM_OUT, HIGH);
    pulseFlag = false;
    pulseIndex = 0;
    return;
  }

  //prepare next pulse
  if (pulses[pulseIndex] > 300) {
    timer0_rearm(pulses[pulseIndex] - 300);
  } else {
    timer0_rearm(1200);
  }

  digitalWrite(CFG_PPM_OUT, HIGH);
  pulseFlag = false;
  pulseIndex++;
}

void initRx() {
//  os_timer_disarm(&rxTimer);

  switch (CFG_PROTOCOL) {
    case 0: //PPM OUT, timer0
      pinMode(CFG_PPM_OUT, OUTPUT);
      timer0_isr_init();
      timer0_attachInterrupt(sendPpmRxInt0);
      timer0_rearm(10000);
      break;

//    case 2: //MSP
//      os_timer_setfn(&rxTimer, sendMspRx, NULL);
//      os_timer_arm(&rxTimer, 20, 1);
//      break;
  }
}



void setup() {
  Serial.begin(115200);
  Serial.println("ESP_RC");

//  WiFi.mode(WIFI_STA);
//  delay(50);
//  WiFi.disconnect();
//  delay(200);
//  pinMode(LED, OUTPUT);
//  delay(200);
//  sprintf(accessPointName, "BDrone-%lu", ESP.getChipId());
//  WiFi.softAP(accessPointName, "12345678");
//  delay(50);

WiFi.softAP(ssid, password);

  accessPointName[DEFAULT_SSID_LENGTH - 1] = {'\0'};
//  WiFi.mode(WIFI_AP_STA);
//  delay(50);
  udp.begin(localPort);
  delay(50);
  #ifdef Print_Debug
    Serial.println("");
    Serial.print("RemoteTest Access Point: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("UDP Port: ");
    Serial.println(localPort);
    Serial.println();
  #endif    
  
  delay(50);
  udp.begin(localPort);
  delay(50);

  initRx();
  
  delay(2000);
  
  // put your setup code here, to run once:

}

void loop() {
//   put your main code here, to run repeatedly:
  currentTime = micros();

  if (currentTime - previousTime >= 10000) 
  {
  computeRC();
  pulses[0] = CH_AIL;
  pulses[1] = CH_ELE;
  pulses[2] = CH_THR;
  pulses[3] = CH_RUD;
  pulses[4] = AUX_1;
  pulses[5] = AUX_2;
  
  Serial.print(CH_THR);Serial.print("\t");
  Serial.print(CH_AIL);Serial.print("\t"); 
  Serial.print(CH_ELE);Serial.print("\t");
  Serial.print(CH_RUD);Serial.print("\t");
  Serial.print(AUX_1);Serial.print("\t");
  Serial.print(AUX_2);Serial.print("\t");
  
  Serial.println("");
  previousTime = currentTime;
  }
  
  
}
