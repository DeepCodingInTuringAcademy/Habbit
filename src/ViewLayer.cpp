#include "ViewLayer.h"
#include "Event.h"
#include "Times.h"
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QPushButton>

// ViewLayer::ViewLayer(QWidget *parent) {

// }

ViewLayer::ViewLayer(QWidget *parent, ServiceLayer& service)
    : QWidget(parent), sv_Layer(service) {
    initEventManageView();
}

void ViewLayer::init() {
    cur_view_type = ViewType::EVENT_MANAGE_VIEW; // 默认先展示事项视图
    initEventManageView();
}

bool ViewLayer::parseTime(const std::string &str, Time &result) {
    return false;
}

void ViewLayer::showView(QWidget *view) {

}

bool ViewLayer::parseDate(const std::string &str, Date &result) {
    return false;
}

void ViewLayer::clearLayout(QLayout *layout) {

}

QDateTime ViewLayer::chronoToQDateTime(const std::chrono::year_month_day& date, const std::chrono::hh_mm_ss<std::chrono::seconds>& time) {
    int year = int(date.year());
    unsigned int month = unsigned(date.month());
    unsigned int day = unsigned(date.day());

    int hour = time.hours().count();
    int minute = time.minutes().count();
    int second = time.seconds().count();

    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}

void ViewLayer::initEventManageView() {

    if (event_manage_widget) delete event_manage_widget;
    event_manage_widget = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(event_manage_widget);

    // 临时代码：模拟测试用事件数据
    std::vector<Event> testEvents = {
        Event(
            1,                      // event_id
            0,                      // user_id
            "测试事项 A",            // title
            Date{std::chrono::year{2025}, std::chrono::month{6}, std::chrono::day{30}},   // event_date
            Time{std::chrono::seconds(36000)},     // event_time (10:00:00)
            true,                   // remind_flag
            Time{std::chrono::seconds(1800)},      // remind_time (30分钟)
            false,                  // is_expired_flag
            false                   // is_deleted
            )
    };

    std::vector<Event> activeEvents = testEvents;
    std::vector<Event> expiredEvents = {}; // 这里我们假设没有过期事项
    auto curDate = Date{std::chrono::year{2025}, std::chrono::month{6}, std::chrono::day{25}};
    auto curTime = Time{std::chrono::seconds(9 * 3600 + 30 * 60)}; // 9:30:00


    // 1. 获取数据
    // std::vector<Event> activeEvents = sv_Layer.getActiveEvents();
    // std::vector<Event> expiredEvents = sv_Layer.getExpiredEvents();
    // auto [curDate, curTime] = sv_Layer.getCurrentTimeStamp();


    // 2. 活跃事项部分标题
    QLabel *activeLabel = new QLabel("活跃中", this);
    activeLabel->setStyleSheet("font-size: 24px; color: black;");
    layout->addWidget(activeLabel);

    // 3. 活跃事项网格布局
    QGridLayout *activeGrid = new QGridLayout;
    int eventWidth = 300, eventHeight = 250;
    int row = 0, col = 0;
    int eventsPerRow = 3;

    for (size_t i = 0; i < activeEvents.size(); ++i) {
        const Event &e = activeEvents[i];

        QGroupBox *eventBox = new QGroupBox(QString::fromStdString(e.getTitle()));
        eventBox->setMinimumSize(eventWidth, eventHeight);
        eventBox->setStyleSheet("QGroupBox { background-color: rgb(255,255,168); color: rgb(237,137,204); font-size: 16px; }");

        QVBoxLayout *boxLayout = new QVBoxLayout(eventBox);

        // 剩余时间计算
        const auto& date = e.getevent_date();
        const auto& time = e.getevent_time();
        QDateTime dueDT(
            QDate(
                static_cast<int>(date.year()),
                static_cast<unsigned int>(date.month()),
                static_cast<unsigned int>(date.day())
                ),
            QTime(
                time.hours().count(),
                time.minutes().count(),
                time.seconds().count()
                )
            );
        QDateTime currentDT = chronoToQDateTime(curDate, curTime);

        qint64 secsRemaining = currentDT.secsTo(dueDT);

        QString remainingText;
        if (secsRemaining >= 86400)
            remainingText = QString("剩余: %1天").arg(secsRemaining / 86400);
        else if (secsRemaining >= 3600)
            remainingText = QString("剩余: %1小时").arg(secsRemaining / 3600);
        else
            remainingText = QString("剩余: %1分钟").arg(secsRemaining / 60);

        boxLayout->addWidget(new QLabel(remainingText));

        // 按钮区
        QHBoxLayout *btnLayout = new QHBoxLayout;

        QPushButton *modifyBtn = new QPushButton("修改");
        modifyBtn->setStyleSheet("background-color: rgb(154,255,154); color: rgb(237,137,204);");
        connect(modifyBtn, &QPushButton::clicked, [=]() {
            this->eventModified(e); // Qt 内建议复制后操作，或存ID重新取
        });

        QPushButton *deleteBtn = new QPushButton("删除");
        deleteBtn->setStyleSheet("background-color: rgb(154,255,154); color: rgb(237,137,204);");
        connect(deleteBtn, &QPushButton::clicked, [=]() {
            this->eventDeleted(e);
        });

        btnLayout->addWidget(modifyBtn);
        btnLayout->addWidget(deleteBtn);
        boxLayout->addLayout(btnLayout);

        activeGrid->addWidget(eventBox, row, col);
        col = (col + 1) % eventsPerRow;
        if (col == 0) ++row;
    }
    layout->addLayout(activeGrid);

    // 4. 过期事项部分标题
    QLabel *expiredLabel = new QLabel("已过期", this);
    expiredLabel->setStyleSheet("font-size: 24px; color: black;");
    layout->addWidget(expiredLabel);

    // 5. 过期事项网格布局
    QGridLayout *expiredGrid = new QGridLayout;
    row = col = 0;
    for (size_t i = 0; i < expiredEvents.size(); ++i) {
        const Event &e = expiredEvents[i];

        QGroupBox *eventBox = new QGroupBox(QString::fromStdString(e.getTitle()));
        eventBox->setMinimumSize(eventWidth, eventHeight);
        eventBox->setStyleSheet("QGroupBox { background-color: rgb(240,240,240); color: rgb(88,112,159); font-size: 16px; }");

        QVBoxLayout *boxLayout = new QVBoxLayout(eventBox);
        const auto& date = e.getevent_date();
        QDate qdate(
            static_cast<int>(date.year()),
            static_cast<unsigned int>(date.month()),
            static_cast<unsigned int>(date.day())
            );

        // 使用QDate的toString()方法格式化日期
        boxLayout->addWidget(new QLabel(qdate.toString("yyyy-MM-dd")));

        QPushButton *deleteBtn = new QPushButton("删除");
        deleteBtn->setStyleSheet("background-color: rgb(211,211,211); color: rgb(88,112,159);");
        connect(deleteBtn, &QPushButton::clicked, [=]() {
            this->eventDeleted(e);
        });

        boxLayout->addWidget(deleteBtn);
        expiredGrid->addWidget(eventBox, row, col);
        col = (col + 1) % eventsPerRow;
        if (col == 0) ++row;
    }
    layout->addLayout(expiredGrid);

    // 6. 添加事项按钮
    QPushButton *addEventBtn = new QPushButton("添加事项");
    addEventBtn->setFixedSize(150, 50);
    addEventBtn->setStyleSheet("background-color: rgb(255,255,204); color: rgb(121,238,227); font-size: 18px;");
    connect(addEventBtn, &QPushButton::clicked, this, &ViewLayer::onAddEventClicked);
    layout->addWidget(addEventBtn, 0, Qt::AlignCenter);

    // 7. 返回按钮
    QPushButton *backBtn = new QPushButton("返回");
    backBtn->setFixedSize(100, 40);
    connect(backBtn, &QPushButton::clicked, this, &ViewLayer::onBackToNavigation);
    layout->addWidget(backBtn, 0, Qt::AlignRight);

    event_manage_widget->setLayout(layout);
    showView(event_manage_widget);
}

void ViewLayer::initHabitManageView() {

}

void ViewLayer::initNavigationView() {

}

void ViewLayer::onBackToNavigation() {

}

void ViewLayer::onDeleteEventClicked() {
    // 保留接口，但实际逻辑在 initEventManageView 中动态构建的按钮中实现
    QMessageBox::information(this, "提示", "请在事项卡片中点击删除按钮");
}

void ViewLayer::onAddEventClicked() {
    bool ok;
    QString name = QInputDialog::getText(this, "新建事项", "请输入事项名称：", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) {
        QMessageBox::warning(this, "错误", "事项名称不能为空！");
        return;
    }
    event_name_input = name.toStdString();

    QString dateStr = QInputDialog::getText(this, "新建事项", "请输入事项到期日期 (YYYY-MM-DD)：", QLineEdit::Normal, "", &ok);
    if (!ok || !parseDate(dateStr.toStdString(), start_date_input)) {
        QMessageBox::warning(this, "错误", "日期格式不正确！");
        return;
    }

    QString timeStr = QInputDialog::getText(this, "新建事项", "请输入事项到期时间 (HH:MM:SS)：", QLineEdit::Normal, "", &ok);
    if (!ok || !parseTime(timeStr.toStdString(), event_time_input)) {
        QMessageBox::warning(this, "错误", "时间格式不正确！");
        return;
    }

    QString remindFlagStr = QInputDialog::getText(this, "新建事项", "是否开启提醒？（1: 是, 0: 否）", QLineEdit::Normal, "0", &ok);
    if (!ok) return;

    bool remindFlag = (remindFlagStr.trimmed() == "1");
    Time remindTime(std::chrono::seconds(0));
    if (remindFlag) {
        QString remindTimeStr = QInputDialog::getText(this, "新建事项", "请输入提前提醒时间 (HH:MM:SS)：", QLineEdit::Normal, "", &ok);
        if (!ok || !parseTime(remindTimeStr.toStdString(), remindTime)) {
            QMessageBox::warning(this, "错误", "提醒时间格式不正确！");
            return;
        }
    }

    // 插入事项
    sv_Layer.insertEvent(event_name_input, start_date_input, event_time_input, remindFlag, remindTime);

    emit eventAdded();
    initEventManageView(); // 刷新界面

}

void ViewLayer::onDeleteHabitClicked() {

}

void ViewLayer::onAddHabitClicked() {

}

void ViewLayer::setcurrentView(ViewType view) {

}
