#include "ViewLayer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ViewLayer view;
    view.resize(800, 600);
    view.setWindowTitle("Habbit 效率管理软件");
    view.setCurrentView(ViewLayer::ViewType::HABIT_MANAGE_VIEW);
    view.show();

    return app.exec();
}