#ifndef PHOTORESISTOR_SENSOR_H
#define PHOTORESISTOR_SENSOR_H

/**
 * Récupérer les données d'une photorésistance : https://www.aranacorp.com/fr/mesure-de-luminosite-avec-une-photoresistance/
 */

#include <Arduino.h>

// par rapport à l'environnement dans lequel est réalisé le test, 
#define MIN_RAW_VALUE 0 // la valeur brute minimale de la photorésistance pour un environnement sombre
#define MAX_RAW_VALUE 1000 // la valeur brute maximale de la photorésistance pour un environnement bien éclairé

// la valeur de commande d'une led est comprise dans l'intervalle [0 ; 255]
#define MIN_LED_CMD 0 // la valeur minimale pour une led (elle sera éteinte)
#define MAX_LED_CMD 255 // la valeur maximale pour une led (elle sera complètement allumée)

class PhotoresistorSensor {
  private:
    float m_resistanceValue;
    float m_tension;

  public:
    PhotoresistorSensor(float resistanceValue, float tension);
    ~PhotoresistorSensor();

    // Enumération du niveau de lumière : DRK (sombre), ..., VRBRGHT (très éclairé).
    enum LightLevel {
      DRK, 
      LW, 
      BRGHT, 
      VRBRGHT,
      UNDEF
    };

    /**
     * Description du niveau de lumière.
     * Si la résistance sur la photorésistance >= 1000, 
     * celà signifie que la photorésistance ne recoit pas de lumière et qu'il fait sombre.
     * Si la résistance sur la photorésistance = 0, 
     * celà signifie que la photorésistance reçoit assez de lumière et que l'environnement est très éclairé.
     */
    static String lightLevelToString(LightLevel value);
        
    /**
     * Permet de fournir une valeur de commande pour une led par rapport
     * à la valeur brute de la photorésistance. 
     * 
     * Plus il fait sombre => 
     * plus la valeur brute de la photorésistance se rapproche de MIN_RAW_VALUE =>
     * plus la valeur de commande de la led sera élevée => la lampe brillera plus.
     * 
     * Plus l'éclairage ambiant est élevé =>
     * Plus la valeur brute de la photorésistance se rapproche de MAX_RAW_VALUE =>
     * plus la valeur de commande de la led sera faible => la lampe brillera moins.
     *
     * @param rawValue la valeur brute de la photorésistance.
     * @return une valeur de commande comprise dans l'intervalle [0 ; 255] 
     * (0 pour une led éteinte, 255 pour une led allumée).
     */
    int convertRawValueToLedCmd(int rawValue);

    /**
     * Convertir la valeur brute de la photorésistannce en niveau de luminosité.
     * 
     * LightLevel::DRK quand il fait sombre
     * LightLevel::VRBRGHT quand il fait très éclairé.
     * 
     * @param rawValue la valeur brute de la photorésistance.
     * @return niveau de lumière 
     */
    LightLevel convertRawValueToLightLevel(int rawValue);
};

#endif