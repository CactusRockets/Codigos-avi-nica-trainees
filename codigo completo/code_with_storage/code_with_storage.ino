/* CONFIGURAÇÕES BMP e MPU */

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>

#define BMP_ADRESS 0x76
#define MPU_ADRESS 0x68

#define RANGE_ERROR_ALTITUDE 5
#define SKIB 5
#define BUZZER 3

#define TIME_LOOP 100
#define DEFAULT_PRESSURE 1017.3
#define TIME_ACTIVATION 1000
#define NUM_BIPS 3
#define TIME_BIP 250

Adafruit_BMP280 bmp;

float maximumVariationAltitude = 0;
float maximumAltitudeValue = 0;
float altitude = 0;
float variationAltitude = 0;
float initialAltitude = 0;
bool isDropping = false;
bool parachuteActivated = false;

bool max_altitude_writed = false;

void setupBMP() {
  if(!bmp.begin(BMP_ADRESS)) {
    Serial.println("Failed to find BMP280 chip");
    while(1);
  }
  Serial.println("BMP conectado!");

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X4,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X8,     /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1);   /* Standby time. */
}

void readBMP() {
  altitude = bmp.readAltitude(DEFAULT_PRESSURE);
  variationAltitude = bmp.readAltitude(DEFAULT_PRESSURE) - initialAltitude;

  if(altitude > maximumAltitudeValue) {
    maximumAltitudeValue = altitude;
  }
}

void printBMP() {
  String dados = String(initialAltitude, 3)         
      + "," + String(variationAltitude, 3)
      + "," + String(maximumVariationAltitude, 3)    
      + "," + String(altitude, 3)
      + "," + String(maximumAltitudeValue, 3); 

  Serial.println(dados);
}

void analyzeStateOfRocket() {
  if((maximumAltitudeValue - altitude) > RANGE_ERROR_ALTITUDE) {
    isDropping = true;
  }
  maximumVariationAltitude = maximumAltitudeValue - initialAltitude;
}

void activateSkib() {
  Serial.println("Skib ativado!");
  digitalWrite(SKIB, HIGH);
  delay(TIME_ACTIVATION);
  Serial.println("Skib já foi ativado!");
  digitalWrite(SKIB, LOW);
  
  parachuteActivated = true;
}

void activeBuzzerStart(){
  for (int i = 0; i < NUM_BIPS; i++){
    Serial.println("BANANA");
    digitalWrite(BUZZER, HIGH);
    delay(TIME_BIP);
    digitalWrite(BUZZER, LOW);
    delay(TIME_BIP);
  }
}

void activeBuzzerSkib(){
  digitalWrite(BUZZER, HIGH);
}

void setup() {
  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(SKIB, OUTPUT);

  setupBMP();

  initialAltitude = bmp.readAltitude(DEFAULT_PRESSURE);
  activeBuzzerStart();
}

void loop() {
  readBMP();
  analyzeStateOfRocket();

  printBMP();

  if(isDropping) {
    activateSkib();
    activeBuzzerSkib();
    if(max_altitude_writed == false) {
      EEPROM.put(0, maximumVariationAltitude);
      max_altitude_writed = true;
    }
  }

  delay(TIME_LOOP);
}
