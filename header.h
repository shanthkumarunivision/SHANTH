//headder.h

#include <signal.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <pthread.h>
//#include "functions_definition.h"

#define Device_Address 0x68						/*Device Address/Identifier for MPU6050*/

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47
#define TEMPERATURE  0x41
#define POWER_KEY 2  //BCM27
//#define LED 0 //BCM17
//extern int fd;
