#include "i2c_mpu6050.h"
#include "i2c.h"
#include "stm32f4xx_hal_i2c.h"
#include <string.h>
#include <stdio.h>
uint8_t i;
float ggx;
float ggy;
float ggz;
uint8_t WriteBuffer[BufferSize],ReadBuffer[BufferSize];
MPUDataTypedef mpu_data = {0,0,0,0,0,0,0,0,0,0,0,0};

void mpu6050dataread(void){
//	printf("kaishiduqu");
if(HAL_I2C_Mem_Read(&hi2c2, 0xa1, AX, I2C_MEMADD_SIZE_8BIT,ReadBuffer,BufferSize, 0x10)!=HAL_OK){
printf("\r\nHAL_I2C_Mem_Read Error\r\n");
}
mpu_data.ax=ReadBuffer[1]<<8|ReadBuffer[0]/32768*16;
mpu_data.ay=ReadBuffer[3]<<8|ReadBuffer[2]/32768*16;
mpu_data.az=ReadBuffer[5]<<8|ReadBuffer[4]/32768*16;
//mpu_data.gx=(float)(ReadBuffer[7]<<8|ReadBuffer[6])/32768*2000;
//mpu_data.gy=(float)(ReadBuffer[9]<<8|ReadBuffer[8])/32768*2000;
//mpu_data.gz=(float)(ReadBuffer[11]<<8|ReadBuffer[10])/32768*2000;
ggx=(float)ReadBuffer[6]/32768*2000;
ggy=(float)ReadBuffer[8]/32768*2000;
ggz=(float)ReadBuffer[10]/32768*2000;
if(ggx<(float)15.5){
mpu_data.gx=ggx;
}else mpu_data.gx=0;
if(ggy<(float)15.5){
mpu_data.gy=ggy;
}else mpu_data.gx=0;
if(ggz<(float)15.5){
mpu_data.gz=ggz;
}else mpu_data.gz=0;
mpu_data.mx=ReadBuffer[13]<<8|ReadBuffer[12];
mpu_data.my=ReadBuffer[15]<<8|ReadBuffer[14];
mpu_data.mz=ReadBuffer[17]<<8|ReadBuffer[16];
mpu_data.real_roll=ReadBuffer[19]<<8|ReadBuffer[18]/32768*180;
mpu_data.real_pitch=ReadBuffer[21]<<8|ReadBuffer[20]/32768*180;
mpu_data.real_yaw=ReadBuffer[23]<<8|ReadBuffer[22]/32768*180;
}

