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

#include <ESP8266HTTPClient.h>
#include "tx23.h"
#include "settings.h"
#include "schedule.h"
#include "mqtt.h"

extern TX23 tx_23;
extern Task task_info;
extern Settings eeprom_settings;

void mqttpubsub::callback(char* topic, byte* payload, unsigned int length) {
  if(length > 0) {
    const String sTopic= topic;
    String sPayload; for (uint i=0; i<length; i++) { sPayload+= String((char)payload[i]); }
    #ifdef SERIAL_DEBUG_MQTT
      Serial_Log.println("Message arrived ["+sTopic+"] "+sPayload);
    #endif

    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_LED)) {
      sPayload.toUpperCase();
      if(sPayload.equals("ON") ) digitalWrite(LED_INDICATION_PIN, LOW);
      if(sPayload.equals("OFF") ) digitalWrite(LED_INDICATION_PIN, HIGH);
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_PUBLISH_DELAY)) {
      uiMqttPublishDelay = sPayload.toInt();
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_URL)) {
      sprintf(eeprom_settings.tx_23_settings.cMiddleware, sPayload.c_str());
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_OP)) {
      sprintf(eeprom_settings.tx_23_settings.cMiddlewareOperation, sPayload.c_str());
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_ENABLE)) {
      sPayload.toUpperCase();
      eeprom_settings.tx_23_settings.bMiddlewareEnabled= sPayload.equals("TRUE") ? true:false;
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_DELAY_TIME)) {
      eeprom_settings.tx_23_settings.uiMiddlewareDelayTime= sPayload.toInt();
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_DIRECTION_OFFSET)) {
      eeprom_settings.tx_23_settings.uiOffset_Direction= sPayload.toInt();
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }
    if(sTopic.equals(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_WINDSPEED_OFFSET)) {
      eeprom_settings.tx_23_settings.iOffset_WindSpeed= sPayload.toInt();
      eeprom_settings.tx_23_settings.bEepromWrite= true;
    }

    // Check and write changed values to eeprom
    if(eeprom_settings.tx_23_settings.bEepromWrite)  {
      eeprom_settings.write(eeprom_settings.tx_23_settings);
    }
  }
}

void mqttpubsub::setup() {
  client->setServer(MQTT_SERVER, MQTT_PORT);
  client->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
}

void mqttpubsub::reconnect() {
  while (!client->connected()) {
    #ifdef SERIAL_DEBUG_MQTT
      Serial_Log.println("Attempting MQTT connecting...");
    #endif
    String clientId = MQTT_IOT_NAME; clientId += String(random(0xffff), HEX);
    if (client->connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD )) {
    #ifdef SERIAL_DEBUG_MQTT
      Serial_Log.print("Done!"); Serial.println("");
    #endif

      // Initial publish current settings and informations
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_LED,              "ON" , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_DIRECTION_OFFSET, String(eeprom_settings.tx_23_settings.uiOffset_Direction).c_str() , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_WINDSPEED_OFFSET, String(eeprom_settings.tx_23_settings.iOffset_WindSpeed ).c_str() , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_PUBLISH_DELAY,    String(uiMqttPublishDelay).c_str(), counter );

      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_URL,        String(eeprom_settings.tx_23_settings.cMiddleware).c_str() , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_OP,         String(eeprom_settings.tx_23_settings.cMiddlewareOperation).c_str(), counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_ENABLE,     eeprom_settings.tx_23_settings.bMiddlewareEnabled ? "true":"false", counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_DELAY_TIME, String(eeprom_settings.tx_23_settings.uiMiddlewareDelayTime).c_str() , counter );

      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE_WIFI_MAC,  eeprom_settings.wifi_info.sMacAddress.c_str() , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE_WIFI_SSID, eeprom_settings.wifi_info.sSSID.c_str() , counter );
      PublishMessage( MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE_WIFI_IP,   eeprom_settings.wifi_info.sLocalIP.c_str() , counter );

      // Setup initial subscribe list
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_LED              ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_DIRECTION_OFFSET ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_WINDSPEED_OFFSET ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_SETTING MQTT_VALUE_SET_PUBLISH_DELAY    ).c_str());

      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_URL        ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_OP         ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_ENABLE     ).c_str());
      client->subscribe(String(MQTT_TOPIC_ROOT_DEVICE_MIDDLEWARE MQTT_VALUE_SET_MIDDLEWARE_DELAY_TIME ).c_str());

    } else {
      #ifdef SERIAL_DEBUG_MQTT
        Serial_Log.print("Failed connection to MQTT, rc="); Serial.print(client->state());
        Serial_Log.println(" try again in 5 seconds");
      #endif
      delay(MQTT_RECONNECT_DELAY);
    }
  }
}

void mqttpubsub::PublishMessage(String strPublishTopic, String strMessage, uint iCounter) {
  #ifdef SERIAL_DEBUG_MQTT
    Serial_Log.println("Publish Topic ("+strPublishTopic+") - Message: "+strMessage+" #"+iCounter);
  #endif
  client->publish(strPublishTopic.c_str(), strMessage.c_str() );
}

void mqttpubsub::loop() {
  if (!client->connected()) {
    reconnect();
  }
  client->loop();
  eeprom_settings.Uptime();

  unsigned long now = millis();
  if (now - lastMsg > uiMqttPublishDelay) {
    lastMsg = now; if (counter == ULONG_MAX ) counter=0; ++counter;

    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_DIRECTION,    tx_23.bLastReadState ? tx_23.sRawDirection.c_str() : tx_23.sLastError.c_str(), counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_DIRECTION_RAW,tx_23.bLastReadState ? String(tx_23.iReadDirection).c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_DIRECTION_LO, tx_23.bLastReadState ? tx_23.sRawDirectionLong.c_str() : tx_23.sLastError.c_str(), counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_DIRECTION_DEG,tx_23.bLastReadState ? String(tx_23.fRawDirection).c_str() : tx_23.sLastError.c_str(), counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED,        tx_23.bLastReadState ? tx_23.sRawSpeedMs.c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED_RAW,    tx_23.bLastReadState ? String(tx_23.uiSpeedRaw).c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED_KMH,    tx_23.bLastReadState ? tx_23.sRawSpeedKMh.c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED_MI,     tx_23.bLastReadState ? tx_23.sRawSpeedMi.c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED_FT,     tx_23.bLastReadState ? tx_23.sRawSpeedFt.c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_SPEED_KN,     tx_23.bLastReadState ? tx_23.sRawSpeedKn.c_str() : "-1", counter );

    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_BFT,            tx_23.bLastReadState ? String(tx_23.uiBeatFortNumber).c_str() : "-1", counter );
    PublishMessage( MQTT_TOPIC_ROOT_DEVICE MQTT_VALUE_BFT_DESCRIPTION,tx_23.bLastReadState ? tx_23.sBeatForNumberDescription.c_str() : "-1", counter );

    PublishMessage( MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE_WIFI_RSSI, String(eeprom_settings.wifi_info.iRSSI).c_str(), counter );

    PublishMessage( MQTT_TOPIC_ROOT_DEVICE_WIFI MQTT_TOPIC_ROOT_DEVICE_UPTIME, eeprom_settings.sys_uptime.sUptime.c_str(), counter );

  }
}
