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
#define COMMAND_LINE_PC         5
#define COMMAND_LINE_VEC        6
#define COMMAND_LINE_MANUAL     7
#define COMMAND_CIRCLE_PC       8
#define COMMAND_CIRCLE_MANUAL   9
#define COMMAND_FAST_LOC_PC     10
#define COMMAND_FAST_LOC_MUNAL  11

#define POS_VAR_LINE            10  // 走直线时的position variable， P0010
#define POS_VAR_FAST_LOC        11  // 感觉向量不应该由底层来封装，而是由上位机来封装
#define POS_VAR_CIRCLE_PC       12

#define SPEED_INTEGER           0  // 单位是0.1mm/s

/* 功能选择的用户变量B */
#define B_VAR_LINE              1
#define B_VAR_LINE_MANUAL       2
#define B_VAR_CIRCLE_PC         3
#define B_VAR_CIRCLE_MANUAL     4
#define B_VAR_FAST_LOC_PC       5
#define B_VAR_FAST_LOC_MUNAL    6

/* 中断继电器 */
#define OUT_RELAY_LINE          10020
#define OUT_RELAY_LINE_MANUAL   10021
#define OUT_RELAY_FAST_LOC_PC   10022
#define OUT_RELAY_FAST_LOC_MUNAL 10023
#define OUT_RELAY_CIRCLE_PC     10024
#define OUT_RELAY_CIRCLE_MANUAL 10025

#define	CYCLETIME  	            50			//[msec]
#define	RTC_TIME  	            1			//[msec]
#define	MOVE_DISTANCE  	        50000	//[micro meter] 微米，毫米是millimeter
#define	PULSE_TIME              20			//[msec]

// #define	ON  	1
// #define	OFF  	0

#define SAFE_DIST               300000       // [micro meter]
#define SAFE_ANGL               100000

#endif