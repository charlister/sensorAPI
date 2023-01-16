#include "thermistorSensor.h"

ThermistorSensor::ThermistorSensor(float resistanceValue) {
  m_resistanceValue = resistanceValue;
}

ThermistorSensor::~ThermistorSensor() {}

float ThermistorSensor::convertRawValueToCTemperature(int rawValue) {
  float R2 = m_resistanceValue * (4095.0 / (float)rawValue - 1.0); // calculer la résistance sur la thermistance
  float logR2 = log(R2);

  float temp = (1.0 / (A + B*logR2 + C*logR2*logR2*logR2)); // température en Kelvin
  temp = temp - 273.15; // convertir la température en °C

  return temp;
}

String ThermistorSensor::temperatureDescription(float temperature) {
  String result = "";
  if (temperature >= 30) {
    result = "CHAUD";
  }
  else if (temperature >= 20) {
    result = "TEMPERE";
  }
  else if (temperature >= 10) {
    result = "FRAIS";
  }
  else if (temperature >= 0) {
    result = "FROID";
  }
  else {
    result = "TRES_FROID";
  }
  return result;
}