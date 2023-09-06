// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023
 */

#include "influxwriter.h"
#include "MessageOutput.h"
#include "NetworkSettings.h"
#include "adc.h"

#define DAT_SEMAPHORE_TAKE() \
    do {                     \
    } while (xSemaphoreTake(_xSemaphore, portMAX_DELAY) != pdPASS)
#define DAT_SEMAPHORE_GIVE() xSemaphoreGive(_xSemaphore)

InfluxWriterClass InfluxWriter;

InfluxWriterClass::InfluxWriterClass()
{
    _xSemaphore = xSemaphoreCreateMutex();
    DAT_SEMAPHORE_GIVE(); // release before first use
}

InfluxWriterClass::~InfluxWriterClass()
{
}

void InfluxWriterClass::init()
{
    Configuration.read();
    isConnected = false;
    _updateTimeout.set(1000);
    
    for (int channel=0; channel < CHAN_MAX_COUNT; channel++)
      _lastTimestamp[channel] = millis();
}

void InfluxWriterClass::run()
{   
   DAT_SEMAPHORE_TAKE();
        
        for (int channel = 0; channel < CHAN_MAX_COUNT; channel++) 
        {
            
            if (config.AD_Converter.ADChannels[channel].takt_s > 0) 
            {
                
                if (millis() - _lastTimestamp[channel] > config.AD_Converter.ADChannels[channel].takt_s * 1000) {
                    Point sensor("bioknecht");
                    String id(config.AD_Converter.ADChannels[channel].channelId);
                    id.concat("_");
                    id.concat(config.AD_Converter.ADChannels[channel].Name);
                    sensor.addTag("Sensor-Channel", id);
                    sensor.addField("value", ADConverter.scaleDat(channel));

                    // Print what are we exactly writing
                    MessageOutput.print("Writing: ");
                    MessageOutput.println(client.pointToLineProtocol(sensor));
                    // Write point
                    if (!client.writePoint(sensor)) {
                        MessageOutput.print("InfluxDB write failed: ");
                        MessageOutput.println(client.getLastErrorMessage());
                    }
                    _lastTimestamp[channel] = millis();                    
                }
                
            }
        }
        DAT_SEMAPHORE_GIVE();
}

void InfluxWriterClass::loop()
{
    // 1. Connect to InfluxDB
    if (!isConnected) {
        if (millis() - _lastTimestamp[0] > 100) {
            if (NetworkSettings.isConnected()) {
                connect();
            }
            _lastTimestamp[0] = millis();
        }
    }
    // 2. Do Job
    else run();
}

void InfluxWriterClass::connect()
{     
    client.setConnectionParams(config.InfluxDB_Url, config.InfluxDB_Org, config.InfluxDB_Bucket, config.InfluxDB_Token);
    MessageOutput.print("Influx-Connection ");
    if (client.validateConnection()) {
        MessageOutput.print("Connected to InfluxDB: ");
        MessageOutput.println(client.getServerUrl());        
        isConnected = true;
    } else {
        MessageOutput.print("InfluxDB connection failed: ");
        MessageOutput.println(client.getLastErrorMessage());
    }
}
