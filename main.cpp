
#include "howlongtobeat.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HowLongTobeat w;
    w.show();
    return a.exec();
}
