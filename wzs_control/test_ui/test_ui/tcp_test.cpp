
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
    // 设置小数点后的显示精度
    int precision = 4; // 小数点后的位数
    int fieldWidth = 10; // 每个矩阵元素的总占位宽度，包括小数点和可能的负号

    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // 使用QString的sprintf或asprintf来格式化浮点数
            result += QString("%1").arg(QString::number(mat.at<float>(i, j), 'f', precision), fieldWidth);
            if (j < mat.cols - 1) {
                // 如果不是每行的最后一个元素，添加空格以分隔列
                result += " ";
            }
        }
        // 在每行的末尾添加换行符
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
//        result += QString::number(vec[i], 'f', 2);  // 'f' 表示定点格式，2 表示保留两位小数
//        if (i < vec.size() - 1) {
//            result += ", ";  // 添加分隔符，除了最后一个元素
//        }
//    }
//    return result;
//}