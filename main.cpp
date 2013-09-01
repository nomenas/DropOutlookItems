#include "dropoutlookitem.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DropOutlookItem w;
    w.show();
    return a.exec();
}
