#include "driver/i2c.h" //libreria necesaria para el control de I2C 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h" //incluye freertos en el esp32 
#include "freertos/task.h"      //proporciona la funcionalidad multitarea
#include "esp_system.h"         /*inicia el sistema al configurar los perifericos en esp32*/
#include "esp_log.h"            /*implementa la funcionalidad permanente del registro en el codigo esp*/ 

#define BUFF_SIZE 6             //Definicion del tamano del Buff 

#define I2C_MASTER_SCL_IO    22   //Puerto GPI0 designado para la linea de reloj maestro (SCL)  
#define I2C_MASTER_SDA_IO    21   //Puerto GPIO designado para la linea de datos del controlador maestro (SDA)  
#define I2C_MASTER_NUM I2C_NUM_1   //Puerto 1 de I2C  
#define I2C_MASTER_TX_BUF_DISABLE   0  //Define que el bloque maestro no necesita de un buffer  
#define I2C_MASTER_RX_BUF_DISABLE   0   //Define que el bloque maestro no necesita de un buffer  
#define I2C_MASTER_FREQ_HZ    100000    //Frecuencia de reloj de controlador maestro  

#define MPU6050_ADDR  0x68    //Direccion asignada al esclavo para MPU6050 
#define WRITE_BIT  I2C_MASTER_WRITE //Construye un enlace de comando para que el maestro pueda enviar una cantidad de bits a un esclavo  
#define READ_BIT   I2C_MASTER_READ  //Construye un enlace de comando para que el maestro pueda leer las senales de bits enviadas por un esclavo  
#define ACK_CHECK_EN   0x1     //Verifica la confirmacion de un esclavo  
#define ACK_CHECK_DIS  0x0     //Deshabilita la verificacion de confirmacion de un esclavo  
/********************************/
#define ACK_VAL    i2c_ack_type_t(0x0)         //EDITADO Valor de confirmacion de mensaje recibido 
#define NACK_VAL   i2c_ack_type_t(0x1)         //EDITADO Valor de confirmacion de no recepcion de mensaje  

#define ACCE_START_ADD 0x3B    //Direccion de registro para empezar a leer los valores crudos (sin procesar) del acelerometro
#define GYRO_START_ADD 0x43    //Direccion de registro para empezar a leer los valores crudos (sin procesar) del giroscopio 
//Variables de tipo entero de 16 bits, que almacenaran los valores del acelerometro y 
//el giroscopio sin procesar (raw).  
int16_t ax, ay, az,gx,gy, gz;
//Puntero de variables sin signo de tipo entero de 8 bits (para gestionar memoria dinamica)
uint8_t* data_rd;

/**
 * @brief //Funcion que inicializa y enciende el sensor MPU6050 
 */
esp_err_t mpu6050_init(i2c_port_t i2c_num)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x6B, ACK_CHECK_EN); //Direccion de registro de energia
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); ///Enciende el chip interno del sensor
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Funcion encargada de leer los valores sin procesar obtenidos por el acelerometro
 */
esp_err_t mpu6050_read_acce(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
{
    //printf("mpu6050_read_acce ***********\n");
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, ACCE_START_ADD, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

/**
 * @brief Lee los valores sin procesar del giroscopio
 */
esp_err_t mpu6050_read_gyro(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, GYRO_START_ADD, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( MPU6050_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Inicializacion del controlador maestro I2C
 */
void i2c_master_init()
{
    printf("i2c master init ***********\n");
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Funcion encargada de controlar un error al momento de inicializar el sensor, 
 * en caso de que se presente un error mostrara en la terminal un mensaje de fallo al iniciar
 *  y el error obtenido. Caso contrario imprimira un mensaje de exito y el sistema podra continuar
 */
void mpu6050_test_raw()
{
    printf("mpu6050_test_raw ***********\n");
    int ret;
    ret = mpu6050_init(I2C_MASTER_NUM);
    //El sistema sigue ejecutandose hasta que se haya iniciado correctamente 
    while(ret != ESP_OK) {
        printf("INIT FAILED... Retry\n");
        vTaskDelay(100/ portTICK_RATE_MS);
        ret = mpu6050_init(I2C_MASTER_NUM);
    }
    printf("INIT SUCCESS...\n");
    vTaskDelay(100/ portTICK_RATE_MS); //Delay to init power on mpu6050
}

/**
 * @brief //Llama a las funciones i2c_master_init() y 
 * mpu6050_test_raw() para inicializar por completo al sensor MPU6050 
 */
void mpu6050_myinit()
{
    i2c_master_init();
    mpu6050_test_raw();
}

/**
 * @brief //Funcion encargada de imprimir por puerto serial 
 * los datos leidos por el giroscopio y acelerometro del sensor MPU6050
 * frecuencia de 100Hz
 */
int16_t *getData(){
    int16_t lectura[1500];
    data_rd = (uint8_t*) malloc(BUFF_SIZE);
    int muestra=0;
    while(muestra<250){
        mpu6050_read_acce(I2C_MASTER_NUM, data_rd, BUFF_SIZE);
        ax = (data_rd[0] << 8) + data_rd[1];   
        ay = (data_rd[2] << 8) + data_rd[3];
        az = (data_rd[4] << 8) + data_rd[5];
        mpu6050_read_gyro(I2C_MASTER_NUM, data_rd, BUFF_SIZE);
        gx = (data_rd[0] << 8) + data_rd[1];   
        gy = (data_rd[2] << 8) + data_rd[3];
        gz = (data_rd[4] << 8) + data_rd[5];
        lectura[0+6*muestra]=ax;
        lectura[1+6*muestra]=ay;
        lectura[2+6*muestra]=az;
        lectura[3+6*muestra]=gx;
        lectura[4+6*muestra]=gy;
        lectura[5+6*muestra]=gz;
        //printf("%d, %d, %d,%d, %d, %d\n", ax,ay,az,gx,gy,gz);
        free(data_rd);
        vTaskDelay(10/portTICK_PERIOD_MS);
        muestra+=1;
    }
    return lectura;
}


