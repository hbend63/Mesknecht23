// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <TimeoutHelper.h>
#include <Configuration.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Configuration.h>
#include <InfluxDbClient.h>

class InfluxWriterClass {
public:  
    InfluxWriterClass();  
    ~InfluxWriterClass();  
    void init();
    void loop();  
    void connect();   
    
private:
    const CONFIG_T& config = Configuration.get();       
    InfluxDBClient client; 
    TimeoutHelper _updateTimeout;
    SemaphoreHandle_t _xSemaphore; 
    uint64_t _lastTimestamp[CHAN_MAX_COUNT];
    bool isConnected;   
    void run();   
};

extern InfluxWriterClass InfluxWriter;