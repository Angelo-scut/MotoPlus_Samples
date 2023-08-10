
#include "yrc_control.hpp"
#include <cstring>
#include <stdlib.h>

namespace yrc{
    YRC_control::YRC_control(const string& ip, int port){
		client = new QTcpSocket(this);
		QObject::connect(client, &QTcpSocket::connected, this, &YRC_control::tcp_connect_event);
        // QObject::connect(client, &QTcpSocket::readyRead, this, &YRC_control::tcp_receive_event);  // 因为要同步，所以要手动读，不能自动读
        QObject::connect(client, &QTcpSocket::disconnected, this, &YRC_control::tcp_disconnect_event);
        connect(ip, port);
        pos = std::vector<float>(6, 0);
        speed = get_speed();
        get_position_();
    }

    YRC_control::~YRC_control(){
        client->close();
        is_tcp_connect = false;
        pos.clear();
    }

    void YRC_control::connect(const string& ip, int port){
        client->connectToHost(QString::fromStdString(ip), port);  // TODO:如何直接等待接收呢？
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
        if(client->waitForReadyRead(50)){  // 等待50ms
            tcp_receive_event();
            return pos;
        }
        else{
            return vector<float>();  // 若没有接收到数据，则返回空
        }
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

    void YRC_control::decoding(string& msg, vector<float>& pos, float& spd){
        long data[7];  // 6轴+速度
        memset(&data[0], 0, sizeof(long) * 7);
        
        char* msg_c = (char*)msg.c_str();
        char* token = strtok(msg_c, ",");
        int idx = 0;
        while (token != NULL && idx < 7)
        {
            data[idx++] = atoi(token);
            token = strtok(NULL, ",");
        }

        if(token != NULL || idx != 7) return;

        int i = 0;
        for (; i < 3; i++)
        {
            pos[i] = (float)data[i] / POSITION_FACTOR;
        }
        for(; i<6; i++){
            pos[i] = (float)data[i] / ANGLE_FACTOR;
        }

        spd = (float)data[i] / SPEED_FACTOR;  // 这个是真实的速度
        
    }

    void YRC_control::encoding(string& msg, int cmd, const vector<float>& pos){
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

    void YRC_control::tcp_receive_event(){
        QByteArray buff = client->readAll();
        string msg = buff.toStdString();
        decoding(msg, pos, speed);
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
