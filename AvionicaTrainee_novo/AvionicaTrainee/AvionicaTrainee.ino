//Incluindo Bibliotecas
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; //I2C

#define PRESSAO_REFERENCIA 1013.25
#define SIZE_BUFFER_ALTITUDE 100

float altitudes[SIZE_BUFFER_ALTITUDE];
float altitude_lida = 0;
int index = 0;

typedef struct Reta{
  float m;
  float b;
};

void setup() {
  //Iniciando a comunicação serial
  Serial.begin(9600);
  // Imprimindo Mensagem de teste no Monitor Serial
  Serial.println(F("BMP280 teste"));
  
  if (!bmp.begin(0x76)) { 
    Serial.println(F("Erro de Conexão com o BMP280"));
    while (1) delay(10);
  }
}

void loop() {
    /*
    Serial.print(F("Altitude Aprox = "));
    */

    altitude_lida = bmp.readAltitude(PRESSAO_REFERENCIA);
    /*
    Serial.print(altitude_lida); 
    
    Serial.println(" m");
    Serial.print(index);
    Serial.println();
    */

    if (index < SIZE_BUFFER_ALTITUDE){
      altitudes[index] = altitude_lida;
      index += 1;
    } else{
      index = 0;
      /*debugPrintAltitudes();*/

      Reta reta = leastSquaresAproximation();
      /*
      Serial.println(" Aproximação linear obtida: ");
      Serial.print("y = ");
      Serial.print(reta.m);
      Serial.print("x + ");
      Serial.println(reta.b);
      */
      debugQueda(reta.m);
    }


    delay(10);
}

void debugPrintAltitudes(){
  for (int i = 0; i < SIZE_BUFFER_ALTITUDE; i++){
    Serial.print(altitudes[i]);
    Serial.print(" m | ");
  }
  Serial.println();
}

void debugQueda(float m){
  Serial.println(m);
  if (m < -0.05){
    Serial.println("Caindo");
  } 
  else if (m > 0.05) {
    Serial.println("Subindo");
  }
  else {
    Serial.println("Estável");
  }
}

Reta leastSquaresAproximation(){
  // Método dos mínimos quadrados para conseguir uma aproximação linear do conjunto de altitudes lidas
  // Se houver uma tendência geral de queda (m < 0, em y = mx + b, aprox. linear), então temos uma queda
  float sum_x = 0;
  float sum_y = 0;
  float sum_x2 = 0;
  float sum_y2 = 0;
  float sum_xy = 0;

  for (int i = 0; i < SIZE_BUFFER_ALTITUDE; i++){
    sum_x += i;
    sum_y += altitudes[i];
    sum_x2 += i * i;
    sum_y2 += altitudes[i] * altitudes[i];
    sum_xy = sum_xy + i * altitudes[i];
  }

  int n = SIZE_BUFFER_ALTITUDE;
  Reta reta;
  reta.m = (SIZE_BUFFER_ALTITUDE * sum_xy - sum_x * sum_y) / (SIZE_BUFFER_ALTITUDE * sum_x2 - sum_x * sum_x);
  reta.b = (sum_x2 * sum_y - sum_x * sum_xy) / (SIZE_BUFFER_ALTITUDE * sum_x2 - sum_x * sum_x);

  return reta;
}