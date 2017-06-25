

#include "mytype.h"

#define PARAM_SAVED_FLAG 0x5A //header of the structure
#define PARAM_CALI_DONE 0x5A
#define PARAM_CALI_NONE 0x00
#define CALIED_FLAG 0x55

//enum the cali result
typedef enum {
    CALI_STATE_ERR,
    CALI_STATE_ING,
    CALI_STATE_DONE,
} CALI_STATE_e;

typedef enum {
    CALI_GYRO,
    CALI_ACC,
    CALI_MAG,
    CALI_IMU_LIST_LEN,
    CALI_GIMBAL,
    //add more...
} CALI_ID_e;

typedef __packed struct
{
    int16_t GimbalYawOffset;
    int16_t GimbalPitOffset;
    uint8_t GimbalNeedCali;
    uint8_t isAlreadyCalied; //already calied
} GimbalCaliStruct_t;

typedef __packed struct
{
    int16_t offset[3]; //x,y,z
    uint8_t NeedCali; //1=need, 0=not
    uint8_t isAlreadyCalied; //0x55 = already calied, else not
    char*   name;
} ImuCaliStruct_t;

typedef __packed struct
{
    uint8_t            ParamSavedFlag; //header
    uint32_t           FirmwareVersion; //version
    GimbalCaliStruct_t GimbalCaliData; //gimbal pitch yaw encoder offset
    ImuCaliStruct_t    ImuCaliList[CALI_IMU_LIST_LEN]; // in fact =3
    GimbalCaliStruct_t CameraCali; //adjust yaw/pit make camera img move to center, record y/p
} AppParam_t;

extern AppParam_t gAppParam;
void              AppParamSave2Flash(void);
void              AppParamReadFromFlash(void);
void              gimbalCaliHook(void);
void imuCaliHook(CALI_ID_e cali_id, s16 raw_xyz[]);
void AppParamInit(void);
