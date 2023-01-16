#include "photoresistorSensor.h"

PhotoresistorSensor::PhotoresistorSensor(float resistanceValue, float tension) {
  m_resistanceValue = resistanceValue;
  m_tension = tension;
}

PhotoresistorSensor::~PhotoresistorSensor() {}

String PhotoresistorSensor::lightLevelToString(LightLevel value) {
  String result = "";
  switch (value)
  {
    case DRK: 
      result = "DARK";
      break;
    case LW: 
      result = "LOW";
      break;
    case BRGHT: 
      result = "BRIGHT";
      break;
    case VRBRGHT: 
      result = "VERY_BRIGHT";
      break;
    case UNDEF:
      result = "UNDEFINED";
      break;
  }
  return result;
}

int PhotoresistorSensor::convertRawValueToLedCmd(int rawValue) {
  int result = map(
    rawValue, 
    MIN_RAW_VALUE, /* à cette valeur sera associé MAX_LED_CMD */
    MAX_RAW_VALUE, /* à cette valeur sera associé MIN_LED_CMD */
    MAX_LED_CMD, 
    MIN_LED_CMD
  );
  result = max(result, MIN_LED_CMD);
  result = min(result, MAX_LED_CMD);
  return result;
}

PhotoresistorSensor::LightLevel PhotoresistorSensor::convertRawValueToLightLevel(int rawValue) {
  float voltage = rawValue * m_tension / 1023.0;
  float resistance = (m_tension - voltage) / voltage * m_resistanceValue;
  PhotoresistorSensor::LightLevel lightLevel;

  if (resistance >= 1000) {
    lightLevel = DRK;
  } else if (resistance >= 300) {
    lightLevel = LW;
  } else if (resistance >= 50) {
    lightLevel = BRGHT;
  } else {
    lightLevel = VRBRGHT;
  }

  return lightLevel;
}