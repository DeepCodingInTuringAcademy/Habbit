#include <QApplication>
#include <windows.h>
#include "CalendarWinWidget.h"

int main(int argc, char *argv[])
{
#ifdef _WIN32
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);  // 隐藏控制台
#endif

    QApplication a(argc, argv);
    CalendarWinWidget widget{nullptr, new ServiceLayer{}};
    widget.show();
    return a.exec();
}
