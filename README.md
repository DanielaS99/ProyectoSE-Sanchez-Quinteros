# ProyectoSE-Sanchez-Quinteros
Daniela Sánchez Orozco - Cristina Quinteros Molina
Repositorio del  proyecto de Sistemas Embebidos que consiste en el entrenamiento de un modelo clasificador de lenguaje de señas utilizando un microcontrolador ESP32 y un sensor MPU6050
Se creó un repositorio en la plataforma Github donde se almacenan todos los archivos utilizados para el desarrollo de este proyecto. Esto incluye, el archivo lecturaMPU6050.c que corresponde al código en ESP-IDF para la adquisición de datos del sensor, el cual se utilizó para la creación del dataset inicial. Se incluye también el dataset completo de la adquisición de datos realizada en la carpeta data. La carpeta LenguajeSenas-Sanchez-Quinteros posee el jupyter notebook utilizado para el entrenamiento del modelo de clasificación en Python. Además, la carpeta extraerModeloEntrenado posee el proyecto de PlatformIO donde se incluye el procesamiento de datos y la extracción del modelo entrenado en códigos C++. El archivo comprimido lib incluye los archivos de la librería TensorFlow necesarios para el proyecto de PlatformIO. Finalmente, se presentan otros archivos como la matriz del modelo entrenado en el archivo model_data.cc y el diagrama esquemático del proyecto.
