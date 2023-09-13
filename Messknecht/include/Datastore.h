// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <TimeoutHelper.h>
#include <Configuration.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class DatastoreClass {
public:
    DatastoreClass();
    void init();
    void loop();   
    float *data(){return _channelData;} 

private:
    TimeoutHelper _updateTimeout;
    SemaphoreHandle_t _xSemaphore;    
    float _channelData[CHAN_MAX_COUNT]{0};
};

extern DatastoreClass Datastore;