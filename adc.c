/**
 * @file adc.c
 *
 *
 * NTC B3950 Thermistor
 * the formula for temp in kelvin is
 *                1
 * T = ----------------------------
 *    1/To + (1/beta) * ln(Rt/Ro)

/*********************
 *      INCLUDES
 *********************/
#include "adc.h"

int init_adc()
{
//Just for phun 
}
double get_temp(int AdcPin){
 double Vout, Rt = 0;
 double T, Tc = 0;
 double adc = 0;
 adc = analogRead(ThermistorPin);
 adc = ADC_LUT[int(adc)]; 
 Vout = adc * Vs/adcMax;
 Rt = R1 * Vout / (Vs - Vout);
 
 T = 1/(1/To + log(Rt/Ro)/Beta);    // Temperature in Kelvin
 Tc = T - 273.15;                   // Celsius
 if (Tc > 0) {
	 return Tc;
 } else {
	 return -1;
 }
}
int get_fwd(int FwdPin) {
}
int get_rev(int RevPin){
}
int get_current(int CurrentPin){
}
int get_voltage(int VoltagePin){
}

