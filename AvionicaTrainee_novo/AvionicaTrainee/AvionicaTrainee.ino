//Incluindo Bibliotecas
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; //I2C

#define PRESSAO_REFERENCIA 1013.25 /* Pressão de referência para o BMP280 */
#define SIZE_BUFFER_ALTITUDE 50 /* Tamanho da janela de amostragem de altitudes */
#define REDUNDANCY 2     /* Quantidade de resultados seguidos iguais necessários para ativar o skib */
#define SENSIBILITY 0.005 /* Valor de m mínimo necessário para perceber uma queda ou subida*/
#define TIME_MEASUREMENT 10 /* Tempo para -cada- medida de altitude do buffer ser feita */
#define TIME_TO_EXPLODE 300 /* Tempo após detectada a queda para a explosão do skib */
#define ACTIVATION_INTERVAL 300 /* Tempo durante o qual o sinal de ativação ficará em ALTO */ 

#define PINO_ACIONAMENTO 8 

float altitudes[SIZE_BUFFER_ALTITUDE];
float altitude_lida = 0;
int index = 0;
int counter_ativation = 0;

typedef struct Reta{
  float m;
  float b;
};

void setup() {
  // Iniciando a comunicação serial
  Serial.begin(9600);
  Serial.println(F("Aviônica Trainee"));
  
  // Inicializando o Pino de acionamento
  pinMode(PINO_ACIONAMENTO, OUTPUT);
  digitalWrite(PINO_ACIONAMENTO, LOW);

  if (!bmp.begin(0x76)) { 
    Serial.println(F("Erro de Conexão com o BMP280"));
    while (1) delay(10);
  }
}

void loop() {
    altitude_lida = bmp.readAltitude(PRESSAO_REFERENCIA);

    if (index < SIZE_BUFFER_ALTITUDE){
      altitudes[index] = altitude_lida;
      index += 1;
    } else{
      index = 0;
      /*debugPrintAltitudes();*/

      Reta reta = leastSquaresAproximation();
      
      /*debugRetaGerada(reta.m, reta.b);*/
      debugQueda(reta.m);
      redundancySystem(reta);
    }

    delay(TIME_MEASUREMENT);
}

void redundancySystem(Reta r){
  if (isFalling(r)){
    counter_ativation++;
    if (counter_ativation == REDUNDANCY){
      activateSkib();
    }
  } else{
    counter_ativation = 0;
  }
}

void activateSkib(){
  Serial.println("BOOOOOMMMM");

  delay(TIME_TO_EXPLODE);
  digitalWrite(PINO_ACIONAMENTO, HIGH);
  delay(ACTIVATION_INTERVAL);
  digitalWrite(PINO_ACIONAMENTO, LOW);
}

void debugRetaGerada(float m, float b){
  Serial.println("Aproximação linear obtida: ");
  Serial.print("y = ");
  Serial.print(m);
  Serial.print("x + ");
  Serial.println(b);
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
  if (m < -SENSIBILITY){
    Serial.println("Caindo");
  } 
  else if (m > SENSIBILITY) {
    Serial.println("Subindo");
  }
  else {
    Serial.println("Estável");
  }
}

bool isFalling(Reta r){
  return (r.m < -SENSIBILITY);
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