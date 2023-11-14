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

#include <QtWidgets/QMainWindow>  // TODO:��Ҫдһ�治����Qt�ģ���Ҫ��TCP
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

        void connect(const string& ip="192.168.255.1", int port=11000);
        
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

    private:
        static void decoding(string& msg, vector<float>& pos, float& spd, vector<double>& tool_data);
        static void decodePosAndSpeed(const std::string& dataStr, vector<float>& pos, float& spd);
        static void decodeToolData(const std::string& dataStr, vector<double>& tool_data);
        static void encoding(string& msg, int cmd, const vector<float>& pos=vector<float>());
        void send(int cmd, const vector<float>& pos=vector<float>());
        void get_position_() { send(COMMAND_UNKNOW); }
        cv::Mat get_tcpMatrix();
        cv::Mat calculateRotationMatrix(double rx, double ry, double rz);
        double degreesToRadians(double degrees);

        void tcp_send_event(const string& msg);
		bool tcp_receive_event(int msec=3000);
		/* Qt signal and slot */
		void tcp_connect_event() { is_tcp_connect = true; }
		void tcp_disconnect_event() { is_tcp_connect = false; }
		
        float speed;
        vector<float> pos;
        vector<double> tool_data;
        QTcpSocket *client;
        bool is_tcp_connect;
    };

}

#endif  // YRC_CONTROL_HPP