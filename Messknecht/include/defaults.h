// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define SERIAL_BAUDRATE 115200

#define APP_HOSTNAME "MK-%06X"

#define HTTP_PORT 80

#define ACCESS_POINT_NAME "MK-"
#define ACCESS_POINT_PASSWORD "knecht42"
#define AUTH_USERNAME "admin"
#define SECURITY_ALLOW_READONLY true

#define ADMIN_TIMEOUT 180
#define WIFI_RECONNECT_TIMEOUT 30
#define WIFI_RECONNECT_REDO_TIMEOUT 600

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define WIFI_DHCP true

#define NTP_SERVER "pool.ntp.org"
#define NTP_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"
#define NTP_TIMEZONEDESCR "Europe/Berlin"
#define NTP_LONGITUDE 10.4515f
#define NTP_LATITUDE 51.1657f
#define NTP_SUNSETTYPE 1



#define DTU_SERIAL 0x99978563412
#define DTU_POLL_INTERVAL 5

#define ADC_NAME "MCP3208"
#define ADC_STD_CHNAME "Kanal"
#define ADC_STD_CHUNIT "V"
#define ADC_POLL_INTERVALL 1
#define ADC_CHANNEL_COUNT 8
