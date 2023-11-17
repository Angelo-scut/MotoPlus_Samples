
#include "yrc_control.hpp"
#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <cmath>

namespace yrc{
    YRC_control::YRC_control(const string& ip, int port){
		client = new QTcpSocket(this);
		QObject::connect(client, &QTcpSocket::connected, this, &YRC_control::tcp_connect_event);
        // QObject::connect(client, &QTcpSocket::readyRead, this, &YRC_control::tcp_receive_event);  // 因为要同步，所以要手动读，不能自动读
        QObject::connect(client, &QTcpSocket::disconnected, this, &YRC_control::tcp_disconnect_event);
       // yrc_connect(ip, port);
        pos = std::vector<float>(6, 0);
        tool_data = std::vector<float>(6, 0);
        speed = get_speed();
        get_position_();
        sendGetToolDataCommand();
    }

    YRC_control::~YRC_control(){
        client->close();
        is_tcp_connect = false;
        pos.clear();
        tool_data.clear();
    }

    void YRC_control::yrc_connect(const string& ip, int port){
        client->connectToHost(QString::fromStdString(ip), port);  // 一开始不能直接connect
    }

    void YRC_control::fast_locate_pc(const vector<float>& pos){
        if (pos.size() != 6)    return;
        send(COMMAND_FAST_LOC_PC, pos);
    }

    void YRC_control::line_pc(const vector<float>& pos){
        if (pos.size() != 6)    return;
        send(COMMAND_LINE_PC, pos);
    }

    void YRC_control::circle_pc(const vector<float>& p1, 
                                const vector<float>& p2, const vector<float>& p3){
        
    }

    void YRC_control::corr_path(float val, int axis){
        if(axis < 0 || axis > 5) return;
        vector<float> pos(6, 0.f);
        pos[axis] = val;
        send(COMMAND_CORRPATH, pos);
    }

    void YRC_control::corr_path(const vector<float>& pos){
        if (pos.size() != 6)    return;
        send(COMMAND_CORRPATH, pos);
    }

    vector<float> YRC_control::get_position(){
        get_position_();
        if(client->waitForReadyRead(100)){  // 等待50ms
            tcp_receive_event();
            return pos;
        }
        else{
            return vector<float>(6, 0.);  // 若没有接收到数据，则返回空
        }
    }

    void YRC_control::sendGetToolDataCommand() {
        if (is_tcp_connect) {
            tcp_send_event("GET_TOOL_DATA");
        }
    }
	vector<float> YRC_control::get_tooldata() {
		//get_tooldata();
        if (client->waitForReadyRead()) {
            tcp_receive_event();
            return tool_data;
        }
		else {
			return vector<float>(6, 0.); // 若没有接收到数据，则返回空
		}
	}
    cv::Mat YRC_control::get_tcpMatrix() {
        //this->tcp_send_event("GET_TOOL_DATA");
        //if (this->tcp_receive_event()) {
        get_tooldata();
        float rx_deg, ry_deg, rz_deg;
        float rx_rad, ry_rad, rz_rad;
        float x, y, z;

        rx_deg = tool_data[3];
        ry_deg = tool_data[4];
        rz_deg = tool_data[5];

        rx_rad = this->degreesToRadians(rx_deg);
        ry_rad = this->degreesToRadians(ry_deg);
        rz_rad = this->degreesToRadians(rz_deg);

        x = tool_data[0];
        y = tool_data[1];
        z = tool_data[2];
        cv::Mat rotationMatrix = this->calculateRotationMatrix(rx_rad, ry_rad, rz_rad);
        cv::Mat tcpMatrix = cv::Mat::eye(4, 4, CV_32F);
        rotationMatrix.copyTo(tcpMatrix(cv::Rect(0, 0, 3, 3)));

        tcpMatrix.at<float>(0, 3) = x;
        tcpMatrix.at<float>(1, 3) = y;
        tcpMatrix.at<float>(2, 3) = z;

        return tcpMatrix;

    }

    cv::Mat YRC_control::calculateRotationMatrix(float rx, float ry, float rz) {
        cv::Mat rotationX = cv::Mat::eye(3, 3, CV_32F);
        cv::Mat rotationY = cv::Mat::eye(3, 3, CV_32F);
        cv::Mat rotationZ = cv::Mat::eye(3, 3, CV_32F);

        rotationX.at<float>(1, 1) = cos(rx);
        rotationX.at<float>(1, 2) = -sin(rx);  
        rotationX.at<float>(2, 1) = sin(rx);
        rotationX.at<float>(2, 2) = cos(rx);

        rotationY.at<float>(0, 0) = cos(ry);
        rotationY.at<float>(0, 2) = sin(ry);
        rotationY.at<float>(2, 0) = -sin(ry);
        rotationY.at<float>(2, 2) = cos(ry);

        rotationZ.at<float>(0, 0) = cos(rz);
        rotationZ.at<float>(0, 1) = -sin(rz);
        rotationZ.at<float>(1, 0) = sin(rz);
        rotationZ.at<float>(1, 1) = cos(rz);
        cv::Mat result = rotationZ * rotationY * rotationX;
        return result;
    }
    float YRC_control::degreesToRadians(float degrees) {
        return degrees * M_PI / 180.0;
    }

    const float YRC_control::get_speed(){
        send(COMMAND_UNKNOW);
        if(client->waitForReadyRead(50)){
            tcp_receive_event();
            return speed;
        }
        else{
            return -1;  // 若没有接收到数据，则返回-1
        }
    }

    void YRC_control::decoding(string& msg, vector<float>& pos, float& spd, vector<float>& tool_data) {
        long data[13];  // 6轴+工具数据+速度
        memset(&data[0], 0, sizeof(long) * 13);

        char* msg_c = (char*)msg.c_str();
        char* token = strtok(msg_c, ",");
        int idx = 0;
        while (token != NULL && idx < 13)
        {
            data[idx++] = atoi(token);
            token = strtok(NULL, ",");
        }

        if (token != NULL || idx != 13) return;

        int i = 0;
        for (; i < 3; i++)
        {
            pos[i] = (float)data[i] / POSITION_FACTOR;
        }
        for (; i < 6; i++) {
            pos[i] = (float)data[i] / ANGLE_FACTOR;
        }
        for (; i < 9; i++)
        {
            tool_data[i - 6] = (float)data[i] / POSITION_FACTOR;
        }
        for (; i < 12; i++) {
            tool_data[i - 6] = (float)data[i] / ANGLE_FACTOR;
        }
        spd = (float)data[i] / SPEED_FACTOR;  // 这个是真实的速度




    }
    void YRC_control::encoding(string& msg, int cmd, const vector<float>& pos) {
        msg.clear();
        msg = std::to_string(cmd) + ":";  // command

        if (pos.size() == 1)  // speed_on
        {
            msg += std::to_string(int(pos[0] * SENSOR_SPEED_FACTOR));  // 这里只是0%-150%的相对速度，这里已经将1.0转化成100% * 100了
            return;
        }

        if (pos.size() == 6)  // motion
        {
            int i = 0;
            for (; (i < 3) && (i < pos.size()); i++)
            {
                msg += std::to_string(int(pos[i] * POSITION_FACTOR));
                if (i != pos.size() - 1)
                {
                    msg += ",";
                }
            }
            for (; (i < 6) && (i < pos.size()); i++)
            {
                msg += std::to_string(int(pos[i] * ANGLE_FACTOR));
                if (i != pos.size() - 1)
                {
                    msg += ",";
                }
            }
        }

    }



    void YRC_control::tcp_receive_event() {
            QByteArray buff = client->readAll();
            std::string msg = buff.toStdString();
            decoding(msg, pos, speed, tool_data);
    }
    void YRC_control::send(int cmd, const vector<float>& pos){
        string msg;
        encoding(msg, cmd, pos);
        tcp_send_event(msg);
    }

    void YRC_control::tcp_send_event(const string& msg){
        if (is_tcp_connect)
        {
            client->write(QString::fromStdString(msg).toUtf8().data());
        }
        
    }

}
