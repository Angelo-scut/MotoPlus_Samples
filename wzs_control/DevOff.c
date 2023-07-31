/**
 * @file DevOff.c
 * @author Zishun Wang
 * @brief Deviation offset control
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "MacroDefine.h"

extern int command_no;
extern long speed;
extern long offset[6];
extern BOOL sensor_flag;

void offsControlTask(void){
    int ret = 0;

    MP_POS_DATA corrpath_src_p;
    long spd_src_p[MP_GRP_NUM];
    CTRLG_T ctrl_grp = 1;  // 指定第一个机器人

    memset(&corrpath_src_p, CLEAR, sizeof(MP_POS_DATA));
    memset(&spd_src_p, CLEAR, sizeof(long) * MP_GRP_NUM);

    corrpath_src_p.ctrl_grp = 1;
	corrpath_src_p.grp_pos_info[0].pos_tag.data[0] = 0x3f;  // 0x0011 1111  //只控制六个轴1-6
	corrpath_src_p.grp_pos_info[0].pos_tag.data[3] = MP_CORR_BF_DTYPE;  // 基于基座标
	corrpath_src_p.grp_pos_info[0].pos[0] = 0; 
	corrpath_src_p.grp_pos_info[0].pos[1] = 0; 
	corrpath_src_p.grp_pos_info[0].pos[2] = 0;
	corrpath_src_p.grp_pos_info[0].pos[3] = 0;
	corrpath_src_p.grp_pos_info[0].pos[4] = 0;
	corrpath_src_p.grp_pos_info[0].pos[5] = 0;
		
	spd_src_p[0] = 0;  // 单位是0.01%，比如速度设置为原本的150%->150 00，有效值为1000-15000

    FOREVER
    {
        // 由于优先级比较高，因此不能有其他的等待函数，否则可能会导致系统报警
        mpClkAnnounce(MP_INTERPOLATION_CLK);  // 每一个插值时钟运行一次，因为要严格控制节奏，所以耗时的程序不能在这个task中执行
        if(sensor_flag){
            switch (command_no)
            {
            case COMMAND_UNKNOW:
                break;
            case COMMAND_CORRPATH:
                memcpy(&corrpath_src_p.grp_pos_info[0].pos[0], &offset[0], sizeof(long) * 6);
                ret = mpMeiPutCorrPath(MP_SL_ID1, &corrpath_src_p);  // 如果确保偏差补偿成功呢？
                if (ret != 0)
                {
                    //TODO:返回错误信息
                }
                command_no = COMMAND_UNKNOW;  // 所有都要清除命令一次
                break;
            case COMMAND_FRCPATH_END:
                ret = mpMeiPutForcePathEnd(MP_SL_ID1, ctrl_grp);
                if (ret != 0)
                {
                    //TODO:返回错误信息
                }
                
                command_no = COMMAND_UNKNOW;
                break;
            case COMMAND_SPD_OVR_ON:  // 如何获得具体的速度呢？没有直接获取速度的API，只有获取每个关节转速的mpGetFBSpeed,还要通过运动学才能转成终端运行速度，，
                spd_src_p[0] = speed;
                ret = mpMeiPutSpdOverride(MP_SL_ID1, ctrl_grp, spd_src_p);
                if (ret != 0)
                {
                    //TODO:返回错误信息
                }
                
                command_no = COMMAND_UNKNOW;
                break;
            case COMMAND_SPD_OVR_OFF:
                ret = mpMeiPutSpdOverride(MP_SL_ID1, OFF, NULL);
                if (ret != 0)
                {
                    //TODO:返回错误信息
                }
                
                command_no = COMMAND_UNKNOW;
                break;
            default:
                break;
            }
            sensor_flag = FALSE;
        }
            
    }

}
