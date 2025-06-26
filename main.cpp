#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include "ViewLayer.h"
#include "ServiceLayer.h"
/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow main_window;
    main_window.setWindowTitle(QObject::tr("Habbit - 构建测试成功"));

    auto central_widget = std::make_unique<QWidget>();
    const auto layout = std::make_unique<QVBoxLayout>();
    const auto label = std::make_unique<QLabel>(QObject::tr("🎉 Qt 应用启动成功！"));

    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label.get());
    central_widget->setLayout(layout.get());

    main_window.setCentralWidget(central_widget.release());

    main_window.resize(400, 200);
    main_window.show();

    return QApplication::exec();
}
*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow main_window;
    main_window.setWindowTitle(QObject::tr("Habbit - 构建测试成功"));

    // 1. 创建 ServiceLayer 实例
    ServiceLayer serviceLayer;

    // 3. 测试插入一条数据
    /*
    using namespace std::chrono;
    serviceLayer.insertEvent(
        "测试事项 A",
        year_month_day{2025y, June, 30d},
        hh_mm_ss<seconds>(seconds(18 * 3600 + 30 * 60)), // 18:30:00
        true,
        hh_mm_ss<seconds>(seconds(3600))
        );
    */

    // 2. 创建 ViewLayer 实例，并传入 ServiceLayer
    ViewLayer *viewLayer = new ViewLayer(&main_window, serviceLayer);

    viewLayer->init(); // ← 添加这句来触发初始视图加载
    // 4. 初始化视图
    //viewLayer->initEventManageView();
    main_window.setCentralWidget(viewLayer);
    main_window.resize(1000, 800);
    main_window.show();

    return QApplication::exec();
}

