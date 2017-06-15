#ifndef _BT_H_
#define _BT_H_

#include "stm32f4xx_hal.h"
#include	"mytype.h"
#include "usart.h"




enum{
    Head1 = 0,
    Head2,
    DataID,
    Byte0,
    Byte1,
    Byte2,
    Byte3,
    SumCheck,
    Tail,
    FrameLength,    //length = 9
};  //数据帧的字节顺序如data【head1】 = 0x55 

typedef union{
	u8 	U8type[4];
	s8	S8type[4];
	u16	U16type[2];
	s16 S16type[2];
	float	F32type;
}FloatConvertType; 
//可能需要注意注意大小端！

typedef __packed union{
	u8 	U8type[8];
//	s8	S8type[8];
	u16	U16type[4];
//	s16 S16type[4];
//	float	F32type[2];
}CurrentUnionType; 

 
typedef enum{

	RolKp = 0,
	RolKi,
	RolKd,

	PitKp,
	PitKi,
	PitKd,

	YawKp,
	YawKi,
	YawKd,

	RolOmgKp,
	RolOmgKi,
	RolOmgKd,

	PitOmgKp,
	PitOmgKi,
	PitOmgKd,

	YawOmgKp,
	YawOmgKi,
	YawOmgKd,

	ExpRol,
	ExpPit,
	ExpYaw,

	Throttle,
	WatchDog,
	DogBattary,

	RealRol,
	RealPit,
	RealYaw,

	GimbalYawKp,
	GimbalYawKi,
	GimbalYawKd,

	GimbalYawOmgKp,
	GimbalYawOmgKi,
	GimbalYawOmgKd,

	GimbalPitKp,
	GimbalPitKi,
	GimbalPitKd,

	GimbalPitOmgKp,
	GimbalPitOmgKi,
	GimbalPitOmgKd,

	GyroKp,
	GyroKi,
	GyroKd,

	Monitor1,
	Monitor2,
	Monitor3,
	Monitor4,
	Monitor5,
	Monitor6,
	Monitor7,
	Monitor8,

	SetupA,
	SetupB,
	SetupC,
	CmdJump2IAP,
	//for cmds
	CmdGyroNeedCalibrate,
	CmdReadAllFromMcu,  //PC发送出的该项值非0，则MCU检查该命令，使 MCU返回当前表中所有参数，
	CmdSaveAllToFlash,  //PC发送出的该项值非0，则MCU检查该命令，使 MCU保存当前表中所有参数到Flash
	CmdResetMCU, 				//这个保留
	CmdMcuGotParamOK,   //PC检查该项非0，则说明MCU成功收到并执行命令。
	//这四个参数用来表示PC对MCU的命令或者应答。

  RcTableLength,
}RcTableType;


#pragma pack()
typedef struct {
	int16_t ch1;	//each ch value from -364 -- +364
	int16_t ch2;
	int16_t ch3;
	int16_t ch4;
	
	uint8_t sw1;	//3 value
	uint8_t sw2;
	
	struct {
		int16_t x;
		int16_t y;
		int16_t z;//???不明觉厉
	
		uint8_t l;//左键 按下是1
		uint8_t r;//右键按下1 放开0
	}mouse;
	
	union {
		uint16_t key_code;
		struct {
			uint16_t W:1;
			uint16_t S:1;
			uint16_t A:1;
			uint16_t D:1;
			uint16_t SHIFT:1;
			uint16_t CTRL:1;
			uint16_t Q:1;
			uint16_t E:1;
			uint16_t R:1;
			uint16_t F:1;
			uint16_t G:1;
			uint16_t Z:1;
			uint16_t X:1;
			uint16_t C:1;
			uint16_t V:1;
			uint16_t B:1;
		}bit;
/**********************************************************************************
   * 键盘通道:15   14   13   12   11   10   9   8   7   6     5     4   3   2   1
   *          V    C    X	  Z    G    F    R   E   Q  CTRL  SHIFT  D   A   S   W
************************************************************************************/
	}kb;
	//uint16_t kb;
	
}RC_Type;

typedef  union {
	
		#pragma pack(1)		
		struct {
			signed long long ch1:11; 
			signed long long ch2:11;
			signed long long ch3:11;
			signed long long ch4:11;
			signed long long sl:2;
			signed long long sr:2;
			signed long long mx:16;
			//8 bytes
			
			int my:16;				//2b
			int mz:16;				//2b
			unsigned int mpl:8;//1
			unsigned int mpr:8;//1
			//4byte
			
			unsigned int W:1;
			unsigned S:1;
			unsigned A:1;
			unsigned D:1;
			unsigned SHIFT:1;
			unsigned CTRL:1;
			unsigned Q:1;
			unsigned E:1;
			unsigned R:1;
			unsigned F:1;
			unsigned G:1;
			unsigned Z:1;
			unsigned X:1;
			unsigned C:1;
			unsigned V:1;
			
			unsigned :1;	//reserved
			unsigned :16;	//reserved
		}bits;
		#pragma pack()

	uint8_t buff[18];

}RC_UnionDef;
//这样写也可以


enum{
	RC_UP = 1,
	RC_MI = 3,
	RC_DN = 2,
};

typedef struct __RC{

	int16_t		expYaw;	//yaw
	int16_t		throttle;	//throttle
	int16_t		expRoll;	//roll
	int16_t		expPitch;	//pit
	bool 			wdogAlive;	
	
	uint8_t 	switch1;
	uint8_t 	switch2;

}LANGO_RC_t;

typedef struct {
	u8 	target_num;	//1-9, 0 = fail
	u8  last_num;
	s16 yaw;	//relative position
	s16 pit; 
	s16 y5;
	s16 p5;
	s16 y9;
	s16 p9;
	s16 calied_y5;
	s16 calied_p5;
	s16 calied_y9;
	s16 calied_p9;
}CV_BigBuff_t;

#define  RX_BUFF_SIZE		(FrameLength*2)
#define DBUS_HUART							huart1
#define BT_HUART								huart2

extern RC_Type rc;
extern CV_BigBuff_t cv_big_buff;
//extern uint8_t bt_rxbuff[]; 
extern FloatConvertType   rcDataTable[RcTableLength]; 
void uart_send_frame(UART_HandleTypeDef *huart, RcTableType id);
void bt_printf(const char* fmt, ...);
void uart_send2pc(UART_HandleTypeDef *huart, RcTableType id, float value);
void uart_send4byte(UART_HandleTypeDef* huart, RcTableType id, void* buff);
void bt_init(void);
void dbus_init(void);
void manifold_uart_init(void);
void judgement_init(void);
void judge_sys_init(void);
//called in uart irq(it.c)
void LanggoUartFrameIRQHandler(UART_HandleTypeDef *huart);

#endif
