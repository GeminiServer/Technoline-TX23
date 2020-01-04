// Technoline TX23 - ESP
//
// Copyright (C) 2019  Erkan Çolak
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//#define SERIAL_DEBUG
//#define SERIAL_DEBUG_MQTT

#ifdef SERIAL_DEBUG
  #define Serial_Log Serial
#endif

// Version and Name
//
#define HOST_NAME             "Technoline-TX23"
#define TX23_VERSION          "0.3.2002130038"
#define TX23_EEPROM_VERSION   "0.3.0212"

// Set LED pin to indicate the TX23 state.
//
#define LED_INDICATION_PIN 2

// Set IO-Port for TX23 Data IO
//
static const uint16_t tx_io_port = 0;

// Define your WiFi SSID and PSK
//
#define STASSID            "YOUR-WiFi-SSID"
#define STAPSK             "WiFiPassword"

// Firmware settings
//
#define UPDATE_HTTP_PATH   "/firmware"
#define UPDATE_USER        "admin"
#define UPDATE_PASSWD      "admin"

// WebServer settings
//
#define WEBSERVER_PORT     80
#define WEBSOCKET_PORT     81

// MQTT Server & Client settings
//
#define MQTT_IOT_NAME               "TX23-Client-"
#define MQTT_SERVER                 "11.11.0.7"
#define MQTT_PORT                   1883
#define MQTT_USER                   "qonos"
#define MQTT_PASSWORD               "qonospi"
#define MQTT_MSG_BUFFER_SIZE        50
#define MQTT_RECONNECT_DELAY        5000
#define MQTT_MSG_PUB_SUB_DELAY      5000
#define MIDDLEWARE_DELAY_TIME       15000
//
#define MQTT_TOPIC_DEVICE_IDENT     ""//"000000001/"
#define MQTT_TOPIC_ROOT             "home/tx23/"
#define MQTT_TOPIC_ROOT_DEVICE      MQTT_TOPIC_ROOT MQTT_TOPIC_DEVICE_IDENT
#define MQTT_VALUE_DIRECTION        "direction"
#define MQTT_VALUE_DIRECTION_LO     "direction-lo"
#define MQTT_VALUE_DIRECTION_DEG    "direction-deg"
#define MQTT_VALUE_DIRECTION_RAW    "direction-raw"
//
#define MQTT_VALUE_SPEED            "speed"
#define MQTT_VALUE_SPEED_RAW        "speed-raw"
#define MQTT_VALUE_SPEED_KMH        "speed-kmh"
#define MQTT_VALUE_SPEED_MI         "speed-mi"
#define MQTT_VALUE_SPEED_FT         "speed-ft"
#define MQTT_VALUE_SPEED_KN         "speed-kn"
//
#define MQTT_VALUE_BFT              "bft-number"
#define MQTT_VALUE_BFT_DESCRIPTION  "bft-description"
//
#define MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_TOPIC_ROOT_DEVICE "settings/"
#define MQTT_VALUE_SET_PUBLISH_DELAY          "publish_message_time"
#define MQTT_VALUE_SET_LED                    "led"
#define MQTT_VALUE_SET_DIRECTION_OFFSET       "direction_offset"
#define MQTT_VALUE_SET_WINDSPEED_OFFSET       "windspeed_offset"
//
#define MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_TOPIC_ROOT_DEVICE_SETTING "middleware/"
#define MQTT_VALUE_SET_MIDDLEWARE_URL         "url"
#define MQTT_VALUE_SET_MIDDLEWARE_OP          "url_operation"
#define MQTT_VALUE_SET_MIDDLEWARE_ENABLE      "enable"
#define MQTT_VALUE_SET_MIDDLEWARE_DELAY_TIME  "delay_time"
//
#define MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE "wifi-info/"
#define MQTT_TOPIC_ROOT_DEVICE_WIFI_RSSI      "rssi"
#define MQTT_TOPIC_ROOT_DEVICE_WIFI_MAC       "mac-address"
#define MQTT_TOPIC_ROOT_DEVICE_WIFI_SSID      "ssid"
#define MQTT_TOPIC_ROOT_DEVICE_WIFI_IP        "local-ip"
#define MQTT_TOPIC_ROOT_DEVICE_UPTIME         "uptime"

// Define middleware url
//
#define MIDDLEWARE_URL          "http://11.11.0.15/middleware.php/data/b01861e0-353c-11ea-9e05-47ade029c05a.json"
#define MIDDLEWARE_OPERATION    "?operation=add&value="

// Internal
//
#define EEPROM_START_ADRESS     0
#define OFFSET_DIRECTION        1  // Default is 0! This is my offset!
#define OFFSET_SPEED            0

class Settings{
public:
  // !! On chages here - Increase the TX23_EEPROM_VERSION!!
  struct TX_23Settings{
    bool bEepromWrite             = false;
    char cEepromVersion[9]        = TX23_EEPROM_VERSION;
    char cMiddleware[96]          = "-";
    char cMiddlewareOperation[32] = "-";
    bool bMiddlewareEnabled       = true;
    uint uiMiddlewareDelayTime    = MIDDLEWARE_DELAY_TIME;
    uint8 uiOffset_Direction      = OFFSET_DIRECTION;
    int16_t iOffset_WindSpeed     = OFFSET_SPEED;
  } tx_23_settings;

  struct WiFi_Info{
    int32_t iRSSI=-1;
    String sMacAddress;
    String sSSID;
    String sLocalIP;
  } wifi_info;

  struct UpTime{
    uint16 lDay       = 0;
    uint8 iHour       = 0;
    uint8 iMinute     = 0;
    uint8 iSecond     = 0;
    bool bHighMillis  = false;
    uint32 iRollover  = 0;
    String sUptime;
  } sys_uptime;

  void Uptime(); 
  void Setup();
  bool read(TX_23Settings &tx_23_Settings);
  bool write(TX_23Settings &tx_23_Settings);
  void ClearEEPROM();
};
