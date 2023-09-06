// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <TimeoutHelper.h>
#include <Configuration.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Configuration.h>
#include <defaults.h>

class ADCClass {
public:  
    ADCClass();  
    void init();
    void loop(); 
    float scaleDat(uint8_t channel);
    u_int16_t rawData(uint8_t channel);     
    bool errorOccured(){return _error;}
private:  
    bool _error{false}; 
    TimeoutHelper _updateTimeout;
    uint32_t _lastTimestamp;
    SemaphoreHandle_t _xSemaphore;    
    CONFIG_T& config = Configuration.get();
    u_int16_t _channelRawData[ADC_CHANNEL_COUNT]{0};
    void readADC();
};

extern ADCClass ADConverter;