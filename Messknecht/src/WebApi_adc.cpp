// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "WebApi_adc.h"
#include "Configuration.h"
#include "adc.h"
#include "MessageOutput.h"
#include "WebApi.h"
#include "WebApi_errors.h"
#include "helper.h"
#include <AsyncJson.h>

void WebApiADCClass::init(AsyncWebServer* server)
{
    using std::placeholders::_1;

    _server = server;

    _server->on("/api/adc/config", HTTP_GET, std::bind(&WebApiADCClass::onADCAdminGet, this, _1));
    _server->on("/api/adc/config", HTTP_POST, std::bind(&WebApiADCClass::onADCAdminPost, this, _1));

    _server->on("/api/adc/read/raw", HTTP_GET, std::bind(&WebApiADCClass::onADCRawValuesGet, this, _1));
    _server->on("/api/adc/read/scale", HTTP_GET, std::bind(&WebApiADCClass::onADCScaleValuesGet, this, _1));

    
}

void WebApiADCClass::loop()
{
}

void WebApiADCClass::onADCAdminGet(AsyncWebServerRequest* request)
{
    MessageOutput.println("Get Config ADC");
    if (!WebApi.checkCredentials(request)) {
        return;
    }
    
    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject root = response->getRoot();

    const CONFIG_T& config = Configuration.get();
    JsonObject adc = root.createNestedObject("adc");
    adc["typename"] = config.AD_Converter.AD_TypeName;
    adc["pollintervall"] = config.AD_Converter.AD_PollIntervall;
    JsonArray channels = adc.createNestedArray("channels");
    for (uint8_t i = 1; i < CHAN_MAX_COUNT+1; i++) {
        JsonObject chanData = channels.createNestedObject();
        chanData["name"] = config.AD_Converter.ADChannels[i-1].Name;
        chanData["unit"] = config.AD_Converter.ADChannels[i-1].Unit;
        chanData["id"] = i;
        chanData["dmin"] = config.AD_Converter.ADChannels[i-1].dglMin;
        chanData["dmax"] = config.AD_Converter.ADChannels[i-1].dglMax;
        chanData["smin"] = 1.0*config.AD_Converter.ADChannels[i-1].sclMin;
        chanData["smax"] = 1.0*config.AD_Converter.ADChannels[i-1].sclMax;
        chanData["takt"] = config.AD_Converter.ADChannels[i-1].takt_s;
    }
    response->setLength();
    request->send(response);
}

void WebApiADCClass::onADCAdminPost(AsyncWebServerRequest* request)
{
    MessageOutput.println("Set Config ADC");
    if (!WebApi.checkCredentials(request)) {
        MessageOutput.println("Not logged in");
        return;
    }
    
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

    if (!(root.containsKey("adc"))) {
        retMsg["message"] = "Values are missing!";
        retMsg["code"] = WebApiError::GenericValueMissing;
        response->setLength();
        request->send(response);
        return;
    }
    
    CONFIG_T& config = Configuration.get();
    JsonObject adc = root["adc"];
    strlcpy(config.AD_Converter.AD_TypeName, adc["typename"], sizeof(config.AD_Converter.AD_TypeName));
    config.AD_Converter.AD_PollIntervall = adc["pollintervall"];
    JsonArray channels = adc["channels"];   
    //MessageOutput.println(channels.size());
    for (uint8_t i = 0; i < CHAN_MAX_COUNT; i++) {
        //MessageOutput.print("ch: ");
        //MessageOutput.println(i);
        JsonObject chanData = channels[i].as<JsonObject>();  
        //u_int16_t t= chanData["takt"];   
        //MessageOutput.println(t);  
        strlcpy(config.AD_Converter.ADChannels[i].Name, chanData["name"], sizeof(config.AD_Converter.ADChannels[i-1].Name));
        strlcpy(config.AD_Converter.ADChannels[i].Unit, chanData["unit"], sizeof(config.AD_Converter.ADChannels[i-1].Unit));
        config.AD_Converter.ADChannels[i].channelId = i+1;
        config.AD_Converter.ADChannels[i].dglMin = chanData["dmin"];
        config.AD_Converter.ADChannels[i].dglMax = chanData["dmax"];        
        config.AD_Converter.ADChannels[i].sclMin = chanData["smin"];        
        config.AD_Converter.ADChannels[i].sclMax = chanData["smax"];
        config.AD_Converter.ADChannels[i].takt_s = chanData["takt"];
    }
    
    Configuration.write();

    retMsg["type"] = "success";
    retMsg["message"] = "Settings saved!";
    retMsg["code"] = WebApiError::GenericSuccess;

    response->setLength();
    request->send(response);
}

void WebApiADCClass::onADCRawValuesGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }
    MessageOutput.println("Read ADC Raw Data");
    const CONFIG_T& config = Configuration.get();
    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject root = response->getRoot();
    JsonArray channels = root.createNestedArray("datavalues");
    for (int channel = 0; channel < CHAN_MAX_COUNT; channel++) {
        uint16_t val = ADConverter.rawData(channel+1);
        MessageOutput.print("Raw-Data Ch ");
        MessageOutput.print(channel);
        MessageOutput.print(": ");
        MessageOutput.println(val);
        JsonObject chanData = channels.createNestedObject();
        chanData["id"] = channel+1;
        chanData["value"] = val;
        chanData["takt"] = config.AD_Converter.ADChannels[channel].takt_s;
    }    
    String json;
    serializeJson(root, json);
    MessageOutput.println(json);
    response->setLength();
    request->send(response);
}

void WebApiADCClass::onADCScaleValuesGet(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }
    MessageOutput.println("Read ADC Scaled Data");
    const CONFIG_T& config = Configuration.get();
    AsyncJsonResponse* response = new AsyncJsonResponse(false, MQTT_JSON_DOC_SIZE);
    JsonObject root = response->getRoot();
    JsonArray channels = root.createNestedArray("datavalues");    
    for (int channel = 0; channel < CHAN_MAX_COUNT; channel++) {
        MessageOutput.print("Scale-Data Ch ");
        MessageOutput.print(channel);
        MessageOutput.print(": ");
        MessageOutput.println(ADConverter.scaleDat(channel+1));
        JsonObject chanData = channels.createNestedObject();   
        chanData["id"] = channel+1;   
        chanData["value"] = ADConverter.scaleDat(channel+1);
        chanData["takt"] = config.AD_Converter.ADChannels[channel].takt_s;
    }    
    String json;
    serializeJson(root, json);
    MessageOutput.println(json);
    response->setLength();
    request->send(response);
}
