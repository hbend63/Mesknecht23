// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_ws_live.h"
#include "Configuration.h"
#include "MessageOutput.h"
#include "WebApi.h"
#include "defaults.h"
#include <AsyncJson.h>

WebApiWsLiveClass::WebApiWsLiveClass()
    : _ws("/livedata")
{
}

void WebApiWsLiveClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    using std::placeholders::_5;
    using std::placeholders::_6;

    _server = server;
    _server->on("/api/livedata/status", HTTP_GET, std::bind(&WebApiWsLiveClass::onLivedataStatus, this, _1));

    _server->addHandler(&_ws);
    _ws.onEvent(std::bind(&WebApiWsLiveClass::onWebsocketEvent, this, _1, _2, _3, _4, _5, _6));
}

void WebApiWsLiveClass::loop()
{
    // see: https://github.com/me-no-dev/ESPAsyncWebServer#limiting-the-number-of-web-socket-clients
    if (millis() - _lastWsCleanup > 1000) {
        _ws.cleanupClients();
        _lastWsCleanup = millis();
    }

    // do nothing if no WS client is connected
    if (_ws.count() == 0) {
        return;
    }

    if (millis() - _lastInvUpdateCheck < 1000) {
        return;
    }
    _lastInvUpdateCheck = millis();

     // Update on every 10 seconds
    if (millis() - _lastWsPublish > (2 * 1000) ) {

        try {
            DynamicJsonDocument root(40960);
            JsonVariant var = root;
            generateJsonResponse(var);

            String buffer;
            if (buffer) {
                serializeJson(root, buffer);

                if (Configuration.get().Security_AllowReadonly) {
                    _ws.setAuthentication("", "");
                } else {
                    _ws.setAuthentication(AUTH_USERNAME, Configuration.get().Security_Password);
                }
                _ws.textAll(buffer);
            }

        } catch (std::bad_alloc& bad_alloc) {
            MessageOutput.printf("Call to /api/livedata/status temporarely out of resources. Reason: \"%s\".\r\n", bad_alloc.what());
        }

        _lastWsPublish = millis();
    }
}

void WebApiWsLiveClass::generateJsonResponse(JsonVariant& root)
{    
    const CONFIG_T& config = Configuration.get();
    JsonArray channels = root.createNestedArray("datavalues");    
    for (int channel = 0; channel < CHAN_MAX_COUNT; channel++) {
        JsonObject chanData = channels.createNestedObject();   
        chanData["id"] = channel+1; 
        chanData["name"] = config.AD_Converter.ADChannels[channel].Name; 
        if (config.AD_Converter.ADChannels[channel].takt_s==0)
           chanData["value"] = 0;
        else
        {
           float value = 1.0*ADConverter.scaleDat(channel+1);  
           chanData["value"] = value;           
        }
        chanData["unit"] = config.AD_Converter.ADChannels[channel].Unit; 
        chanData["takt"] = config.AD_Converter.ADChannels[channel].takt_s;
    }  
}


void WebApiWsLiveClass::onWebsocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
{
    if (type == WS_EVT_CONNECT) {
        char str[64];
        snprintf(str, sizeof(str), "Websocket: [%s][%u] connect", server->url(), client->id());
        MessageOutput.println(str);
    } else if (type == WS_EVT_DISCONNECT) {
        char str[64];
        snprintf(str, sizeof(str), "Websocket: [%s][%u] disconnect", server->url(), client->id());
        MessageOutput.println(str);
    }
}

void WebApiWsLiveClass::onLivedataStatus(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentialsReadonly(request)) {
        return;
    }

    try {
        AsyncJsonResponse* response = new AsyncJsonResponse(false, 40960U);
        JsonVariant root = response->getRoot();

        generateJsonResponse(root);

        response->setLength();
        request->send(response);

    } catch (std::bad_alloc& bad_alloc) {
        MessageOutput.printf("Call to /api/livedata/status temporarely out of resources. Reason: \"%s\".\r\n", bad_alloc.what());

        WebApi.sendTooManyRequests(request);
    }
}