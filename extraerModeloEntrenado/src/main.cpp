#include <Arduino.h>
#include "NeuralNetwork.h"
#include "leerSensor.h"
#include "prepararDatos.h"
NeuralNetwork *nn;
int16_t *lectura;
int16_t *features;
void setup()
{
  Serial.begin(115200);
  nn = new NeuralNetwork();
  Serial.println("setup");
}

void loop()
{
  Serial.println("Realice un movimiento");
  delay(3000);
  lectura=getData();
  features=getFeatures(lectura);
  for (int i = 0; i < sizeof(features); i++){
    nn->getInputBuffer()[i]= features[i]; 
    Serial.print(features[i]);
  }
  Serial.println();
  Serial.println(nn->predict());
  delay(10000);
}
