// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi.h"
#include "Configuration.h"
#include "defaults.h"
#include <AsyncJson.h>

WebApiClass::WebApiClass()
    : _server(HTTP_PORT)
    , _events("/events")
{
}

void WebApiClass::init()
{
    _server.addHandler(&_events);

    _webApiConfig.init(&_server);
    _webApiDevice.init(&_server);
    _webApiDevInfo.init(&_server);
    _webApiDtu.init(&_server);
    _webApiEventlog.init(&_server);
    _webApiFirmware.init(&_server);
    
    _webApiMaintenance.init(&_server);
    
    _webApiNetwork.init(&_server);
    
    _webApiNtp.init(&_server);
    _webApiADC.init(&_server);
    _webApiInflux.init(&_server);
   
   
    _webApiSecurity.init(&_server);
    _webApiSysstatus.init(&_server);
    _webApiWebapp.init(&_server);
    _webApiWsConsole.init(&_server);
    _webApiWsLive.init(&_server);
    

    _server.begin();
}

void WebApiClass::loop()
{
    _webApiConfig.loop();
    _webApiDevice.loop();
    _webApiDevInfo.loop();
    _webApiDtu.loop();
    _webApiEventlog.loop();
    _webApiFirmware.loop();
   
   
    _webApiMaintenance.loop();
    
    _webApiNetwork.loop();
    
    _webApiNtp.loop();
    _webApiADC.loop();
    _webApiInflux.loop();
    
    _webApiSecurity.loop();
    _webApiSysstatus.loop();
    _webApiWebapp.loop();
    _webApiWsConsole.loop();
    _webApiWsLive.loop();
    
}

bool WebApiClass::checkCredentials(AsyncWebServerRequest* request)
{
    CONFIG_T& config = Configuration.get();
    if (request->authenticate(AUTH_USERNAME, config.Security_Password)) {
        return true;
    }

    AsyncWebServerResponse* r = request->beginResponse(401);

    // WebAPI should set the X-Requested-With to prevent browser internal auth dialogs
    if (!request->hasHeader("X-Requested-With")) {
        r->addHeader("WWW-Authenticate", "Basic realm=\"Login Required\"");
    }
    request->send(r);

    return false;
}

bool WebApiClass::checkCredentialsReadonly(AsyncWebServerRequest* request)
{
    CONFIG_T& config = Configuration.get();
    if (config.Security_AllowReadonly) {
        return true;
    } else {
        return checkCredentials(request);
    }
}

void WebApiClass::sendTooManyRequests(AsyncWebServerRequest* request)
{
    auto response = request->beginResponse(429, "text/plain", "Too Many Requests");
    response->addHeader("Retry-After", "60");
    request->send(response);
}

WebApiClass WebApi;