// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <Arduino.h>

#define CONFIG_FILENAME "/config.json"
#define CONFIG_VERSION 0x00011900 // 0.1.24 // make sure to clean all after change

#define WIFI_MAX_SSID_STRLEN 32
#define WIFI_MAX_PASSWORD_STRLEN 64
#define WIFI_MAX_HOSTNAME_STRLEN 31


#define INFLUXDB_MAX_URL_STRLEN 64
#define INFLUXDB_MAX_TOKEN_STRLEN 128
#define INFLUXDB_MAX_ORG_STRLEN 32
#define INFLUXDB_MAX_BUCKET_STRLEN 32

 
#define INFLUXDB_URL "http://192.168.188.20:8086"
// InfluxDB 2 server or cloud API authentication token (Use: InfluxDB UI -> Load Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "oKRVuepAzM7LiobYCJTm1XTUf0j59hTgDZbDyQky8bPoT1QXsUrdPG8m-_I5oIR0olyFagAbsSHu4LcJT9qWfg=="
// InfluxDB 2 organization id (Use: InfluxDB UI -> Settings -> Profile -> <name under tile> )
#define INFLUXDB_ORG "hsel"
// InfluxDB 2 bucket name (Use: InfluxDB UI -> Load Data -> Buckets)
#define INFLUXDB_BUCKET "messknecht"

#define NTP_MAX_SERVER_STRLEN 31
#define NTP_MAX_TIMEZONE_STRLEN 50
#define NTP_MAX_TIMEZONEDESCR_STRLEN 50

#define AD_MAX_TYPE_STRLEN 11
#define CHAN_MAX_NAME_STRLEN 31
#define CHAN_MAX_COUNT 6

#define JSON_BUFFER_SIZE 12288

struct CHANNEL_CONFIG_T {
    uint8_t channelId;
    uint32_t dglMin;
    uint32_t dglMax;
    float sclMin;
    float sclMax;
    uint16_t takt_s;
    char Name[CHAN_MAX_NAME_STRLEN];
    char Unit[CHAN_MAX_NAME_STRLEN];
};

struct AD_CONFIG_T {
   char AD_TypeName[AD_MAX_TYPE_STRLEN + 1];
   CHANNEL_CONFIG_T ADChannels[CHAN_MAX_COUNT];
   uint32_t AD_PollIntervall; 
};


struct CONFIG_T {
    uint32_t Cfg_Version;
    uint Cfg_SaveCount;

    char WiFi_Ssid[WIFI_MAX_SSID_STRLEN + 1];
    char WiFi_Password[WIFI_MAX_PASSWORD_STRLEN + 1];
    byte WiFi_Ip[4];
    byte WiFi_Netmask[4];
    byte WiFi_Gateway[4];
    byte WiFi_Dns1[4];
    byte WiFi_Dns2[4];
    bool WiFi_Dhcp;
    char WiFi_Hostname[WIFI_MAX_HOSTNAME_STRLEN + 1];


    char InfluxDB_Url[INFLUXDB_MAX_URL_STRLEN+1];
    char InfluxDB_Token[INFLUXDB_MAX_TOKEN_STRLEN+1];
    char InfluxDB_Org[INFLUXDB_MAX_ORG_STRLEN+1];
    char InfluxDB_Bucket[INFLUXDB_MAX_BUCKET_STRLEN+1];

    char Ntp_Server[NTP_MAX_SERVER_STRLEN + 1];
    char Ntp_Timezone[NTP_MAX_TIMEZONE_STRLEN + 1];
    char Ntp_TimezoneDescr[NTP_MAX_TIMEZONEDESCR_STRLEN + 1];
    double Ntp_Longitude;
    double Ntp_Latitude;      

    AD_CONFIG_T AD_Converter;

    uint64_t Dtu_Serial;
    uint32_t Dtu_PollInterval;
    
    char Security_Password[WIFI_MAX_PASSWORD_STRLEN + 1];
    bool Security_AllowReadonly;
    
};

class ConfigurationClass {
public:
    void init();
    bool read();
    bool write();
    void migrate();
    CONFIG_T& get();    
};

extern ConfigurationClass Configuration;