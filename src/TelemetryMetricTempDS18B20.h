#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <TelemetryMetricReporter.h>

#ifndef LINENTOOLS_TELEM_METRIC_TEMP_DS18B20_H
#define LINENTOOLS_TELEM_METRIC_TEMP_DS18B20_H

class TelemetryMetricTempDS18B20 : public TelemetryMetricReporter {
private:
  OneWire* oneWire;
  DallasTemperature* sensors;
  uint8_t countSensors;

  /* arrays for storing temps */
  double tempCurrentF[64];
  double tempCurrentC[64];
  double tempPreviousF[64];

  void _recordTemperatures();
  void _publishIndex(uint8_t index);

protected:
  String _getTopicEvents() override;

public:
  TelemetryMetricTempDS18B20(
    uint8_t pin,
    String topicMqttBase,
    bool retainEventMessages,
    uint8_t qosEventMessages)
    : TelemetryMetricReporter( // call super constructor
      topicMqttBase, 
      retainEventMessages, 
      qosEventMessages) {
    /* init OneWire and DS18B20 sensor */
    oneWire = new OneWire(pin);
    sensors = new DallasTemperature(oneWire);
  };
  void begin(MqttClient* mqttClient) override;
  void publishMetric() override;
  void publishConfiguration() override;
  float getMetricValue() override;
};

#endif