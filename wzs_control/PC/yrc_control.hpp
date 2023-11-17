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

#include <QtWidgets/QMainWindow>  // TODO:还要写一版不依赖Qt的，主要是TCP
#include <QtNetwork/qtcpsocket.h>
#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp> 

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

#define POSITION_FACTOR         1000
#define ANGLE_FACTOR            10000
#define SENSOR_SPEED_FACTOR     10000
#define SPEED_FACTOR            10

#define BUFF_LEN                1024
#define M_PI 3.14159265358979323846

namespace yrc{
    using namespace std;

    class YRC_control : public QWidget{
	public:
		YRC_control(const YRC_control& yrc_ctrl)=default;
        YRC_control(const string& ip="192.168.255.1", int port=11000);
        ~YRC_control();

        void yrc_connect(const string& ip="192.168.255.1", int port=11000);
        
        /* Manual Function */
        void fast_locate_manual(){ send(COMMAND_FAST_LOC_MUNAL); }
        void line_manual(){ send(COMMAND_LINE_MANUAL); }
        void circle_manual(){ send(COMMAND_CIRCLE_MANUAL); }

        /* Remote Function */
        void fast_locate_pc(const vector<float>& pos);        
        void line_pc(const vector<float>& pos);
        void circle_pc(const vector<float>& p1, 
                        const vector<float>& p2, 
                        const vector<float>& p3);
        
        /* Sensor Function */
        void change_speed(float spd_ftr){ send(COMMAND_SPD_OVR_ON, { spd_ftr }); }
        void restore_speed(){ send(COMMAND_SPD_OVR_OFF); }
        void force_path_end(){ send(COMMAND_FRCPATH_END); }
        void corr_path(float val, int axis);
        void corr_path(const vector<float>& pos);

        void job_exit() { send(COMMAND_EXIT); }
        void sever_exit(){ tcp_send_event("EXIT"); }

        vector<float> get_position();
        const float get_speed();
        vector<float> get_tooldata();
        void sendGetToolDataCommand();
        cv::Mat get_tcpMatrix();
        bool is_tcp_connect;

    private:
        static void decoding(string& msg, vector<float>& pos, float& spd, vector<float>& tool_data);
        static void encoding(string& msg, int cmd, const vector<float>& pos = vector<float>());
        void send(int cmd, const vector<float>& pos=vector<float>());
        void get_position_() { send(COMMAND_UNKNOW); };
        //cv::Mat get_tcpMatrix(const vector<double>& tool_data);
        cv::Mat calculateRotationMatrix(float rx, float ry, float rz);
        float degreesToRadians(float degrees);


        void tcp_send_event(const string& msg);
		void tcp_receive_event();
		/* Qt signal and slot */
		void tcp_connect_event() { is_tcp_connect = true; }
		void tcp_disconnect_event() { is_tcp_connect = false; }
		
        float speed;
        vector<float> pos;
        vector<float> tool_data;
        QTcpSocket *client;
        //bool is_tcp_connect;
    };

}

#endif  // YRC_CONTROL_HPP