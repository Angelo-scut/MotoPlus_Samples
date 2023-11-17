#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_tcp_test.h"
#include "yrc_control.hpp"
#include <QtNetwork/QTcpSocket>
#include <vector>
#include <QString>

using namespace yrc;
class tcp_test : public QMainWindow
{
	Q_OBJECT

public:
	tcp_test(QWidget *parent = Q_NULLPTR);
	~tcp_test();
	void init_widget(bool state = false);

private:
	void connect_button_event();
	void get_tool_data_button_event();
	//std::string tcp_test::tcp_data_encoding();
	QString floatVectorToQString(const std::vector<float>& vec);
	QString matToString(const cv::Mat& mat);
	vector<float> tool_data;
	cv::Mat tcpMatrix;
	//void readTcpData();
	Ui::tcp_testClass* ui;
	//QTcpSocket* tcpSocket;
	std::shared_ptr<yrc::YRC_control> controller;

};
