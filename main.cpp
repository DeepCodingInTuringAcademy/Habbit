#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QObject::tr("Habbit - 构建测试成功"));

    auto centralWidget = std::make_unique<QWidget>();
    const auto layout = std::make_unique<QVBoxLayout>();
    const auto label = std::make_unique<QLabel>(QObject::tr("🎉 Qt 应用启动成功！"));

    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label.get());
    centralWidget->setLayout(layout.get());

    mainWindow.setCentralWidget(centralWidget.release());

    mainWindow.resize(400, 200);
    mainWindow.show();

    return QApplication::exec();
}