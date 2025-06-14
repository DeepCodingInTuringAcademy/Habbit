#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>


int main(int argc, char *argv[]) {
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