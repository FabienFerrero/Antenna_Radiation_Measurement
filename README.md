# Low Cost Antenna Return loss & Radiation Measurement method for IoT System

Author : Fabien Ferrero
Last Update : 15/04/2020

Objective : This repository integrates hardware and software for low-cost antenna radiation measurement between 800 and 950 MHz.
Two different methods with tutorial are detailed.
29/01/2020 : You will find in tutorial directory the workshop presented in The Things conference 2020 for "Antenna tuning" based on low-cost VNA.
15/04/2020 : You can find the new tutorial made for Virtual TTC
Video is available here : https://youtu.be/OxE_F2ohXu0

For measurment demo, I'm using the LoRa board presented on this github : https://github.com/FabienFerrero/UCA_Board
But this method can be adapted to any IoT device.

The first method is using the spectrum analyzer and a CW mode on the AUT

A tutorial is available here : https://github.com/FabienFerrero/Antenna_Radiation_Measurement/blob/master/Tutorial/Tutorial_Antenna_Measurement_Spectrum_2019_v1.pdf

The second method, which is less accurate, is using the RSSI of a LoRa receiver to performe the measurement

A tutorial is available here : https://github.com/FabienFerrero/Antenna_Radiation_Measurement/blob/master/Tutorial/Tutorial_Antenna_Measurement_RSSI_2019_v1.pdf


<img src="https://github.com/FabienFerrero/Antenna_Radiation_Measurement/blob/master/documents/picture/receiver.png">


<img src="https://github.com/FabienFerrero/Antenna_Radiation_Measurement/blob/master/documents/picture/ref_ant.png">

