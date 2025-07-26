#include <QApplication>
#include "CalendarWinWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CalendarWinWidget widget{nullptr, new ServiceLayer{}};
    widget.show();
    return a.exec();
}
