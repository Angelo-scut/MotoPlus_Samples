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
#define COMMAND_LINE            5
#define COMMAND_LINE_VEC        6
#define COMMAND_LINE_MANUAL     7
#define COMMAND_CIRCLE_PC       8
#define COMMAND_CIRCLE_MANUAL   9

#define POS_VAR_LINE            10  // 走直线时的position variable， P0010
#define POS_VAR_LINE_VEC        11
#define POS_VAR_CIRCLE_PC       12


#define	CYCLETIME  	50			//[msec]
#define	RTC_TIME  	1			//[msec]
#define	MOVE_DISTANCE  	50000	//[micro meter] 微米，毫米是millimeter
#define	PULSE_TIME  20			//[msec]

// #define	ON  	1
// #define	OFF  	0

#define SAFE_DIST   10000       // [micro meter]
#define SAFE_ANGL   100000

#endif