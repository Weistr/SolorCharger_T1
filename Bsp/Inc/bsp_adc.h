#ifndef _BSP_ADC_H
#define _BSP_ADC_H
#include "main.h"

#define batteryADCdata	adc_buf[0]
#define vrefADCdata		adc_buf[4]


void adc_scan(void);
void adc_init(void);

#endif
