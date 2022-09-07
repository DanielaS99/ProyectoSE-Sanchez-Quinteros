#include <Arduino.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

//Normaliza los datos leidos (250 por cada una de las 6 variables) 
int16_t *normalizar(int16_t* lectura){
    int16_t datosNorm[1500];
    list<int> listAX,listAY,listAZ,listGX,listGY,listGZ;
    for(int i = 0; i < (sizeof(lectura)/6); i++){
    //Se dividen los datos en listas de sus respectivas columnas
        listAX.push_back(lectura[i*6+0]);
        listAY.push_back(lectura[i*6+1]);
        listAZ.push_back(lectura[i*6+2]);
        listGX.push_back(lectura[i*6+3]);
        listGY.push_back(lectura[i*6+4]);
        listGZ.push_back(lectura[i*6+5]);
    }
    //Se ordenan las columnas en orden ascendente
    listAX.sort();
    listAY.sort();
    listAZ.sort();
    listGX.sort();
    listGY.sort();
    listGZ.sort();
    //Valores minimos de cada columna
    int16_t minAX=listAX.front();
    int16_t minAY=listAY.front();
    int16_t minAZ=listAZ.front();
    int16_t minGX=listGX.front();
    int16_t minGY=listGY.front();
    int16_t minGZ=listGZ.front();
     //Valores maximos de cada columna
    int16_t maxAX=listAX.back();
    int16_t maxAY=listAY.back();
    int16_t maxAZ=listAZ.back();
    int16_t maxGX=listGX.back();
    int16_t maxGY=listGY.back();
    int16_t maxGZ=listGZ.back();
    //Normalizar datos
    for(int i2 = 0; i2 < (sizeof(lectura)/6); i2++){
        datosNorm[0+i2*6]=(lectura[i2*6+0]-minAX)/(maxAX-minAX);
        datosNorm[1+i2*6]=(lectura[i2*6+1]-minAY)/(maxAY-minAY);
        datosNorm[2+i2*6]=(lectura[i2*6+2]-minAZ)/(maxAZ-minAZ);
        datosNorm[3+i2*6]=(lectura[i2*6+3]-minGX)/(maxGX-minGX);
        datosNorm[4+i2*6]=(lectura[i2*6+4]-minGY)/(maxGY-minGY);
        datosNorm[5+i2*6]=(lectura[i2*6+5]-minGZ)/(maxGZ-minGZ);
    }
    return datosNorm;
}

//Obtiene la caracteristica RMS de las 6 variables con una ventana de 50 muestras (Total: 30 valores, 5x6)
int16_t *getFeatures(int16_t* lectura){
    int16_t* datosNorm=normalizar(lectura);
    uint8_t j=0;
    int16_t features[30];
    int32_t sumAX=0;
    int32_t sumAY=0;
    int32_t sumAZ=0;
    int32_t sumGX=0;
    int32_t sumGY=0;
    int32_t sumGZ=0;
    for(int i = 0; i < (sizeof(datosNorm)/6); i++){
        sumAX+=pow(datosNorm[0+i*6],2);
        sumAY+=pow(datosNorm[1+i*6],2);
        sumAZ+=pow(datosNorm[2+i*6],2);
        sumGX+=pow(datosNorm[3+i*6],2);
        sumGY+=pow(datosNorm[4+i*6],2);
        sumGZ+=pow(datosNorm[5+i*6],2);
        if((i+1)%50==0){
            features[j]=sqrt(sumAX/50);
            features[j+5]=sqrt(sumAX/50);
            features[j+10]=sqrt(sumAX/50);
            features[j+15]=sqrt(sumAX/50);
            features[j+20]=sqrt(sumAX/50);
            features[j+25]=sqrt(sumAX/50);
            j+=1;
            sumAX=0;
            sumAY=0;
            sumAZ=0;
            sumGX=0;
            sumGY=0;
            sumGZ=0;
        }
    }
    return features;
}