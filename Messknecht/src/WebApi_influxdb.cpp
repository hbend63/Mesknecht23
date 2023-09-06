// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_influxdb.h"
#include "Configuration.h"
#include "MessageOutput.h"
#include "WebApi.h"
#include "WebApi_errors.h"
#include "helper.h"
#include <AsyncJson.h>

void WebApiInfluxClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/influx/config", HTTP_GET, std::bind(&WebApiInfluxClass::onInfluxAdminGet, this, _1));
    _server->on("/api/influx/config", HTTP_POST, std::bind(&WebApiInfluxClass::onInfluxAdminPost, this, _1));
}

void WebApiInfluxClass::loop()
{
}

void WebApiInfluxClass::onInfluxAdminGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }
    MessageOutput.println("Get Config Influxdb");
    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject root = response->getRoot();

    const CONFIG_T& config = Configuration.get();

    //JsonObject influxdb = root.createNestedObject("influxdb");
    root["url"] = config.InfluxDB_Url;
    root["token"] = config.InfluxDB_Token;
    root["org"] = config.InfluxDB_Org;
    root["bucket"] = config.InfluxDB_Bucket; 

    String json;
    serializeJson(root, json);
    MessageOutput.println(json);
    
    response->setLength();
    request->send(response);
}

void WebApiInfluxClass::onInfluxAdminPost(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }
    MessageOutput.println("Set Config Influxdb");
    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject retMsg = response->getRoot();
    retMsg["type"] = "warning";

    if (!request->hasParam("data", true)) {
        retMsg["message"] = "No values found!";
        retMsg["code"] = WebApiError::GenericNoValueFound;
        response->setLength();
        request->send(response);
        return;
    }

    String json = request->getParam("data", true)->value();

    if (json.length() > MQTT_JSON_DOC_SIZE) {
        retMsg["message"] = "Data too large!";
        retMsg["code"] = WebApiError::GenericDataTooLarge;
        response->setLength();
        request->send(response);
        return;
    }

    MessageOutput.println("daten");
    MessageOutput.println(json);

    DynamicJsonDocument root(MQTT_JSON_DOC_SIZE);
    DeserializationError error = deserializeJson(root, json);

    if (error) {
        retMsg["message"] = "Failed to parse data!";
        retMsg["code"] = WebApiError::GenericParseError;
        response->setLength();
        request->send(response);
        return;
    }

    if (!(root.containsKey("influxdb"))) {
        retMsg["message"] = "Values are missing!";
        retMsg["code"] = WebApiError::GenericValueMissing;
        response->setLength();
        request->send(response);
        return;
    }
    
    CONFIG_T& config = Configuration.get();

    JsonObject influxdb = root["influxdb"];
    strlcpy(config.InfluxDB_Url, influxdb["url"], sizeof(config.InfluxDB_Url));
    strlcpy(config.InfluxDB_Token, influxdb["token"], sizeof(config.InfluxDB_Token));
    strlcpy(config.InfluxDB_Org, influxdb["org"], sizeof(config.InfluxDB_Org));
    strlcpy(config.InfluxDB_Bucket, influxdb["bucket"], sizeof(config.InfluxDB_Bucket));
    
    MessageOutput.println("Influx");
    MessageOutput.println(config.InfluxDB_Url);

    Configuration.write();

    retMsg["type"] = "success";
    retMsg["message"] = "Settings saved!";
    retMsg["code"] = WebApiError::GenericSuccess;

    response->setLength();
    request->send(response);
}


