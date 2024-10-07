#include "TelemetryMetricTempDS18B20.h"

void TelemetryMetricTempDS18B20::begin(MqttClient* mqttClient) {
  TelemetryMetricReporter::begin(mqttClient);

  /* initialize sensors */
  sensors->begin();
  countSensors = sensors->getDeviceCount();
  Serial.println();
  Serial.print(countSensors);
  Serial.println(" - sensors found");

  _recordTemperatures(); // record temps on start
}

String TelemetryMetricTempDS18B20::_getTopicEvents() {
  return _topicMqttBase + "/temp/events";
}

void TelemetryMetricTempDS18B20::publishMetric() {
  _recordTemperatures(); // record live sensor data
  /* cycle through connected sensors and publish */
  for (uint8_t i = 0; i < countSensors; i++) {
    _publishIndex(i);
  }
}

void TelemetryMetricTempDS18B20::publishConfiguration() {
  JsonDocument json = _getBaseConfig();

  /* add custom json */
  _publishJson(
    json,
    _topicMqttBase + "/temp/config",
    _retainEvents,
    _qosEvents
    );
}

float TelemetryMetricTempDS18B20::getMetricValue() {
  /* record live temperature data */
  _recordTemperatures();

  float tempTotalChange = 0; // placeholder for the change 

  /* iterate sensors */
  for (uint8_t i = 0; i < countSensors; i++) {
    /* check the current recorded temperature */
    double tempCurr = tempCurrentF[i];
    double tempPrevious = tempPreviousF[i];

    /* total up the difference */
    tempTotalChange += abs(tempCurr - tempPrevious);
  }

  return tempTotalChange;
}

void TelemetryMetricTempDS18B20::_recordTemperatures() {
  /* read the DS18B20 sensors */
  sensors->requestTemperatures();

  for (uint8_t index = 0; index < countSensors; index++) {
    /* record the current temperature in Farenheit and Celsius */
    tempCurrentF[index] = sensors->getTempFByIndex(index);
    tempCurrentC[index] = sensors->getTempCByIndex(index);
  }
}

void TelemetryMetricTempDS18B20::_publishIndex(uint8_t index) {
  String stringTopicF = _topicMqttBase + "/temp/" + index + "/F";
  int topicLengthF = stringTopicF.length() + 1;
  char sensorTopicF[topicLengthF];

  stringTopicF.toCharArray(sensorTopicF, topicLengthF);

  _mqttClient->beginMessage(
    sensorTopicF,
    _retainEvents,
    _qosEvents);

  _mqttClient->print(tempCurrentF[index]);
  _mqttClient->endMessage();
  _mqttClient->flush();

   String stringTopicC = _topicMqttBase + "/temp/" + index + "/C";
  int topicLengthC = stringTopicC.length() + 1;
  char sensorTopicC[topicLengthC];

  stringTopicC.toCharArray(sensorTopicC, topicLengthC);

  _mqttClient->beginMessage(
    sensorTopicC,
    _retainEvents,
    _qosEvents);

  _mqttClient->print(tempCurrentC[index]);
  _mqttClient->endMessage();

  Serial.print("[sensorTempDS18B20]: published temp data->");

  String stringTempF = String(tempCurrentF[index]);
  String stringTempC = String(tempCurrentC[index]);

  Serial.print("temp-sensor-");
  Serial.print(index);
  Serial.print(": -> ");
  Serial.print(String("C: " + stringTempC + " "));
  Serial.print(String("F: " + stringTempF + " "));
  Serial.println("");
}
