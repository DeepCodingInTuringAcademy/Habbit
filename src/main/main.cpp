#include<QApplication>
#include <windows.h>
#include "ViewLayer.h"


int main(int argc, char* argv[])
{
#ifdef _WIN32
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);  // 隐藏控制台
#endif
    QApplication app(argc, argv);

    ViewLayer view;
    view.setFixedSize(1100, 800);
    view.setWindowTitle("Habbit 效率管理软件");
    view.show();

    return QApplication::exec();
}