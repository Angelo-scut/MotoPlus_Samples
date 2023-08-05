/**
 * @file yrc_control.hpp
 * @author Zishun Wang
 * @brief The Interface class and Implimentation class of YRC robot control
 * @version 0.1
 * @date 2023-08-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef YRC_CONTROL_HPP
#define YRC_CONTROL_HPP

#include <QtNetwork/qtcpsocket.h>
#include <string>
#include <memory>
#include <vector>

#define COMMAND_UNKNOW          0
#define COMMAND_CORRPATH        1
#define COMMAND_SPD_OVR_ON      2
#define COMMAND_SPD_OVR_OFF     3
#define COMMAND_FRCPATH_END     4
#define COMMAND_LINE_PC         5
// #define COMMAND_LINE_VEC        6
#define COMMAND_LINE_MANUAL     7
#define COMMAND_CIRCLE_PC       8
#define COMMAND_CIRCLE_MANUAL   9
#define COMMAND_FAST_LOC_PC     10
#define COMMAND_FAST_LOC_MUNAL  11
#define COMMAND_EXIT            12

namespace yrc{
    using std::string, std::vector;

    class YRC_control{
        YRC_control();
        bool connect(const string& ip="192.168.255.1", int port=11000);
        
        /* Manual Function */
        void fast_locate_manual();
        void line_manual();
        void circle_manual();

        /* Remote Function */
        void fast_locate_pc(const float pos[6]);        
        void line_pc(const float pos[6]);
        void circle_pc(const float pos[3][6]);
        
        /* Sensor Function */
        void change_speed(float spd);
        void restore_speed();
        void force_path_end();
        void rectify_path(float val, int axis);
        void rectify_path(const float pos[6]);

        void job_exit();
        void sever_exit();

        const vector<int> get_position();
        const float get_speed();
    private:
        static void decoding(const char* mesg);
        static void encoding(const float data[6]);
        float speed;
        float position[6];
        QTcpSocket *client;
    };

}

#endif  // YRC_CONTROL_HPP