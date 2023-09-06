// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_devinfo.h"
#include "WebApi.h"
#include <AsyncJson.h>

#include <ctime>

void WebApiDevInfoClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/devinfo/status", HTTP_GET, std::bind(&WebApiDevInfoClass::onDevInfoStatus, this, _1));
}

void WebApiDevInfoClass::loop()
{
}

void WebApiDevInfoClass::onDevInfoStatus(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentialsReadonly(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    //JsonObject root = response->getRoot();

    uint64_t serial = 0;
    if (request->hasParam("inv")) {
        String s = request->getParam("inv")->value();
        serial = strtoll(s.c_str(), NULL, 16);
    }

   
    response->setLength();
    request->send(response);
}