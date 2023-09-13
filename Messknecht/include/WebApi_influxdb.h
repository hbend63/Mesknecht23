// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ESPAsyncWebServer.h>

#define MQTT_JSON_DOC_SIZE 10240

class WebApiInfluxClass {
public:
    void init(AsyncWebServer* server);
    void loop();

private:    
    void onInfluxAdminGet(AsyncWebServerRequest* request);
    void onInfluxAdminPost(AsyncWebServerRequest* request);   

    AsyncWebServer* _server;
};