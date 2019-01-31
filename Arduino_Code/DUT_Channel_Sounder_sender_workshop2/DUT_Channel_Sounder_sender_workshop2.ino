//#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
//#include<Arduino.h>


#define SS 10
#define RST 8
#define DI0 2
#define BAND 866E6
#define spreadingFactor 7
#define SignalBandwidth 125E3

#define preambleLength 8
#define codingRateDenominator 8
#define Pow 2

#define SHOW_DEBUGINFO    // Uncomment if you want to see info on Serial

int counter = 0;
int avg = 1;
long freq_start = 800e6;
long freq_stop = 950e6;
long freq_step = 1e6;

long next_freq = freq_start;
long curr_freq = freq_start;

void setup() {
    
  LoRa.setPins(SS,RST,DI0);
  Serial.begin(115200); 
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(SignalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
  LoRa.setTxPower(Pow, 1);
   
  delay(1000);
  
  #ifdef SHOW_DEBUGINFO 
  Serial.println("LoRa Sender");
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  Serial.print("LoRa power: ");
  Serial.println(Pow);
  Serial.println("LoRa Initial OK!");
  delay(1000);
  #endif  
  
}

void loop() {

  LoRa.setFrequency(next_freq); // change frequency

  for (int i=1;i<=avg;i++){

  #ifdef SHOW_DEBUGINFO
  Serial.print("Sending packet: ");
  Serial.println((String)next_freq + " MHz");
  #endif

  if (i==(avg)){  
  curr_freq = curr_freq + freq_step ; // increment of the next 
  
  }
  
  if(next_freq > freq_stop){  // when freq arrived at freq stop, it restart at freq_start
  curr_freq = freq_start;
      }
  next_freq = curr_freq;
  
  // send packet
  LoRa.beginPacket();
    LoRa.print(next_freq);
  LoRa.endPacket();  
  counter++;
  
  digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(10); // wait for a second
  digitalWrite(13, LOW); // turn the LED off by making the voltage LOW
  delay(90); // wait for a second
}

 
  

}
