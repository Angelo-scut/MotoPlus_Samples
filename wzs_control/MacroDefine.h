#ifndef MACRODEFINE_H
#define MACRODEFINE_H

#include "motoPlus.h"

#define PORT        11000  // 端口号
#define BUFF_MAX    1023  // 接收数据的BUFF

#define COMMAND_UNKNOW          0
#define COMMAND_CORRPATH        1
#define COMMAND_SPD_OVR_ON      2
#define COMMAND_SPD_OVR_OFF     3
#define COMMAND_FRCPATH_END     4

#define	CYCLETIME  	50			//[msec]
#define	RTC_TIME  	1			//[msec]
#define	MOVE_DISTANCE  	50000	//[micro meter] 微米，毫米是millimeter
#define	PULSE_TIME  20			//[msec]

// #define	ON  	1
// #define	OFF  	0

#define SAFE_DIST   10000       // [micro meter]
#define SAFE_ANGL   100000

#endif