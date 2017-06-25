#ifndef _MPU_H_
#define _MPU_H_

//#include "my_math.h"
#include "mytype.h"

typedef struct
{
    s16 ax;
    s16 ay;
    s16 az;

    s16 mx;
    s16 my;
    s16 mz;

    s16 temp;

    s16 gx;
    s16 gy;
    s16 gz;

} MPU_OriginData;

typedef struct
{
    s16 ax;
    s16 ay;
    s16 az;

    s16 mx;
    s16 my;
    s16 mz;

    float temp;

    float wx; //omiga, +- 2000dps => +-32768  so gx/16.384/57.3 =	rad/s
    float wy;
    float wz;

    float rol;
    float pit;
    float yaw;
} imu_t;

u8                    mpu_device_init(void);
void                  mpu_get_data(void);
extern MPU_OriginData mpu_data;
extern imu_t          imu;

#endif
