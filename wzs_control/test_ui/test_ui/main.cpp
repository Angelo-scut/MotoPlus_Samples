#include "test_ui.h"
//#include "tcp_test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    test_ui w;
    //tcp_test w;
    w.show();
    return a.exec();
}
