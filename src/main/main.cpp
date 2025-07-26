#include<QApplication>

#include "SimpleAuthSystem.h"
#include "ViewLayer.h"

// 临时测试函数

// 测试不同界面
int testHabbitModule(int argc, char* argv[], const ViewLayer::ViewType type)
{
    QApplication app(argc, argv);

    ViewLayer view;
    view.setFixedSize(1000, 800);
    view.setWindowTitle("Habbit 效率管理软件");
    view.setCurrentView(type);
    view.show();

    return app.exec();
}

int testHabbitModule(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SimpleAuthSystem authSystem;
    authSystem.show();

    return app.exec();
}

// 其他部分的临时测试函数在本行上方添加

int main(const int argc, char *argv[])
{
     return testHabbitModule(argc, argv, ViewLayer::ViewType::MAIN_VIEW);
}