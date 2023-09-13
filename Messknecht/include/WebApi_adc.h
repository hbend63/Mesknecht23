// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ESPAsyncWebServer.h>

#define MQTT_JSON_DOC_SIZE 10240

class WebApiADCClass {
public:
    void init(AsyncWebServer* server);
    void loop();

private:
    void onADCRawValuesGet(AsyncWebServerRequest* request);
    void onADCScaleValuesGet(AsyncWebServerRequest* request);
    void onADCAdminGet(AsyncWebServerRequest* request);
    void onADCAdminPost(AsyncWebServerRequest* request);
    //String getTlsCertInfo(const char* cert);

    AsyncWebServer* _server;
};