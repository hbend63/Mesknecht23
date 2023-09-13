// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <adc.h>

class WebApiWsLiveClass {
public:
    WebApiWsLiveClass();
    void init(AsyncWebServer* server);
    void loop();

private:
    void generateJsonResponse(JsonVariant& root);
    
    void onLivedataStatus(AsyncWebServerRequest* request);
    void onWebsocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);

    AsyncWebServer* _server;
    AsyncWebSocket _ws;

    uint32_t _lastWsPublish = 0;
    uint32_t _lastInvUpdateCheck = 0;
    uint32_t _lastWsCleanup = 0;
    //uint32_t _newestInverterTimestamp = 0;
};