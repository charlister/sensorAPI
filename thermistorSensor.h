#ifndef THERMISTOR_SENSOR_H
#define THERMISTOR_SENSOR_H

#include <Arduino.h>

/**
 * montage d'un circuit pour récupérer la température : https://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/
 * information sur le thermistor 103 : https://udvabony.com/product/10k-%CF%89-ohm-ntc-thermistor/
 */

class ThermistorSensor {
  private:
    float m_resistanceValue;

    // les coefficient de steinhart-hart pour le thermistor
    const float A = 1.009249522e-03;
    const float B = 2.378405444e-04;
    const float C = 2.019202697e-07;

  public: 
    ThermistorSensor(float resistanceValue);
    ~ThermistorSensor();

    /**
     * Permet de convertir la valeur brute lue entre la thermistance et la résistance en °C.
     *
     * @param rawValue valeur brute entre la thermistance et la résistance.
     * @return une température exprimée en °C.
     */
    float convertRawValueToCTemperature(int rawValue);

    /**
     * Description générale de la température
     * 
     * Chaud : 30 degrés Celsius et plus
     * Tempéré : entre 20 et 30 degrés Celsius
     * Frais : entre 10 et 20 degrés Celsius
     * Froid : moins de 10 degrés Celsius
     * Très froid : moins de 0 degrés Celsius
     *
     * @param temperature température en °C
     * @return la description associée à la température.
     */
    static String temperatureDescription(float temperature);
};

#endif