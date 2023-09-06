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
  _error = !(channel < CHAN_MAX_COUNT+1);
  if (_error) {
    return 0;
  }  
  MessageOutput.print("CH: ");
  MessageOutput.println(channel);   
  MessageOutput.print("RAW: ");
  MessageOutput.println(_channelRawData[channel]);
  MessageOutput.print("DLG-MAX: ");
  MessageOutput.println(config.AD_Converter.ADChannels[channel].dglMax);

  double faktor = (_channelRawData[channel+1]-config.AD_Converter.ADChannels[channel].dglMin)/(1.0*config.AD_Converter.ADChannels[channel].dglMax-config.AD_Converter.ADChannels[channel].dglMin);
  double result = (config.AD_Converter.ADChannels[channel].sclMax-config.AD_Converter.ADChannels[channel].sclMin)*faktor+config.AD_Converter.ADChannels[channel].sclMin;
  return result;
}

u_int16_t ADCClass::rawData(u_int8_t channel)
{  _error = !(channel < CHAN_MAX_COUNT);
   if (_error) return 0;    
   return _channelRawData[channel+1];   
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
}

void ADCClass::init()
{      
    mcp1.begin(2);
    mcp1.setSPIspeed(4000000);
    _updateTimeout.set(1000); 
    _lastTimestamp=millis();
}

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



