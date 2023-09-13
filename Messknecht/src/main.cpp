// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */

#define INFLUXDB_CLIENT_DEBUG_ENABLE

#include "Configuration.h"
//#include "Datastore.h"
#include "adc.h"

#include "MessageOutput.h"

#include "NetworkSettings.h"
#include "influxwriter.h"
#include "NtpSettings.h"
#include "Utils.h"
#include "WebApi.h"
#include "defaults.h"
#include <Arduino.h>
#include <LittleFS.h>

void setup()
{
    // Initialize serial output
    Serial.begin(SERIAL_BAUDRATE);
#if ARDUINO_USB_CDC_ON_BOOT
    Serial.setTxTimeoutMs(0);
    delay(100);
#else
    while (!Serial)
        yield();
#endif
    MessageOutput.println();
    MessageOutput.println("Starting Messknecht");

    // Initialize file system
    MessageOutput.print("Initialize FS... ");
    if (!LittleFS.begin(false)) { // Do not format if mount failed
        MessageOutput.print("failed... trying to format...");
        if (!LittleFS.begin(true)) {
            MessageOutput.print("success");
        } else {
            MessageOutput.print("failed");
        }
    } else {
        MessageOutput.println("done");
    }

    // Read configuration values
    MessageOutput.print("Reading configuration... ");
    if (!Configuration.read()) {
        MessageOutput.print("initializing... ");
        Configuration.init();
        if (Configuration.write()) {
            MessageOutput.print("written... ");
        } else {
            MessageOutput.print("failed... ");
        }
    }
    if (Configuration.get().Cfg_Version != CONFIG_VERSION) {
        MessageOutput.print("migrated... ");
        Configuration.migrate();
    }
    CONFIG_T& config = Configuration.get();
    MessageOutput.println("done");


    // Initialize WiFi
    MessageOutput.print("Initialize Network... ");
    NetworkSettings.init();
    MessageOutput.println("done");
    NetworkSettings.applyConfig();

    // Initialize NTP
    MessageOutput.print("Initialize NTP... ");
    NtpSettings.init();
    MessageOutput.println("done");    

    // Initialize WebApi
    MessageOutput.print("Initialize WebApi... ");
    WebApi.init();
    MessageOutput.println("done");

    // Initialize ADC
    MessageOutput.print("Initialize ADC... ");
    ADConverter.init();
    MessageOutput.println("done");

    //Initialize Influxdb
    MessageOutput.print("Initialize InfluxDB... ");
    InfluxWriter.init();
    MessageOutput.println("done");

    // Initialize Display
    MessageOutput.print("Initialize Display... ");
    
    // Check for default DTU serial
    MessageOutput.print("Check for default DTU serial... ");
    if (config.Dtu_Serial == DTU_SERIAL) {
        MessageOutput.print("generate serial based on ESP chip id: ");
        uint64_t dtuId = Utils::generateDtuSerial();
        MessageOutput.printf("%0x%08x... ",
            ((uint32_t)((dtuId >> 32) & 0xFFFFFFFF)),
            ((uint32_t)(dtuId & 0xFFFFFFFF)));
        config.Dtu_Serial = dtuId;
        Configuration.write();
    }
    MessageOutput.println("done");    

    
}

void loop()
{
    NetworkSettings.loop();
    yield();

    InfluxWriter.loop();
    yield();
    
    ADConverter.loop();
    yield();
    
    WebApi.loop();
    yield();
    
    MessageOutput.loop();
    yield();
    
}