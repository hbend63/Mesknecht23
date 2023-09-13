// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 Thomas Basler and others
 */
#include "Datastore.h"
#include "adc.h"


#define DAT_SEMAPHORE_TAKE() \
    do {                     \
    } while (xSemaphoreTake(_xSemaphore, portMAX_DELAY) != pdPASS)
#define DAT_SEMAPHORE_GIVE() xSemaphoreGive(_xSemaphore)

DatastoreClass Datastore;

DatastoreClass::DatastoreClass()
{
    _xSemaphore = xSemaphoreCreateMutex();
    DAT_SEMAPHORE_GIVE(); // release before first use
}

void DatastoreClass::init()
{
    _updateTimeout.set(1000);    
}

void DatastoreClass::loop()
{
    /* DAT_SEMAPHORE_TAKE();
    ADConverter.updateScale();
    for (int channel = 0; channel < CHAN_MAX_COUNT; channel++) {        
        _channelData[channel]=ADConverter.scaleData()[channel];
    }  
    DAT_SEMAPHORE_GIVE(); 
    _updateTimeout.reset();   */ 
}

