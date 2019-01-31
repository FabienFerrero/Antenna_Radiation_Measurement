/* 
 *  
 *  Copyright (c) 2018 Fabien Ferrero
 *
 *  Permission is hereby granted, free of charge, to anyone
 *  obtaining a copy of this document and accompanying files,
 *  to do whatever they want with them without any restriction,
 *  including, but not limited to, copying, modification and redistribution.
 *  NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 *  This example receive LoRa packet with frequency hooping information in the
 *  payload and change the frequency for the next packet.
 *  This code made for Heltec LoRa board 860-915MHz version
 *  It can be use to measure LoRa device  radiation performance and to tune the 
 *  device antenna
 *  
 */

// In this program, all RSSI are in tenth of dB

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <SPI.h>
#include <LoRa.h>
// #include "SSD1306.h"

SSD1306 display(0x3c, 4, 15); // Set the display

#define SS 18
#define RST 14
#define DI0 26

// LoRa Settings 
#define BAND 800E6
#define spreadingFactor 7 // Use of a low SF to speed the scanning process
#define SignalBandwidth 125E3


#define codingRateDenominator 8
#define interruptPin 0  // PIN 0 for interrupt with PRG button

long freq = 800e6; // Default frequency at initialisation
long freq_new = 800e6; // Default frequency at initialisation

long freq_max=950e6;
long freq_min=800e6;
long freq_step=1e6;

long max_freq=freq; // freq with max RSSI
long min_freq=freq; // freq with max RSSI

bool Pushdetected = false; // reset peak freq

long counter = -1; // number of received packet since the last init
int avg=1; // averaging counter
int RSSI = -1400;
float ming = 100;
float maxg = -100;
float gain=0;
int RSSI_array[100]; // Valuee are stored for averaging , Max averaging is 100
int ref_freq[] = {799e6, 850e6, 900e6, 950e6};
float ref_val[]={-3,17,18,3}; // Warning, all power values are in tenth of dB
int calib=0;
const byte nbpoint = 200;
int cal[nbpoint];  //array for cal

void gotPush(){   // reset peak freq on interrupt
 Pushdetected = true;
}
  
void setup() {

  // Define interrupt on PRG button to reset peak freq
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gotPush, CHANGE);
    
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);
  
  Serial.println("LoRa Receiver");
  
  display.drawString(5,5,"LoRa Receiver");
  display.display();
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);



  if (!LoRa.begin(BAND)) {
    display.drawString(5,25,"Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Init OK");
  
  Serial.print("LoRa Frequency: ");
  Serial.println(BAND);
  
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);
  
  display.drawString(5,25,"LoRa Init OK");
  display.drawString(5,45,"Wait @ "+(String)((int)(freq/1e6))+ "MHz");
  display.display();
}

void loop() {
  
  if(Pushdetected){ // reset Peak freq and max RSSI value
    Pushdetected = false;
    maxg=-100;
    ming=100;
    counter = -5; // start with count < 0 to add a small delay before the start of the measurement
    max_freq=0;
    min_freq=0;

    // Reset cal
    for (int i=0;i<nbpoint;i++){
    cal[i]=0;
    }       
  }

  calib = 0;
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packets        
    counter++;
    
    // read packet
    while (LoRa.available()) {
      String data = LoRa.readString();
      freq_new = data.toInt(); // data converted to int


          
    //RSSI calculation
      RSSI = 10*LoRa.packetRssi()+1570; // Extract raw value of packet rssi register and multiply by 10
      RSSI = (16*RSSI)/15; //value correction from the datasheet
      RSSI = RSSI -1570; // Re-Calculate value in dBm
      
      if(counter >= 0){
      RSSI_array[avg-1]=RSSI; // Store RSSI for averaging
      
      }
      
    long freq_MHz= freq / 1e6; // freq in MHz
    float raw = ((float)RSSI)/10;
    display.clear();    
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Raw " + (String)freq_MHz + "M: " + (String)raw + "dBm");
    display.display();

    if (freq != freq_new) {

    LoRa.setFrequency(freq_new); // change frequency
      
    RSSI = 0;
    for( int i=0; i<avg; i++ ) {
      
    
    RSSI = RSSI + RSSI_array[i];
    
      }
    RSSI = RSSI / avg;

    int n = (freq - freq_min)/freq_step;      
    gain = (RSSI-cal[n]);
      if (cal[n] == 0){
     int k=0;
     while (freq > ref_freq[k+1]){k++;}
     float num = (float)freq-ref_freq[k];
     float den = (float)ref_freq[k+1]-ref_freq[k];
     float ratiofreq = num/den;
     int gain_ref = ratiofreq * (ref_val[k+1]-ref_val[k]) + ref_val[k];
     
            
      cal[n] = RSSI-gain_ref; // Store data for calibration

      calib = 1;
      }
      gain = ((float)gain)/10;
    }

     if(counter > 0){      
    
    display.drawString(2, 15,"Gain:"+(String)gain + "dBi" + " Avg="+ avg);
    display.display();

if ((freq != freq_new) && (calib == 0) ) {
     if(gain > maxg && counter > 0){
      maxg=gain;
      max_freq=freq;
      }

  if(gain < ming && counter > 0){
      ming=gain;
      min_freq=freq;
      }  

}
    // print RSSI of packet
           
      long minfreq_MHz= min_freq / 1e6; // freq in MHz
      display.drawString(0,33,"Min:"+(String)minfreq_MHz+"MHz:"+(String)ming + "dBm");      
      display.display();
      }
    
   if (freq != freq_new) {
    Serial.print("{XYPLOT|DATA|Gain|");
    Serial.print(freq_MHz);
    Serial.print("|");
    Serial.print(gain);
    Serial.println("}");
   }

    
    long maxfreq_MHz= max_freq / 1e6; // freq in MHz
    display.drawString(0, 52, "Max:"+(String)maxfreq_MHz+"MHz:"+(String)maxg + "dBm");
    display.display();
   

    if (freq != freq_new) {
      // new frequency measurement
        avg=1;
      }
      else{
        avg++; // averaging
         }

    freq=freq_new;

    
    }
  }
}
