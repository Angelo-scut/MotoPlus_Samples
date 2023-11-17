
#include "tcp_test.h"
#include <string>

using namespace std;
tcp_test::tcp_test(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::tcp_testClass)
{
    ui->setupUi(this);
    //tcpSocket = new QTcpSocket(this);
    init_widget();
    vector<double> tool_data(6, 1); 
    controller = make_shared<YRC_control>();

    ui->ipEdit->setText("192.168.255.1");
    ui->portEdit->setText("11000");
    //ui->ipEdit->setText("172.28.6.44");
    //ui->portEdit->setText("8080");
    

    connect(ui->connectBtn, &QPushButton::clicked, this, &tcp_test::connect_button_event);
    connect(ui->get_tool_dataBtn, &QPushButton::clicked, this, &tcp_test::get_tool_data_button_event);
    
}

tcp_test::~tcp_test()
{
    delete ui;

}

void tcp_test::init_widget(bool state)
{
    ui->get_tool_dataBtn->setEnabled(state);

}
void tcp_test::connect_button_event()
{
    string ip = ui->ipEdit->text().toStdString();
    int port = ui->portEdit->text().toInt();
    controller->yrc_connect(ip, port); 
    bool isConnected = controller->is_tcp_connect;
    init_widget(isConnected); 
}

void tcp_test::get_tool_data_button_event() {
	//if (controller->is_tcp_connect) {
		controller->sendGetToolDataCommand();
		tcpMatrix = controller->get_tcpMatrix();
		QString matrixString;
        matrixString = matToString(tcpMatrix);
		ui->textEdit->setText(matrixString);
	//}
}

QString tcp_test::matToString(const cv::Mat& mat) {
    QString result;
    // ����С��������ʾ����
    int precision = 4; // С������λ��
    int fieldWidth = 10; // ÿ������Ԫ�ص���ռλ��ȣ�����С����Ϳ��ܵĸ���

    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // ʹ��QString��sprintf��asprintf����ʽ��������
            result += QString("%1").arg(QString::number(mat.at<float>(i, j), 'f', precision), fieldWidth);
            if (j < mat.cols - 1) {
                // �������ÿ�е����һ��Ԫ�أ���ӿո��Էָ���
                result += " ";
            }
        }
        // ��ÿ�е�ĩβ��ӻ��з�
        result += "\n";
    }
    return result;
}
//QString tcp_test::matToString(const cv::Mat& mat) {
//    QString result;
//    for (int i = 0; i < mat.rows; ++i) {
//        for (int j = 0; j < mat.cols; ++j) {
//            result += QString::number(mat.at<float>(i, j)) + "\t";
//        }
//        result += "\n";
//    }
//    return result;
//}
//QString tcp_test::floatVectorToQString(const std::vector<float>& vec) {
//    QString result;
//    for (size_t i = 0; i < vec.size(); ++i) {
//        result += QString::number(vec[i], 'f', 2);  // 'f' ��ʾ�����ʽ��2 ��ʾ������λС��
//        if (i < vec.size() - 1) {
//            result += ", ";  // ��ӷָ������������һ��Ԫ��
//        }
//    }
//    return result;
//}