#include <QApplication>
#include "CalendarWinWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CalendarWinWidget widget;
    widget.show();
    return a.exec();
}
