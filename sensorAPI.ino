#include "photoresistorSensor.h"
#include "thermistorSensor.h"
#include "homescreen.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#define DELAY 200 // le délai entre deux itérations dans la fonction loop()

#define PORT 21000 // le port du serveur (port ouvert localement)

WebServer server(PORT);

// identifiants de connexion
const char* ssid = "SFR_606F";
const char* password = "OnePiece!";

// création d'instances des classes PhotoresistorSensor et ThermistorSensor pour manipuler les données
PhotoresistorSensor* photoresSensor = new PhotoresistorSensor(10000.0, 5.0);
ThermistorSensor *thermistorSensor = new ThermistorSensor(10000.0);

const int photoresistorPin = 32; // pour permettre la lecture de la valeur brute de la photorésistance. 
const int thermistorPin = A0; // pour permettre la lecture de la valeur brute de la thermistance.
const int ledPin = 2; // pour permettre l'envoi d'une valeur comprise dans l'intervalle [0 ; 255] à une lampe à Led.
const int controledLedPin = 26;

int photoresistorRawValue = 0; // pour stocker la valeur brute de la photorésistance
PhotoresistorSensor::LightLevel lightLevel = PhotoresistorSensor::LightLevel::UNDEF; // pour stocker le niveau de lumière

int cmdValueForLed = 0; // pour stocker la valeur de commande à envoyer à la led
int cmdForControledLed = 0;

int thermistorRawValue = 0; // pour stocker la valeur brute de la thermistance
float temperature = 0.0; // pour stocker la valeur de la température

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200); // démarrer la communication avec le moniteur série

  screenInit();
  screenPresentation(); // afficher l'écran de présentation

  delay(3000); // pour afficher l'écran de présentation pendant 3 secondes

  screenServerLoading(); // afficher l'écran de chargement

  // Configurer la broche spécifier dans la constante photoresistorPin en entrée
  pinMode(photoresistorPin, INPUT);
  // Configurer la broche spécifier dans la constante thermistorPin en entrée  
  pinMode(thermistorPin, INPUT);
  // Configurer la broche spécifier dans la constante ledPin en sortie  
  pinMode(ledPin, OUTPUT);
  // Configurer la broche spécifier dans la constante controledLedPin en sortie  
  pinMode(controledLedPin, OUTPUT);
  // Initialiser la connexion wifi
  initWifiConnection(ssid, password);
  // Configurer les routes
  setupRoutes();
  // Lancer le serveur
  server.begin();
}

void loop() {
  // Lecture de la valeur brute de la photorésistance
  photoresistorRawValue = analogRead(photoresistorPin);
  Serial.print("valeur brute de la photorésistance : ");
  Serial.println(photoresistorRawValue);

  // Conversion de la valeur brute de la photorésistance en une valeur de cmd pour la led liée à la broche ledPin
  cmdValueForLed = photoresSensor->convertRawValueToLedCmd(photoresistorRawValue);
  Serial.print("valeur de cmd pour la led : ");
  Serial.println(cmdValueForLed);

  // Obtention du niveau de lumière
  lightLevel = photoresSensor->convertRawValueToLightLevel(photoresistorRawValue);
  Serial.print("description du niveau de luminosité : ");
  Serial.println(PhotoresistorSensor::lightLevelToString(lightLevel));

  // valeur brute de la thermistance
  thermistorRawValue = analogRead(thermistorPin);
  Serial.print("valeur brute de la thermistance : ");
  Serial.println(thermistorRawValue);

  // température en °C
  temperature = thermistorSensor->convertRawValueToCTemperature(thermistorRawValue);
  Serial.print("temperature : ");
  Serial.println(temperature);

  // affichage des données à l'écran
  screenDisplayData();

  // Envoi de la valeur de cmd pour la led liée à la broche ledPin
  analogWrite(ledPin, cmdValueForLed);
  analogWrite(controledLedPin, cmdForControledLed);

  server.handleClient();

  // Attendre un certain temps avant la prochaine lecture
  delay(DELAY);
}

// initialisation de l'écran 
void screenInit() {
  tft.init();
  tft.setRotation(1);
}

// affichage d'une image de prpésentation du groupe
void screenPresentation() {
  tft.fillScreen(TFT_LIGHTGREY);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 240, 135, homescreen);
}

// affichage d'un texte montrant que le serveur est en cours de chargement
void screenServerLoading() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);
  tft.setCursor((240/2)-(strlen("lancement du serveur ...")*5/2), 135/2);  // Positionner le curseur
  tft.println("lancement du serveur ...");  // Afficher le texte
}

// affichage des données de température et lumières ambiante à l'écran
void screenDisplayData() {
  tft.fillScreen(TFT_BLACK);

  int x = 30, y = 30; // Position de départ
  tft.drawCircle(x, y, 11, TFT_WHITE);
  if(lightLevel == 0) {
    tft.fillCircle(x, y, 10, TFT_BLACK); // Dessiner le globe de l'ampoule
  }
  else {
    tft.fillCircle(x, y, 10, TFT_YELLOW); // Dessiner le globe de l'ampoule
  }
  tft.drawLine(x-5, y, x+5, y, TFT_WHITE); // Dessiner la partie supérieure de la tige
  tft.drawLine(x, y, x, y+10, TFT_WHITE); // Dessiner la partie inférieure de la tige
  tft.drawRect(x-2, y+10, 5, 2, TFT_WHITE); // Dessiner la base de l'ampoule
  tft.setCursor(x+20, y); // Positionner le curseur
  tft.setTextSize(2);  // Définir la taille du texte
  tft.setTextColor(TFT_WHITE);  // Définir la couleur du texte
  tft.print(PhotoresistorSensor::lightLevelToString(lightLevel)); // Décrire le niveau de lumière

  x = 30; 
  y = 60; // Position de départ
  int tempPos = map(temperature, 0, 50, 0, 60); // Mapper la température sur la hauteur du thermomètre
  tft.drawRect(x-5, y, 10, 60, TFT_WHITE); // Dessiner le corps du thermomètre
  tft.fillRect(x-3, y+60-tempPos, 6, tempPos, TFT_RED); // Dessiner la colonne d'huile
  tft.drawCircle(x, y, 10, TFT_WHITE); // Dessiner le bulbe du thermomètre
  tft.setCursor(x+20, y+20); // Positionner le curseur
  tft.print(temperature);
  tft.print(" deg C");
}

/**
 * Connecter le dispositif à un réseau WIFI
 *
 * @param ssid identifiant WIFI
 * @param password mot de passe WIFI
 */
void initWifiConnection(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connecté à ");
  Serial.println(ssid);
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
}

/**
 * envoyer au client un message montrant que l'API de l'ESP32 est ouvert
 */
void handleRoot() {
  StaticJsonDocument<200> doc;
  
  const String msg = "L'API de l'ESP32 ouvert";

  doc["msg"] = msg;
  
  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

/**
 * envoyer au client les données collecté depuis les capteurs liés à l'ESP32.
 * 
 * {
 *    "photoresistor": {
 *      "raw_value": int,
 *      "light_level": int,
 *      "light_level_desc": String
 *    },
 *    "thermistor": {
 *      "raw_value": int,
 *      "temperature": int,
 *      "temperature_desc": String
 *    }
 * }
 */
void handleData() {
  StaticJsonDocument<200> doc;
  StaticJsonDocument<200> photoresistorObject;
  StaticJsonDocument<200> thermistorObject;

  photoresistorObject["raw_value"] = photoresistorRawValue;
  photoresistorObject["light_level"] = lightLevel;
  photoresistorObject["light_level_desc"] = PhotoresistorSensor::lightLevelToString(lightLevel);

  thermistorObject["raw_value"] = thermistorRawValue;
  thermistorObject["temperature"] = temperature;
  thermistorObject["temperature_desc"] = ThermistorSensor::temperatureDescription(temperature);

  doc["photoresistor"] = photoresistorObject;
  doc["thermistor"] = thermistorObject;

  String json;
  serializeJson(doc, json);

  server.send(200, "application/json", json);
}

// envoi des informations sur es capteurs
void handleSensorsInfos() {
  server.send(200, "text/plain", "thermistor 103;photoresistor");
}

/**
 * résultats retournés pour une route introuvable.
 */
void handleNotFound() {
  StaticJsonDocument<200> doc;

  doc["thermistor"] = "Route introuvable";
  doc["uri"] = server.uri();
  doc["method"] = (server.method() == HTTP_GET) ? "GET" : "POST";
  doc["args"] = server.args();
  
  String json;
  serializeJson(doc, json);
  server.send(404, "application/json", json);
}

void setCmdForControledLedPin(int cmd) {
  cmdForControledLed = cmd;
}

void handleLighting() {
  if (server.hasArg("cmd")) {
    String cmd = server.arg("cmd");

    int cmdInt = cmd.toInt();

    // Traitement de la données reçue
    if (cmdInt>=0 && cmdInt<=255) {
      setCmdForControledLedPin(cmdInt);
      // Envoyer une réponse à la requête
      if (cmdInt > 0) {
        server.send(200, "text/plain", "LED ALLUMEE !");
      }
      else {
        server.send(200, "text/plain", "LED ETEINTE !");
      }
    }
    else {
      server.send(200, "text/plain", "DONNEES INVALIDES !");
    }
  } 
  else {
    server.send(200, "text/plain", "VALEUR CMD NON FOURNIES !");
  }
}

/**
 * configuration des routes à partir des fonctions handle prédéfinies.
 */
void setupRoutes() {
  server.on("/esp32api/", handleRoot);
  server.on("/esp32api/data", handleData);
  server.on("/esp32api/infos", handleSensorsInfos);
  server.on("/esp32api/lighting", handleLighting);
  server.onNotFound(handleNotFound);
}