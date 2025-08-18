#include <QApplication>
#include "ViewLayer.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    ViewLayer view;
    view.setFixedSize(1100, 800);
    view.setWindowTitle("Habbit 效率管理软件");
    view.show();

    return QApplication::exec();
}