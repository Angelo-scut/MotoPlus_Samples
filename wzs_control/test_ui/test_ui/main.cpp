#include "test_ui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    test_ui w;
    w.show();
    return a.exec();
}
