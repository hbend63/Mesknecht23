// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 Thomas Basler and others
 */
#include "adc.h"
#include "MCP_ADC.h"
#include "MessageOutput.h"

#define DAT_SEMAPHORE_TAKE() \
    do {                     \
    } while (xSemaphoreTake(_xSemaphore, portMAX_DELAY) != pdPASS)
#define DAT_SEMAPHORE_GIVE() xSemaphoreGive(_xSemaphore)


MCP3208 mcp1(19, 15, 18); // use SWSPI


ADCClass ADConverter;

ADCClass::ADCClass()
{
  _xSemaphore = xSemaphoreCreateMutex();
   DAT_SEMAPHORE_GIVE(); // release before first use
}

float ADCClass::scaleDat(uint8_t channel)
{ 
  _error = !(channel <= CHAN_MAX_COUNT);
  if (_error) {
    return 0;
  }  
  uint8_t channel_int=channel-1;
  float m = (1.0*config.AD_Converter.ADChannels[channel_int].sclMax-config.AD_Converter.ADChannels[channel_int].sclMin)/(config.AD_Converter.ADChannels[channel_int].dglMax-config.AD_Converter.ADChannels[channel_int].dglMin);
  float value = m*_channelRawData[channel]+config.AD_Converter.ADChannels[channel_int].sclMax-m*config.AD_Converter.ADChannels[channel_int].dglMax;
  
  // MessageOutput.print("raw: ");
  // MessageOutput.println(_channelRawData[channel]);  
  // MessageOutput.print("val: ");
  // MessageOutput.println(value);
  // Test Git 
  return value;
}

u_int32_t ADCClass::rawData(u_int8_t channel)
{  _error = !(channel <= CHAN_MAX_COUNT);
   if (_error) return 0;    
  // MessageOutput.print("rae CH: ");
  // MessageOutput.println(channel);
   return _channelRawData[channel];   
}

void ADCClass::readADC()
{
  int data[ADC_CHANNEL_COUNT]{0}; 
  for (int i=0; i<5; i++)    
  {     
    for (int channel = 0; channel < ADC_CHANNEL_COUNT; channel++) 
       data[channel]+=mcp1.analogRead(channel);
    delayMicroseconds(1);                   
  }  
  for (int channel = 0; channel < ADC_CHANNEL_COUNT; channel++)
     _channelRawData[channel]=data[channel]/5; 
  
  // Korrektur CH1 bis CH7 und Spannung als virtueller Digitalwert
  
  //MessageOutput.print("CH 0: "); MessageOutput.println(_channelRawData[0]);

  u_int32_t Uref = ADC_REF_VOLTAGE*adcMax/_channelRawData[0];
  //MessageOutput.print("Uref: "); MessageOutput.println(Uref);
  for (int channel = 1; channel < ADC_CHANNEL_COUNT; channel++)
  {     
     u_int32_t Uakt = Uref*_channelRawData[channel]/adcMax;     
     //MessageOutput.print("CH "); MessageOutput.print(channel);
     //MessageOutput.print(" akt: "); MessageOutput.print(_channelRawData[channel]);
     _channelRawData[channel]=Uakt;
     //MessageOutput.print(" val: "); MessageOutput.println(Uakt);
  }  
}

void ADCClass::init()
{      
    mcp1.begin(2);
    mcp1.setSPIspeed(4000000);
    _updateTimeout.set(1000); 
    _lastTimestamp=millis();
}

// Take a new value every Second (can be tuned...)
void ADCClass::loop()
{  
    if (millis()-_lastTimestamp > 1000)
    {
      DAT_SEMAPHORE_TAKE();
      readADC();
      _lastTimestamp=millis();
      DAT_SEMAPHORE_GIVE();
    }
}



