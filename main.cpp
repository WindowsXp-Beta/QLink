#include "startwindow.h"
#include "qlinktest.h"

#include <QApplication>

int main(int argc, char *argv[])
{
//    QTEST_MAIN_IMPL(QLinkTest);

    QApplication a(argc, argv);
    StartWindow w;
    w.show();
    return a.exec();
}
