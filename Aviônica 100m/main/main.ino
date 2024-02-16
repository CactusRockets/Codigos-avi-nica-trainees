#include <SPI.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SD.h>

#include "microSD.h"

#define BMP_ADRESS 0x76
#define MPU_ADRESS 0x68

#define rangeMaximumAltitudeValue 0.5

Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp;

float temperature = 0;
float altitudeAtual = 0;
float maximumAltitudeValue = 0;

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
  temperature = bmp.readTemperature();
  altitudeAtual = bmp.readAltitude(1017.3);
  
  if(altitudeAtual > maximumAltitudeValue) {
    maximumAltitudeValue = altitudeAtual;
  }
}

void setup() {

}

void loop() {

}