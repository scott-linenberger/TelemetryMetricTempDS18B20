#include <TelemetryMetricTempDS18B20.h>
#include "TELEM_CONFIG.h"
#include <TelemetryNode.h>
#include <RunnableLed.h>

/* Runnable LED (we'll use the onboard LED for this example)*/
RunnableLed ledOnboard = RunnableLed(
  0,    // pin
  HIGH  // "on" pin state
);

/* Connections */
WiFiClient wiFiClient;
MqttClient mqttClient(wiFiClient);

/* Telemetry Node */
TelemetryNode telemNode = TelemetryNode(
  wiFiClient,
  mqttClient,
  ledOnboard,
  TELEM_CONFIG);

TelemetryMetricTempDS18B20 metricTemp = TelemetryMetricTempDS18B20(
  13,
  "telem-node-weather-unit", // the base topic for telemetry information
  true, // mqtt message retain - outgoing messages
  0); // mqtt message qos - outgoing messages

void setup() {
  /* begin the telemetry node */
  telemNode.begin();

  /* connect TelemetryNode to network & MQTT */
  telemNode.connect();

  // do MQTT pub/sub
  mqttClient.onMessage(onMqttOnMessage);
  mqttClient.subscribe(TELEM_CONFIG.topic.incoming_actions, 1);  //subscribe to actions

  /* start metrics */
  // metricTemp.setRunTypeInterval(500);  // publish every 60 seconds
  metricTemp.setRunTypeOnChange(100, 1.0);
  metricTemp.begin(telemNode.getMqttClient());
}

void loop() {
  yield();
  telemNode.run();
  yield();
  metricTemp.run();
  yield();
}

void onMqttOnMessage(int messageSize) {
  ledOnboard.flashTimes(3, 50);
  JsonDocument json = telemNode.processIncomingMessage(messageSize);
  metricTemp.onMessage(json);
}
