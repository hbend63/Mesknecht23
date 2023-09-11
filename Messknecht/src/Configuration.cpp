// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 Thomas Basler and others
 */
#include "Configuration.h"
#include "MessageOutput.h"
#include "defaults.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

CONFIG_T config;

void ConfigurationClass::init()
{
    memset(&config, 0x0, sizeof(config));
}

bool ConfigurationClass::write()
{
    File f = LittleFS.open(CONFIG_FILENAME, "w");
    if (!f) {
        return false;
    }
    config.Cfg_SaveCount++;

    DynamicJsonDocument doc(JSON_BUFFER_SIZE);

    JsonObject cfg = doc.createNestedObject("cfg");
    cfg["version"] = config.Cfg_Version;
    cfg["save_count"] = config.Cfg_SaveCount;

    JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = config.WiFi_Ssid;
    wifi["password"] = config.WiFi_Password;
    wifi["ip"] = IPAddress(config.WiFi_Ip).toString();
    wifi["netmask"] = IPAddress(config.WiFi_Netmask).toString();
    wifi["gateway"] = IPAddress(config.WiFi_Gateway).toString();
    wifi["dns1"] = IPAddress(config.WiFi_Dns1).toString();
    wifi["dns2"] = IPAddress(config.WiFi_Dns2).toString();
    wifi["dhcp"] = config.WiFi_Dhcp;
    wifi["hostname"] = config.WiFi_Hostname;

    JsonObject influxdb = doc.createNestedObject("influxdb");
    influxdb["url"] = config.InfluxDB_Url;
    influxdb["token"] = config.InfluxDB_Token;
    influxdb["org"] = config.InfluxDB_Org;
    influxdb["bucket"] = config.InfluxDB_Bucket; 
    influxdb["name"] = config.InfluxDB_SensorName; 

    JsonObject ntp = doc.createNestedObject("ntp");
    ntp["server"] = config.Ntp_Server;
    ntp["timezone"] = config.Ntp_Timezone;
    ntp["timezone_descr"] = config.Ntp_TimezoneDescr;
    ntp["latitude"] = config.Ntp_Latitude;
    ntp["longitude"] = config.Ntp_Longitude;   

    JsonObject dtu = doc.createNestedObject("dtu");
    dtu["serial"] = config.Dtu_Serial;
    dtu["poll_interval"] = config.Dtu_PollInterval;
    
    JsonObject security = doc.createNestedObject("security");
    security["password"] = config.Security_Password;
    security["allow_readonly"] = config.Security_AllowReadonly;

    JsonObject adc = doc.createNestedObject("adc");
    adc["typename"] = config.AD_Converter.AD_TypeName;
    adc["pollintervall"] = config.AD_Converter.AD_PollIntervall;
    JsonArray channels = adc.createNestedArray("channels");
    for (uint8_t i = 0; i < CHAN_MAX_COUNT; i++) {
        JsonObject chanData = channels.createNestedObject();
        chanData["name"] = config.AD_Converter.ADChannels[i].Name;
        chanData["unit"] = config.AD_Converter.ADChannels[i].Unit;
        chanData["id"] =  config.AD_Converter.ADChannels[i].channelId;
        chanData["dmin"] = config.AD_Converter.ADChannels[i].dglMin;
        chanData["dmax"] = config.AD_Converter.ADChannels[i].dglMax; 
        chanData["smin"] = config.AD_Converter.ADChannels[i].sclMin;
        chanData["smax"] = config.AD_Converter.ADChannels[i].sclMax;
        chanData["takt"] = config.AD_Converter.ADChannels[i].takt_s;
        
    }       

    // Serialize JSON to file
    if (serializeJson(doc, f) == 0) {
        MessageOutput.println("Failed to write file");
        return false;
    }

    f.close();
    return true;
}

bool ConfigurationClass::read()
{
    File f = LittleFS.open(CONFIG_FILENAME, "r", false);

    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, f);
    if (error) {
        MessageOutput.println("Failed to read file, using default configuration");
    }

    JsonObject cfg = doc["cfg"];
    config.Cfg_Version = cfg["version"] | CONFIG_VERSION;
    config.Cfg_SaveCount = cfg["save_count"] | 0;

    JsonObject wifi = doc["wifi"];
    strlcpy(config.WiFi_Ssid, wifi["ssid"] | WIFI_SSID, sizeof(config.WiFi_Ssid));
    strlcpy(config.WiFi_Password, wifi["password"] | WIFI_PASSWORD, sizeof(config.WiFi_Password));
    strlcpy(config.WiFi_Hostname, wifi["hostname"] | APP_HOSTNAME, sizeof(config.WiFi_Hostname));

    IPAddress wifi_ip;
    wifi_ip.fromString(wifi["ip"] | "");
    config.WiFi_Ip[0] = wifi_ip[0];
    config.WiFi_Ip[1] = wifi_ip[1];
    config.WiFi_Ip[2] = wifi_ip[2];
    config.WiFi_Ip[3] = wifi_ip[3];

    IPAddress wifi_netmask;
    wifi_netmask.fromString(wifi["netmask"] | "");
    config.WiFi_Netmask[0] = wifi_netmask[0];
    config.WiFi_Netmask[1] = wifi_netmask[1];
    config.WiFi_Netmask[2] = wifi_netmask[2];
    config.WiFi_Netmask[3] = wifi_netmask[3];

    IPAddress wifi_gateway;
    wifi_gateway.fromString(wifi["gateway"] | "");
    config.WiFi_Gateway[0] = wifi_gateway[0];
    config.WiFi_Gateway[1] = wifi_gateway[1];
    config.WiFi_Gateway[2] = wifi_gateway[2];
    config.WiFi_Gateway[3] = wifi_gateway[3];

    IPAddress wifi_dns1;
    wifi_dns1.fromString(wifi["dns1"] | "");
    config.WiFi_Dns1[0] = wifi_dns1[0];
    config.WiFi_Dns1[1] = wifi_dns1[1];
    config.WiFi_Dns1[2] = wifi_dns1[2];
    config.WiFi_Dns1[3] = wifi_dns1[3];

    IPAddress wifi_dns2;
    wifi_dns2.fromString(wifi["dns2"] | "");
    config.WiFi_Dns2[0] = wifi_dns2[0];
    config.WiFi_Dns2[1] = wifi_dns2[1];
    config.WiFi_Dns2[2] = wifi_dns2[2];
    config.WiFi_Dns2[3] = wifi_dns2[3];

    config.WiFi_Dhcp = wifi["dhcp"] | WIFI_DHCP;

    JsonObject influxdb = doc["influxdb"];
    strlcpy(config.InfluxDB_Url, influxdb["url"] | INFLUXDB_URL, sizeof(config.InfluxDB_Url));
    strlcpy(config.InfluxDB_Token, influxdb["token"] | INFLUXDB_TOKEN, sizeof(config.InfluxDB_Token));
    strlcpy(config.InfluxDB_Org, influxdb["org"] | INFLUXDB_ORG, sizeof(config.InfluxDB_Org));
    strlcpy(config.InfluxDB_Bucket, influxdb["bucket"] | INFLUXDB_BUCKET, sizeof(config.InfluxDB_Bucket));
    strlcpy(config.InfluxDB_SensorName, influxdb["name"] | INFLUXDB_BUCKET, sizeof(config.InfluxDB_SensorName));

    JsonObject ntp = doc["ntp"];
    strlcpy(config.Ntp_Server, ntp["server"] | NTP_SERVER, sizeof(config.Ntp_Server));
    strlcpy(config.Ntp_Timezone, ntp["timezone"] | NTP_TIMEZONE, sizeof(config.Ntp_Timezone));
    strlcpy(config.Ntp_TimezoneDescr, ntp["timezone_descr"] | NTP_TIMEZONEDESCR, sizeof(config.Ntp_TimezoneDescr));
    config.Ntp_Latitude = ntp["latitude"] | NTP_LATITUDE;
    config.Ntp_Longitude = ntp["longitude"] | NTP_LONGITUDE;
    
    JsonObject dtu = doc["dtu"];
    config.Dtu_Serial = dtu["serial"] | DTU_SERIAL;
    config.Dtu_PollInterval = dtu["poll_interval"] | DTU_POLL_INTERVAL;
    
    JsonObject security = doc["security"];
    strlcpy(config.Security_Password, security["password"] | ACCESS_POINT_PASSWORD, sizeof(config.Security_Password));
    config.Security_AllowReadonly = security["allow_readonly"] | SECURITY_ALLOW_READONLY;

    JsonObject adc = doc["adc"];
    strlcpy(config.AD_Converter.AD_TypeName, adc["typename"] | ADC_NAME, sizeof(config.AD_Converter.AD_TypeName));
    config.AD_Converter.AD_PollIntervall=adc["pollintervall"] | ADC_POLL_INTERVALL;
    JsonArray channels = adc["channels"];
    for (uint8_t i = 0; i < CHAN_MAX_COUNT; i++) {
        JsonObject chanData = channels[i].as<JsonObject>();
        
        strlcpy(config.AD_Converter.ADChannels[i].Name, chanData["name"] | ADC_STD_CHNAME, sizeof(config.AD_Converter.ADChannels[i].Name));
        strlcpy(config.AD_Converter.ADChannels[i].Unit, chanData["unit"] | ADC_STD_CHUNIT, sizeof(config.AD_Converter.ADChannels[i].Unit));        
        config.AD_Converter.ADChannels[i].channelId=chanData["id"] | (i+1);
        config.AD_Converter.ADChannels[i].dglMin=chanData["dmin"] | 0.0;
        config.AD_Converter.ADChannels[i].dglMax=chanData["dmax"] | 4095;
        config.AD_Converter.ADChannels[i].sclMin=chanData["smin"] | 0.0;
        config.AD_Converter.ADChannels[i].sclMax=chanData["smax"] | 5.0;
        config.AD_Converter.ADChannels[i].takt_s=chanData["takt"] | 0.0;               
    }    
    f.close();
    return true;
}

void ConfigurationClass::migrate()
{
    File f = LittleFS.open(CONFIG_FILENAME, "r", false);
    if (!f) {
        MessageOutput.println("Failed to open file, cancel migration");
        return;
    }

    DynamicJsonDocument doc(JSON_BUFFER_SIZE);
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, f);
    if (error) {
        MessageOutput.printf("Failed to read file, cancel migration: %s\r\n", error.c_str());
        return;
    }

    

    f.close();

    config.Cfg_Version = CONFIG_VERSION;
    write();
    read();
}

CONFIG_T& ConfigurationClass::get()
{
    return config;
}


ConfigurationClass Configuration;