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

#include <Scheduler.h>
#include <PubSubClient.h>

class mqttpubsub : public Task {
  protected:
    void setup();
    void loop();
  private:
    unsigned int counter = 0;
    unsigned long lastMsg = 0;
    String sMessage;
    String sPublishTopic;
    uint uiMqttPublishDelay= MQTT_MSG_PUB_SUB_DELAY;

    void reconnect();
    void PublishMessage(String strPublishTopic, String strMessage, uint iCounter);
    WiFiClient lWClient;
    PubSubClient *client= new PubSubClient(lWClient);
  protected:
    void callback(char* topic, byte* payload, unsigned int length);
};
