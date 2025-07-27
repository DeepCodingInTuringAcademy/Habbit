#include <QCheckBox>
#include<QDialogButtonBox>
#include <QRandomGenerator>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <iostream>
#include "ViewLayer.h"
#include <queue>
#include "CalendarView.h"
#include "NavigationBar.h"


ViewLayer::ViewLayer(QWidget *parent) : QWidget(parent),
                                        cur_view_type(ViewType::MAIN_VIEW)
{
    main_layout = new QVBoxLayout(this);

    init();
}

void ViewLayer::init()
{
    navigation_widget =  new NavigationBar(this);
    main_widget = new QWidget(this);
    habit_manage_widget = new QWidget(this);
    event_manage_widget = new QWidget(this);
    pomodoro_widget = new QWidget(this);
    timeline_widget = new QWidget(this);
    calendar_widget = new QWidget(this);
    settings_widget = new QWidget(this);

    initNavigationView();
    initMainView();
    initEventManageView();
    initHabitManageView();
    initPomodoroView();
    initTimelineView();
    initCalendarView();
    initSettingsView();

    resetCurrentView(ViewType::MAIN_VIEW);

    // 程序启动后立即检查并恢复番茄钟状态
    QTimer::singleShot(100, this, &ViewLayer::checkAndRestorePomodoroState);

    // 测试数据库操作
    QTimer::singleShot(500, this, [this]() {
        std::cout << "=== 测试数据库操作 ===" << std::endl;

        // 测试保存
        bool save_result = sv_Layer.savePomodoroState(1, 300, 250, "测试番茄钟", "2024-01-01 10:00:00");
        std::cout << "测试保存结果:" << (save_result ? "成功" : "失败") << std::endl;

        // 测试加载
        int state, total_seconds, remaining_seconds;
        std::string remark, start_time;
        bool load_result = sv_Layer.loadPomodoroState(state, total_seconds, remaining_seconds, remark, start_time);
        std::cout << "测试加载结果:" << (load_result ? "成功" : "失败") << std::endl;

        if (load_result) {
            std::cout << "加载的数据:" << std::endl;
            std::cout << "  状态:" << state << std::endl;
            std::cout << "  总时长:" << total_seconds << std::endl;
            std::cout << "  剩余时长:" << remaining_seconds << std::endl;
            std::cout << "  备注:" << remark << std::endl;
            std::cout << "  开始时间:" << start_time << std::endl;
        }

        // 测试清除
        bool clear_result = sv_Layer.clearPomodoroState();
        std::cout << "测试清除结果:" << (clear_result ? "成功" : "失败") << std::endl;

        std::cout << "=== 数据库操作测试完成 ===" << std::endl;
        // 移除自动退出逻辑，方便用户手动测试
        // QTimer::singleShot(3000, qApp, &QApplication::quit);
    });
}

void ViewLayer::resetCurrentView(ViewType view)
{
    // 隐藏所有视图
    main_widget->hide();
    navigation_widget->hide();
    habit_manage_widget->hide();
    event_manage_widget->hide();
    pomodoro_widget->hide();
    timeline_widget->hide();
    calendar_widget->hide();
    settings_widget->hide();

    // 清空主布局
    clearLayout(main_layout);

    cur_view_type = view;

    switch (view)
    {
    case ViewType::MAIN_VIEW:
        main_layout->addWidget(main_widget);
        main_widget->show();
        break;
    case ViewType::HABIT_MANAGE_VIEW:
        initHabitManageView(); // 刷新习惯管理视图
        main_layout->addWidget(habit_manage_widget);
        habit_manage_widget->show();
        break;
    case ViewType::EVENT_MANAGE_VIEW:
        initEventManageView(); // 刷新事项管理视图
        main_layout->addWidget(event_manage_widget);
        event_manage_widget->show();
        break;
    case ViewType::POMODORO_VIEW:
        main_layout->addWidget(pomodoro_widget);
        pomodoro_widget->show();
        break;
    case ViewType::TIMELINE_VIEW:
        main_layout->addWidget(timeline_widget);
        timeline_widget->show();
        break;
    case ViewType::CALENDAR_VIEW:
        initCalendarView();
        main_layout->addWidget(calendar_widget);
        calendar_widget->show();
        break;
    case ViewType::SETTINGS_VIEW:
        main_layout->addWidget(settings_widget);
        settings_widget->show();
        break;
    default:
        main_layout->addWidget(new QLabel("待开发的视图", this));
        break;
    }
    main_layout->addWidget(navigation_widget);
    navigation_widget->show();
}

bool ViewLayer::parseTime(const std::string &str, Time &result)
{
    const auto split_res = Utility::split(str, ':');
    if (split_res.size() != 3)
        return false;

    try
    {
        auto [hh_str, mm_str, ss_str] = std::tie(split_res[0], split_res[1], split_res[2]);
        const int hh = std::stoi(hh_str);
        const int mm = std::stoi(mm_str);
        const int ss = std::stoi(ss_str);

        if (hh < 0 || hh >= 24 || mm < 0 || mm >= 60 || ss < 0 || ss >= 60)
            return false;

        const std::chrono::seconds total = std::chrono::hours{hh} + std::chrono::minutes{mm} + std::chrono::seconds{ss};
        result = Time{total};
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void ViewLayer::showView(ViewType type)
{
    this->setCurrentView(type);
    this->show();
}

bool ViewLayer::parseDate(const std::string &str, Date &result)
{
    const auto split_res = Utility::split(str, '-');
    if (split_res.size() != 3)
        return false;

    try
    {
        auto [year_str, month_str, day_str] = std::tie(split_res[0], split_res[1], split_res[2]);
        const int year = std::stoi(year_str);
        const unsigned month = std::stoi(month_str);
        const unsigned day = std::stoi(day_str);

        if (year < 1900 || year >= 2100 || month < 1 || month > 12 || day < 1 || day > 31)
        {
            return false;
        }

        const std::chrono::year_month_day res = std::chrono::year{year} / std::chrono::month{month} / std::chrono::day{day};
        if (!res.ok())
        {
            return false;
        }
        result = res;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

void ViewLayer::clearLayout(QLayout *layout)
{
    if (!layout) return;

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        if (QWidget* widget = item->widget())
        {
            layout->removeWidget(widget);
            widget->hide();
            widget->setParent(nullptr);
        }
        else if (QLayout* childLayout = item->layout())
        {
            clearLayout(childLayout);
        }

        delete item;
    }
}

void ViewLayer::initEventManageView()
{
    if (!event_manage_widget)
        event_manage_widget = new QWidget(this);

    clearLayout(event_manage_widget->layout());

    QGridLayout *layout = qobject_cast<QGridLayout*>(event_manage_widget->layout());
    if (!layout) {
        layout = new QGridLayout();
        event_manage_widget->setLayout(layout);
    }

    // 顶部标题
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("事项管理", event_manage_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topLayout->addWidget(title);
    topLayout->addStretch();
    layout->addLayout(topLayout, 0, 0, 1, 4);

    // 滚动区域和事项列表容器
    QScrollArea *scrollArea = new QScrollArea(event_manage_widget);
    QWidget *eventListContainer = new QWidget();
    QGridLayout *grid = new QGridLayout(eventListContainer);
    grid->setAlignment(Qt::AlignTop | Qt::AlignLeft); // 关键：左上对齐

    std::vector<Event> events = sv_Layer.getActiveEvents();
    constexpr int eventsPerRow = 6;

    int row = 0, col = 0;
    int i = 0;
    for (i = 0; i < events.size(); ++i)
    {
        const Event &event = events[i];

        QWidget *eventCard = new QWidget();
        eventCard->setFixedSize(150, 150);
        eventCard->setStyleSheet(
            "background-color: #fefefe;"
            "border: 1px solid #cccccc;"
            "padding: 1px;"
            "margin: 1px;"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(eventCard);
        QLabel *nameLabel = new QLabel(QString::fromStdString("标题: " + event.title));
        QLabel *dateLabel = new QLabel(QString::fromStdString("日期: " + toString(event.event_date)));
        QLabel *timeLabel = new QLabel(QString::fromStdString("时间: " + toString(event.event_time)));
        QLabel *remindLabel = new QLabel(QString::fromStdString("提醒: ") + (event.remind_flag ? "是" : "否"));

        QHBoxLayout *buttonLayout = new QHBoxLayout();

        QPushButton *modifyBtn = new QPushButton();
        modifyBtn->setIcon(QIcon(":/assets/images/modify.png"));
        modifyBtn->setIconSize(QSize(18, 18));
        modifyBtn->setFixedSize(25, 25);

        QPushButton *deleteBtn = new QPushButton();
        deleteBtn->setIcon(QIcon(":/assets/images/delete.png"));
        deleteBtn->setIconSize(QSize(20, 20));
        deleteBtn->setFixedSize(25, 25);

        connect(modifyBtn, &QPushButton::clicked, [this, event]() {
            EventUpdateView(event);
        });
        connect(deleteBtn, &QPushButton::clicked, [this, event]() {
            if (QMessageBox::question(this, "确认删除", "确定删除该事项吗？") == QMessageBox::Yes)
            {
                if (sv_Layer.deleteEvent(event.event_id))
                {
                    QMessageBox::information(this, "提示", "删除成功");
                    initEventManageView();
                }
                else
                {
                    QMessageBox::warning(this, "错误", "删除失败");
                }
            }
        });

        buttonLayout->addWidget(modifyBtn);
        buttonLayout->addWidget(deleteBtn);

        cardLayout->addWidget(nameLabel);
        cardLayout->addWidget(dateLabel);
        cardLayout->addWidget(timeLabel);
        cardLayout->addWidget(remindLabel);
        cardLayout->addLayout(buttonLayout);

        int row = i / eventsPerRow;
        int col = i % eventsPerRow;
        grid->addWidget(eventCard, row, col);
    }

    // 添加事项按钮
    QPushButton *addEventBtn = new QPushButton();
    addEventBtn->setIcon(QIcon(":/assets/images/add.png"));
    addEventBtn->setIconSize(QSize(36, 36));
    addEventBtn->setFixedSize(150, 150);
    connect(addEventBtn, &QPushButton::clicked, this, [this]()
    {
        eventInsertView();
    });

    i++;
    row = i / eventsPerRow;
    col = i % eventsPerRow;
    grid->addWidget(addEventBtn, row, col);

    eventListContainer->setLayout(grid->layout());
    scrollArea->setWidget(eventListContainer);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea, 1, 0, 1, 4);  // 占据第1行，4列
}

void ViewLayer::eventInsertView()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建事项");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *name_edit = new QLineEdit(event_manage_widget);
    name_edit->setPlaceholderText("请输入事项名称");

    QPushButton* event_date_btn = new QPushButton("未选择");

    QLineEdit *time_edit = new QLineEdit(event_manage_widget);
    time_edit->setPlaceholderText("请输入事项时间（hh:mm:ss）");

    QCheckBox *remind_checkbox = new QCheckBox("开启提醒", event_manage_widget);

    QLineEdit *remind_time_input = new QLineEdit(event_manage_widget);
    remind_time_input->setPlaceholderText("请输入提醒时间 (hh:mm:ss)");

    layout->addWidget(name_edit);
    layout->addWidget(new QLabel("开始日期:"));
    layout->addWidget(event_date_btn);
    layout->addWidget(time_edit);
    layout->addWidget(remind_checkbox);
    layout->addWidget(remind_time_input);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    connect(event_date_btn, &QPushButton::clicked, [&dialog, &event_date_btn]
    {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            event_date_btn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string event_name = name_edit->text().toStdString();
        std::string event_date_str = event_date_btn->text().toStdString();
        std::string event_time_str = time_edit->text().toStdString();
        bool remind_flag = remind_checkbox->isChecked();
        std::string remind_time_str = remind_time_input->text().toStdString();

        Date event_date;
        Time event_time;
        Time remind_time;

        if (!parseDate(event_date_str, event_date) || !parseTime(event_time_str, event_time))
        {
            QMessageBox::warning(this, "错误", "日期或时间格式不正确！");
            return;
        }

        if (remind_flag && !parseTime(remind_time_str, remind_time))
        {
            QMessageBox::warning(this, "错误", "提醒时间格式不正确！");
            return;
        }

        if (sv_Layer.insertEvent(event_name, event_date, event_time, remind_flag, remind_time))
        {
            QMessageBox::information(this, "提示", "事项添加成功！");
            initEventManageView();
        }
        else
        {
            QMessageBox::warning(this, "错误", "事项添加失败，请检查输入！");
        }
    }
}

void ViewLayer::EventUpdateView(const Event &event)
{
    QDialog dialog(this);
    dialog.setWindowTitle("修改事项");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *name_edit = new QLineEdit(QString::fromStdString(event.title));
    QPushButton* event_date_btn = new QPushButton(QString::fromStdString(toString(event.event_date)));
    QLineEdit *time_edit = new QLineEdit(QString::fromStdString(toString(event.event_time)));
    QCheckBox *remind_checkbox = new QCheckBox("开启提醒");
    remind_checkbox->setChecked(event.remind_flag);
    QLineEdit *remind_time_edit = new QLineEdit(QString::fromStdString(toString(event.remind_time)));

    layout->addWidget(name_edit);
    layout->addWidget(event_date_btn);
    layout->addWidget(time_edit);
    layout->addWidget(remind_checkbox);
    layout->addWidget(remind_time_edit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    connect(event_date_btn, &QPushButton::clicked, [&dialog, &event_date_btn]
    {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            event_date_btn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string new_name = name_edit->text().toStdString();
        std::string date_str = event_date_btn->text().toStdString();
        std::string time_str = time_edit->text().toStdString();
        bool remind_flag = remind_checkbox->isChecked();
        std::string remind_time_str = remind_time_edit->text().toStdString();

        Date new_date;
        Time new_time, new_remind;

        if (!parseDate(date_str, new_date) || !parseTime(time_str, new_time)) {
            QMessageBox::warning(this, "错误", "时间或日期格式不正确！");
            return;
        }

        if (remind_flag && !parseTime(remind_time_str, new_remind)) {
            QMessageBox::warning(this, "错误", "提醒时间格式错误！");
            return;
        }

        if (sv_Layer.updateEvent(event.event_id, new_name, new_date, new_time, remind_flag, new_remind))
        {
            QMessageBox::information(this, "成功", "修改成功！");
            initEventManageView();  // 重新刷新列表
        }
        else
        {
            QMessageBox::warning(this, "失败", "修改失败！");
        }
    }
}

void ViewLayer::initMainView()
{
    if (!main_widget)
        main_widget = new QWidget(this);

    // 彻底清空主界面原有布局和控件，防止布局错乱
    if (main_widget->layout()) {
        QLayoutItem* item;
        while ((item = main_widget->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        delete main_widget->layout();
    }

    // 主垂直布局（包含内容区和底部导航栏）
    QVBoxLayout* mainVLayout = new QVBoxLayout(main_widget);

    // ======= 内容区：网格布局 =======
    QGridLayout* gridLayout = new QGridLayout();
    // 1. 左侧：习惯列表区
    QGroupBox* habitGroup = new QGroupBox("待打卡习惯", main_widget);

    // 创建滚动区域
    QScrollArea* habitScroll = new QScrollArea(habitGroup);
    habitScroll->setWidgetResizable(true);
    habitScroll->setFixedHeight(500); // 可根据需要调整高度
    habitScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    habitScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    habitScroll->setStyleSheet(R"(
    QScrollBar:horizontal {
        height: 8px;
        background: #f0f0f0;
        margin: 0px 20px 0 20px;
        border-radius: 4px;
    }
    QScrollBar::handle:horizontal {
        background: #bfbfbf;
        min-width: 24px;
        border-radius: 4px;
    }
    QScrollBar:vertical {
        width: 8px;
        background: #f0f0f0;
        margin: 20px 0 20px 0;
        border-radius: 4px;
    }
    QScrollBar::handle:vertical {
        background: #bfbfbf;
        min-height: 24px;
        border-radius: 4px;
    }
    QScrollBar::add-line, QScrollBar::sub-line {
        background: none;
        border: none;
    }
)");

    // 容器和布局
    QWidget* habitListContainer = new QWidget();
    habitListContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout* habitLayout = new QVBoxLayout(habitListContainer);

    // 获取今日打卡记录
    Date today = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
    DateRecord todayRecord = sv_Layer.getAllRecordsByDate(today);

    std::vector<Habit> habits = sv_Layer.getActiveHabits();
    if (habits.empty()) {
        QLabel* noHabit = new QLabel("暂无习惯");
        noHabit->setAlignment(Qt::AlignCenter);
        noHabit->setStyleSheet("color:#888;font-size:18px;margin:20px 0;");
        habitLayout->addWidget(noHabit);
    } else {
        int index = 0;
        for (const auto& habit : habits) {
            if (index >= 5) break; // 最多显示5个卡片
            // 统计今日打卡次数
            int todayCheckin = 0;
            for (const auto& rec : todayRecord.habit_records) {
                if (rec.second.habit_id == habit.habit_id) {
                    ++todayCheckin;
                }
            }
            QWidget* habitCard = new QWidget();
            habitCard->setFixedHeight(80); // 固定卡片高度
            habitCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 关键

            QHBoxLayout* cardLayout = new QHBoxLayout(habitCard);
            cardLayout->setAlignment(Qt::AlignVCenter);
            cardLayout->setSpacing(16);

            // 习惯名称框
            QLabel* nameLabel = new QLabel(QString::fromStdString(habit.name));
            nameLabel->setFixedSize(120, 50);
            nameLabel->setAlignment(Qt::AlignCenter);
            nameLabel->setStyleSheet("font-weight:bold;font-size:16px;border-radius:8px;background:#fff;border:1px solid #e0e0e0;");
            nameLabel->setWordWrap(true);

            // 日期
            QLabel* dateLabel = new QLabel(
                QString("%1 ~ %2")
                .arg(QString::fromStdString(toString(habit.start_date)))
                .arg(QString::fromStdString(toString(habit.end_date)))
            );
            dateLabel->setStyleSheet("font-size:16px;");



            // 打卡次数
            QLabel* checkinLabel = new QLabel(
                QString("打卡：%1 / %2 次").arg(QString::number(todayCheckin)).arg(QString::number(habit.target_count))
            );
            checkinLabel->setStyleSheet("font-size:16px;");

            // 打卡按钮
            QPushButton* checkinBtn = new QPushButton("打卡");
            checkinBtn->setFixedSize(60, 30);
            checkinBtn->setStyleSheet(
                "QPushButton {"
                "    background-color: #52c41a;"
                "    color: white;"
                "    border: none;"
                "    border-radius: 6px;"
                "    font-size: 14px;"
                "    font-weight: bold;"
                "    padding: 2px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #389e0d;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #237804;"
                "}"
            );

            // 连接打卡按钮信号
            connect(checkinBtn, &QPushButton::clicked, [this, habit, checkinLabel, todayCheckin]() mutable {
                // 检查是否已达到目标次数
                if (todayCheckin >= habit.target_count) {
                    QMessageBox::information(this, "打卡完成", QString("习惯「%1」今日打卡已完成！(%2/%3)").arg(QString::fromStdString(habit.name)).arg(QString::number(todayCheckin)).arg(QString::number(habit.target_count)));
                    return;
                }

                if (sv_Layer.checkinHabit(habit)) {
                    // 直接增加打卡次数
                    todayCheckin++;
                    checkinLabel->setText(QString("打卡：%1 / %2 次").arg(QString::number(todayCheckin)).arg(QString::number(habit.target_count)));

                    // 检查是否完成目标
                    if (todayCheckin >= habit.target_count) {
                        QMessageBox::information(this, "打卡完成", QString("习惯「%1」今日打卡已完成！(%2/%3)").arg(QString::fromStdString(habit.name)).arg(QString::number(todayCheckin)).arg(QString::number(habit.target_count)));
                        // 可选：禁用打卡按钮
                        // checkinBtn->setEnabled(false);
                        // checkinBtn->setStyleSheet(
                        //     "QPushButton {"
                        //     "    background-color: #d9d9d9;"
                        //     "    color: #999;"
                        //     "    border: none;"
                        //     "    border-radius: 6px;"
                        //     "    font-size: 14px;"
                        //     "    font-weight: bold;"
                        //     "    padding: 2px;"
                        //     "}"
                        // );
                    } else {
                        QMessageBox::information(this, "打卡成功", QString("习惯「%1」打卡成功！(%2/%3)").arg(QString::fromStdString(habit.name)).arg(QString::number(todayCheckin)).arg(QString::number(habit.target_count)));
                    }
                } else {
                    QMessageBox::warning(this, "打卡失败", QString("习惯「%1」打卡失败！").arg(QString::fromStdString(habit.name)));
                }
            });

            // 横向布局
            cardLayout->addWidget(nameLabel);
            cardLayout->addWidget(dateLabel);
            cardLayout->addWidget(checkinLabel);
            cardLayout->addWidget(checkinBtn);
            cardLayout->addStretch();
            habitCard->setLayout(cardLayout);

            // 卡片样式
            habitCard->setStyleSheet(
                "background:#fff;"
                "border:1px solid #e0e0e0;"
                "border-radius:12px;"
                "padding:8px;"
                "margin-bottom:12px;"
            );

            habitLayout->addWidget(habitCard);
            ++index;
        }
    }

    habitListContainer->setLayout(habitLayout);
    habitScroll->setWidget(habitListContainer);

    QVBoxLayout* groupLayout = new QVBoxLayout(habitGroup);
    groupLayout->addWidget(habitScroll);

    habitGroup->setLayout(groupLayout);
    gridLayout->addWidget(habitGroup, 0, 0, 2, 1);

    // 2. 右上：事项区
    QGroupBox* eventGroup = new QGroupBox("活跃事项", main_widget);
    QWidget* eventListContainer = new QWidget();
    QVBoxLayout* eventLayout = new QVBoxLayout(eventListContainer);

    std::vector<Event> events = sv_Layer.getActiveEvents();
    if (events.empty()) {
        QLabel* noEvent = new QLabel("暂无事项");
        noEvent->setAlignment(Qt::AlignCenter);
        noEvent->setStyleSheet("color:#888;font-size:18px;margin:20px 0;");
        eventLayout->addWidget(noEvent);
    } else {
        int index = 0;
        for (const auto& event : events) {
            if (index >= 4) break; // 最多显示4个卡片
            QWidget* eventCard = new QWidget();
            QHBoxLayout* cardLayout = new QHBoxLayout(eventCard);
            QLabel* nameLabel = new QLabel(QString::fromStdString(event.title));
            QLabel* dateLabel = new QLabel(QString::fromStdString(toString(event.event_date)));
            QLabel* timeLabel = new QLabel(QString::fromStdString(toString(event.event_time)));
            cardLayout->addWidget(nameLabel);
            cardLayout->addWidget(dateLabel);
            cardLayout->addWidget(timeLabel);
            eventCard->setLayout(cardLayout);
            eventLayout->addWidget(eventCard);
            ++index;
        }
    }
    eventListContainer->setLayout(eventLayout);
    QVBoxLayout* eventGroupLayout = new QVBoxLayout(eventGroup);
    eventGroupLayout->addWidget(eventListContainer);
    eventGroup->setLayout(eventGroupLayout);
    gridLayout->addWidget(eventGroup, 0, 1);

    // 3. 右下：番茄钟区
    QGroupBox* pomoGroup = new QGroupBox("当前番茄钟", main_widget);
    QVBoxLayout* pomoLayout = new QVBoxLayout(pomoGroup);

    // 创建番茄钟显示组件
    main_pomodoro_remark_label = new QLabel("", pomoGroup);
    main_pomodoro_remark_label->setAlignment(Qt::AlignCenter);
    main_pomodoro_remark_label->setStyleSheet("color:#333;font-size:30px;font-weight:bold;margin:5px 0;");
    main_pomodoro_remark_label->hide();

    main_pomodoro_time_label = new QLabel("00 : 00 : 00", pomoGroup);
    main_pomodoro_time_label->setAlignment(Qt::AlignCenter);
    main_pomodoro_time_label->setStyleSheet("color:#1890ff;font-size:20px;font-weight:bold;margin:5px 0;");
    main_pomodoro_time_label->hide();

    main_pomodoro_no_pomodoro_label = new QLabel("暂无番茄钟", pomoGroup);
    main_pomodoro_no_pomodoro_label->setAlignment(Qt::AlignCenter);
    main_pomodoro_no_pomodoro_label->setStyleSheet("color:#888;font-size:16px;margin:20px 0;");

    pomoLayout->addWidget(main_pomodoro_remark_label);
    pomoLayout->addWidget(main_pomodoro_time_label);
    pomoLayout->addWidget(main_pomodoro_no_pomodoro_label);

    pomoGroup->setLayout(pomoLayout);
    gridLayout->addWidget(pomoGroup, 1, 1);

    // 4. 右下角：吉祥物+对话框
    QWidget* dialogMascotWidget = new QWidget(main_widget);
    QHBoxLayout* dialogMascotLayout = new QHBoxLayout(dialogMascotWidget);
    QGroupBox* dialogGroup = new QGroupBox(dialogMascotWidget);
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogGroup);
    if (!dialogLabel) dialogLabel = new QLabel(dialogGroup);
    dialogLabel->setText(dialogTexts[0]);
    dialogLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dialogLabel->setWordWrap(true);
    dialogGroup->setStyleSheet(R"(
    QGroupBox {
        background: #fffbe6;
        border: 2px solid #ffd666;
        border-radius: 16px;
        margin-top: 8px;
        margin-bottom: 8px;
        padding: 16px;
        min-width: 220px;
        max-width: 320px;
    }
)");
    dialogLabel->setStyleSheet(R"(
    QLabel {
        color: #ad6800;
        font-size: 18px;
        font-weight: bold;
        padding: 4px 8px;
        background: transparent;
    }
)");
    dialogLayout->addWidget(dialogLabel);
    dialogGroup->setLayout(dialogLayout);
    dialogMascotLayout->addWidget(dialogGroup, 2);
    QLabel* mascotLabel = new QLabel(dialogMascotWidget);
    QPixmap mascotPixmap(":/assets/images/logo.png");
    if (mascotPixmap.isNull()) {
        mascotLabel->setText("logo未加载");
    } else {
        mascotLabel->setPixmap(mascotPixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    dialogMascotLayout->addWidget(mascotLabel, 1, Qt::AlignRight | Qt::AlignBottom);
    dialogMascotWidget->setLayout(dialogMascotLayout);
    gridLayout->addWidget(dialogMascotWidget, 2, 1);

    // 设置行列拉伸比例
    gridLayout->setRowStretch(0, 2);
    gridLayout->setRowStretch(1, 2);
    gridLayout->setRowStretch(2, 1);
    gridLayout->setColumnStretch(0, 5);  // 习惯框占5份
    gridLayout->setColumnStretch(1, 2);  // 右侧区域占2份

    // 把内容区加到主垂直布局
    mainVLayout->addLayout(gridLayout, 10);
    mainVLayout->addWidget(navigation_widget, 1);


    main_widget->setLayout(mainVLayout);
    dialogLabel->installEventFilter(this);
    main_widget->setStyleSheet("background: #F5F6FA;");

    // 初始化主界面番茄钟定时器
    if (!main_pomodoro_timer) {
        main_pomodoro_timer = new QTimer(this);
        main_pomodoro_timer->setInterval(1000); // 每秒更新
        connect(main_pomodoro_timer, &QTimer::timeout, this, &ViewLayer::updateMainPomodoroDisplay);
    }

    // 检查并恢复番茄钟状态（延迟执行，确保番茄钟组件已创建）
    QTimer::singleShot(200, this, &ViewLayer::checkAndRestorePomodoroState);
}

void ViewLayer::initTimelineView()
{
    if (!timeline_widget)
    {
        timeline_widget = new QWidget(this);
    }

    clearLayout(timeline_widget->layout());

    QVBoxLayout *layout = new QVBoxLayout(timeline_widget);

    // 顶部标题
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("时间线", timeline_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topLayout->addWidget(title);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    // 日期选择栏
    QHBoxLayout *dateLayout = new QHBoxLayout();
    QPushButton *prevDayButton = createButton(":/assets/images/backward.png", "前一天", 40, 20);
    QPushButton *nextDayButton = createButton(":/assets/images/forward.png", "后一天", 40, 20);
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("yyyy-MM-dd");
    dateEdit->setCalendarPopup(true);

    dateLayout->addWidget(prevDayButton);
    dateLayout->addWidget(dateEdit);
    dateLayout->addWidget(nextDayButton);
    dateLayout->addStretch();
    layout->addLayout(dateLayout);

    // 时间线展示区域
    timeline_scroll_area = new QScrollArea(timeline_widget);
    timeline_scroll_area->setWidgetResizable(true);
    timeline_content_widget = new QWidget();
    timeline_layout = new QVBoxLayout(timeline_content_widget);
    timeline_content_widget->setLayout(timeline_layout);
    timeline_scroll_area->setWidget(timeline_content_widget);
    layout->addWidget(timeline_scroll_area);

    // 信号连接
    connect(prevDayButton, &QPushButton::clicked, [this]()
    {
        dateEdit->setDate(dateEdit->date().addDays(-1));
        refreshTimeline();
    });

    connect(nextDayButton, &QPushButton::clicked, [this]()
    {
        dateEdit->setDate(dateEdit->date().addDays(1));
        refreshTimeline();
    });

    connect(dateEdit, &QDateEdit::dateChanged, this, &ViewLayer::refreshTimeline);

    refreshTimeline();  // 初次加载
}

QPushButton* ViewLayer::createButton(const QString& iconPath, const QString& tooltip, int width, int height)
{
    QPushButton *button = new QPushButton();
    button->setIcon(QIcon(iconPath));
    button->setIconSize(QSize(width, height));
    button->setFixedSize(width, height);
    button->setToolTip(tooltip);
    return button;
}

struct TimeTupleComparator
{
    bool operator()(const std::tuple<Time, std::string, std::string> &a,
                    const std::tuple<Time, std::string, std::string> &b) const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::get<0>(a).to_duration())
             > std::chrono::duration_cast<std::chrono::seconds>(std::get<0>(b).to_duration());
    }
};

void ViewLayer::refreshTimeline()
{
    if (!timeline_layout) return;

    // 清除旧内容
    QLayoutItem *child;
    while ((child = timeline_layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 获取日期
    const auto qdate = dateEdit->date();
    const Date date = Date{std::chrono::year{qdate.year()} / qdate.month() / qdate.day()};

    // 向服务层请求当天记录
    DateRecord raw_record = sv_Layer.getAllRecordsByDate(date);
    std::vector<Event> events = sv_Layer.getEventsByDate(Utility::chronoToQDateTime(date, Time{}).date());

    // 定义用于排序合并的向量（按 Time 升序）
    using TimelineItem = std::tuple<Time, std::string, std::string>; // time, type, content
    std::vector<TimelineItem> timeline_items;

    // 插入习惯记录
    for (const auto &pair : raw_record.habit_records)
    {
        const Time &time = pair.first;
        const Habit &habit = pair.second;
        timeline_items.emplace_back(time, "习惯打卡", habit.name);
    }

    // 插入番茄钟记录
    for (const auto &pair : raw_record.pomodoro_records)
    {
        const Time &time = pair.first;
        const Pomodoro &pomodoro = pair.second;
        timeline_items.emplace_back(time, pomodoro.record, "番茄钟专注 " + toString(pomodoro.pomodoro_time));
    }

    // 插入事件记录
    for (const auto &event : events)
    {
        timeline_items.emplace_back(event.event_time, "事件", event.title);
    }

    // 按时间排序
    std::sort(timeline_items.begin(), timeline_items.end(), [](const TimelineItem &a, const TimelineItem &b) {
        return std::get<0>(a) < std::get<0>(b);
    });

    // 添加到布局
    for (const auto &[time, type, content] : timeline_items)
    {
        QString display = QString("%1 - %2 - %3").arg(QString::fromStdString(toString(time))).arg(QString::fromStdString(type)).arg(QString::fromStdString(content));
        QLabel *label = new QLabel(display);
        label->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 5px;");
        timeline_layout->addWidget(label);
    }
}

void ViewLayer::initPomodoroView()
{
    if (!pomodoro_widget)
    {
        pomodoro_widget = new QWidget(this);
    }

    clearLayout(pomodoro_widget->layout());

    auto *layout = new QVBoxLayout(pomodoro_widget);

    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("番茄钟", pomodoro_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topLayout->addWidget(title);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    // 主体容器内容 - 圆形钟居中显示
    QWidget* center_container = new QWidget(pomodoro_widget);
    QVBoxLayout* center_layout = new QVBoxLayout(center_container);
    center_layout->setContentsMargins(0, 0, 0, 0);

    if (!pomodoro_widget_component)
    {
        pomodoro_widget_component = new PomodoroWidget(pomodoro_widget);

        // 连接番茄钟信号到主界面更新
        connect(pomodoro_widget_component, &PomodoroWidget::stateChanged, this, &ViewLayer::updateMainPomodoroDisplay);
        connect(pomodoro_widget_component, &PomodoroWidget::timerUpdated, this, &ViewLayer::updateMainPomodoroDisplay);

        // 番茄钟组件创建后，立即检查并恢复状态
        QTimer::singleShot(50, this, &ViewLayer::checkAndRestorePomodoroState);

        // 连接状态改变信号到保存状态
        connect(pomodoro_widget_component, &PomodoroWidget::stateChanged, this, [this]() {
            if (pomodoro_widget_component) {
                qDebug() << "番茄钟状态改变，当前状态:" << pomodoro_widget_component->getState();

                // 如果是IDLE状态，清除数据库中的状态
                if (pomodoro_widget_component->getState() == PomodoroWidget::IDLE) {
                    qDebug() << "清除番茄钟状态";
                    sv_Layer.clearPomodoroState();
                } else {
                    // 保存番茄钟状态到数据库
                    qDebug() << "保存番茄钟状态到数据库";
                    bool success = sv_Layer.savePomodoroState(
                        static_cast<int>(pomodoro_widget_component->getState()),
                        pomodoro_widget_component->getTotalSeconds(),
                        pomodoro_widget_component->getRemainingSeconds(),
                        pomodoro_widget_component->getRemark().toStdString(),
                        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()
                    );
                    qDebug() << "保存番茄钟状态结果:" << (success ? "成功" : "失败");
                }
            }
        });

        // 连接定时器更新信号到保存状态（每秒保存一次）
        connect(pomodoro_widget_component, &PomodoroWidget::timerUpdated, this, [this]() {
            if (pomodoro_widget_component && pomodoro_widget_component->getState() == PomodoroWidget::RUNNING) {
                // 每秒保存运行中的番茄钟状态
                bool success = sv_Layer.savePomodoroState(
                    static_cast<int>(pomodoro_widget_component->getState()),
                    pomodoro_widget_component->getTotalSeconds(),
                    pomodoro_widget_component->getRemainingSeconds(),
                    pomodoro_widget_component->getRemark().toStdString(),
                    QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()
                );
                if (!success) {
                    qDebug() << "定时器更新时保存番茄钟状态失败";
                }
            }
        });
    }

    // 添加弹性空间使圆形钟居中
    center_layout->addStretch();
    center_layout->addWidget(pomodoro_widget_component, 0, Qt::AlignCenter);
    center_layout->addStretch();

    layout->addWidget(center_container);


}

void ViewLayer::initSettingsView()
{
    if (!settings_widget)
    {
        settings_widget = new QWidget(this);
    }

    clearLayout(settings_widget->layout());

    auto *layout = new QVBoxLayout(settings_widget);

    // 顶部：标题
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("个人设置", settings_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topLayout->addWidget(title);
    topLayout->addStretch();
    layout->addLayout(topLayout);


}

void ViewLayer::initCalendarView()
{
    if (!calendar_widget) {
        calendar_widget = new QWidget(this);
    }

    clearLayout(calendar_widget->layout());

    // 获取或创建主布局
    QVBoxLayout * layout = qobject_cast<QVBoxLayout*>(calendar_widget->layout());
    if (!layout) {
        layout = new QVBoxLayout(calendar_widget);
    }

    // 标题
    auto top_layout = new QHBoxLayout();
    auto title = new QLabel("日历", calendar_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    top_layout->addWidget(title);
    top_layout->addStretch();
    layout->addLayout(top_layout);

    // 创建 CalendarView 并传入 ServiceLayer
    auto calendar_view = new CalendarView(&sv_Layer, calendar_widget);
    connect(calendar_view, &CalendarView::dateClicked, this, [this](const QDate& date) {
        qDebug() << "Date clicked:" << date;
    });

    layout->addWidget(calendar_view);

    calendar_widget->setLayout(layout);
}

void ViewLayer::habitInsertView()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建习惯");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("请输入习惯名称");

    // 使用日历选择按钮
    QPushButton *startDateBtn = new QPushButton("未选择");
    QPushButton *endDateBtn = new QPushButton("未选择");

    connect(startDateBtn, &QPushButton::clicked, [&dialog, &startDateBtn]() {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            startDateBtn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    connect(endDateBtn, &QPushButton::clicked, [&dialog, &endDateBtn]() {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            endDateBtn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    QSpinBox *targetCountSpin = new QSpinBox();
    targetCountSpin->setRange(1, 1000);
    targetCountSpin->setPrefix("每日目标次数：");

    layout->addWidget(new QLabel("习惯名称:"));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("开始日期:"));
    layout->addWidget(startDateBtn);
    layout->addWidget(new QLabel("结束日期:"));
    layout->addWidget(endDateBtn);
    layout->addWidget(targetCountSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string name = nameEdit->text().toStdString();
        Date start, end;
        if (!parseDate(startDateBtn->text().toStdString(), start) ||
            !parseDate(endDateBtn->text().toStdString(), end))
        {
            QMessageBox::warning(this, "格式错误", "日期格式不正确！");
            return;
        }

        int count = targetCountSpin->value();
        if (sv_Layer.insertHabit(name, start, end, count))
        {
            QMessageBox::information(this, "成功", "添加习惯成功！");
            initHabitManageView();
        }
        else
        {
            QMessageBox::warning(this, "失败", "添加习惯失败！");
        }
    }
}

void ViewLayer::habitUpdateView(const Habit &habit)
{
    QDialog dialog(this);
    dialog.setWindowTitle("修改习惯");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit(QString::fromStdString(habit.name));

    QPushButton *startDateBtn = new QPushButton(QString::fromStdString(toString(habit.start_date)));
    QPushButton *endDateBtn = new QPushButton(QString::fromStdString(toString(habit.end_date)));

    connect(startDateBtn, &QPushButton::clicked, [&dialog, &startDateBtn]() {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            startDateBtn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    connect(endDateBtn, &QPushButton::clicked, [&dialog, &endDateBtn]() {
        CalendarDialog calendar(&dialog);
        if (calendar.exec() == QDialog::Accepted) {
            QDate date = calendar.selectedDate();
            endDateBtn->setText(date.toString("yyyy-MM-dd"));
        }
    });

    QSpinBox *targetCountSpin = new QSpinBox();
    targetCountSpin->setRange(1, 1000);
    targetCountSpin->setValue(habit.target_count);
    targetCountSpin->setPrefix("每日目标次数：");

    layout->addWidget(new QLabel("习惯名称:"));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("开始日期:"));
    layout->addWidget(startDateBtn);
    layout->addWidget(new QLabel("结束日期:"));
    layout->addWidget(endDateBtn);
    layout->addWidget(targetCountSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string new_name = nameEdit->text().toStdString();
        Date newStart, newEnd;
        if (!parseDate(startDateBtn->text().toStdString(), newStart) ||
            !parseDate(endDateBtn->text().toStdString(), newEnd))
        {
            QMessageBox::warning(this, "格式错误", "日期格式不正确！");
            return;
        }

        int newCount = targetCountSpin->value();
        if (sv_Layer.updateHabit(habit.habit_id, new_name, newStart, newEnd, newCount, true))
        {
            QMessageBox::information(this, "成功", "修改成功！");
            initHabitManageView();
        }
        else
        {
            QMessageBox::warning(this, "失败", "修改失败！");
        }
    }
}

void ViewLayer::initHabitManageView()
{
    if (!habit_manage_widget)
        habit_manage_widget = new QWidget(this);

    clearLayout(habit_manage_widget->layout());  // 清空旧布局

    // 如果没有布局，重新设置一个
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(habit_manage_widget->layout());
    if (!gridLayout)
    {
        gridLayout = new QGridLayout();
        habit_manage_widget->setLayout(gridLayout);
    }

    // 顶部标题
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("习惯管理", habit_manage_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topLayout->addWidget(title);
    topLayout->addStretch();
    gridLayout->addLayout(topLayout, 0, 0, 1, 4);  // 占据第0行，4列

    // 活跃习惯展示区（滚动区域）
    QScrollArea *activeScrollArea = new QScrollArea(habit_manage_widget);
    QWidget *activeHabitListContainer = new QWidget();
    QGridLayout *activeHabitGridLayout = new QGridLayout(activeHabitListContainer);

    activeHabitGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    std::vector<Habit> activeHabits = sv_Layer.getActiveHabits();
    constexpr int habitsPerRow = 5;

    int activeRow = 0, activeCol = 0;
    int i = 0;
    for (i = 0; i < activeHabits.size(); ++i)
    {
        const Habit &habit = activeHabits[i];
        QWidget *habitCard = new QWidget();
        habitCard->setFixedSize(180, 150);
        habitCard->setStyleSheet
        (
            "background-color: #fefefe;"
            "border: 1px solid #cccccc;"
            "padding: 1px;"
            "margin: 1px;"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(habitCard);

        QLabel *nameLabel = new QLabel(QString::fromStdString("名称: " + habit.name));
        QLabel *countLabel = new QLabel(QString("目标: %1").arg(QString::number(habit.target_count)));
        QLabel *startLabel = new QLabel(QString::fromStdString("开始: " + toString(habit.start_date)));
        QLabel *endLabel = new QLabel(QString::fromStdString("结束: " + toString(habit.end_date)));

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *modifyBtn = new QPushButton();
        modifyBtn->setIcon(QIcon(":/assets/images/modify.png"));
        modifyBtn->setFixedSize(40, 22);

        QPushButton *deleteBtn = new QPushButton();
        deleteBtn->setIcon(QIcon(":/assets/images/delete.png"));
        deleteBtn->setFixedSize(40, 22);

        QPushButton *checkinBtn = new QPushButton();
        checkinBtn->setIcon(QIcon(":/assets/images/check.png"));
        checkinBtn->setFixedSize(40, 22);

        QPushButton *disableBtn = new QPushButton();
        disableBtn->setIcon(QIcon(":/assets/images/inactive.png"));
        disableBtn->setFixedSize(40, 22);

        connect(modifyBtn, &QPushButton::clicked, [this, habit]()
        {
            habitUpdateView(habit);
        });
        connect(deleteBtn, &QPushButton::clicked, [this, habit]()
        {
            if (QMessageBox::question(this, "确认删除", "确定删除该习惯吗？") == QMessageBox::Yes)
            {
                if (sv_Layer.deleteHabit(habit.habit_id))
                {
                    QMessageBox::information(this, "提示", "删除成功");
                    initHabitManageView();  // 重新刷新
                }
                else
                {
                    QMessageBox::warning(this, "错误", "删除失败");
                }
            }
        });
        connect(checkinBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.checkinHabit(habit))
            {
                QMessageBox::information(this, "打卡成功", QString::fromStdString(habit.name));
            }
            else
            {
                QMessageBox::information(this, "打卡失败", QString::fromStdString(habit.name));
            }
        });
        connect(disableBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.inactiveHabit(habit.habit_id))
            {
                QMessageBox::information(this, "停用成功", QString::fromStdString(habit.name));
                initHabitManageView();  // 重新刷新
            }
            else
            {
                QMessageBox::warning(this, "停用失败", QString::fromStdString(habit.name));
            }
        });

        buttonLayout->addWidget(modifyBtn);
        buttonLayout->addWidget(deleteBtn);
        buttonLayout->addWidget(checkinBtn);
        buttonLayout->addWidget(disableBtn);

        cardLayout->addWidget(nameLabel);
        cardLayout->addWidget(countLabel);
        cardLayout->addWidget(startLabel);
        cardLayout->addWidget(endLabel);
        cardLayout->addLayout(buttonLayout);

        activeRow = i / habitsPerRow;
        activeCol = i % habitsPerRow;
        activeHabitGridLayout->addWidget(habitCard, activeRow, activeCol);
    }

    // 添加习惯按钮
    QPushButton *addHabitButton = new QPushButton();
    addHabitButton->setIcon(QIcon(":/assets/images/add.png"));
    addHabitButton->setIconSize(QSize(36, 36));
    addHabitButton->setFixedSize(180, 150);

    connect(addHabitButton, &QPushButton::clicked, this, [this]()
    {
        habitInsertView();  // 弹出添加弹窗
    });

    // 计算添加按钮的位置
    int addRow = i / habitsPerRow;
    int addCol = i % habitsPerRow;

    // 将添加按钮添加到活跃习惯展示区
    activeHabitGridLayout->addWidget(addHabitButton, addRow, addCol);

    activeHabitListContainer->setLayout(activeHabitGridLayout);
    activeScrollArea->setWidget(activeHabitListContainer);
    activeScrollArea->setWidgetResizable(true);
    gridLayout->addWidget(activeScrollArea, 1, 0, 1, 4);  // 占据第1行，4列

    // 不活跃习惯展示区（滚动区域）
    QScrollArea *inactiveScrollArea = new QScrollArea(habit_manage_widget);
    QWidget *inactiveHabitListContainer = new QWidget();
    QGridLayout *inactiveHabitGridLayout = new QGridLayout(inactiveHabitListContainer);

    inactiveHabitGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    std::vector<Habit> inactiveHabits = sv_Layer.getInactiveHabits();

    int inactiveRow = 0, inactiveCol = 0;
    for (size_t i = 0; i < inactiveHabits.size(); ++i)
    {
        const Habit &habit = inactiveHabits[i];
        QWidget *habitCard = new QWidget();
        habitCard->setFixedSize(180, 150);
        habitCard->setStyleSheet
        (
            "background-color: #fefefe;"
            "border: 1px solid #cccccc;"
            "padding: 1px;"
            "margin: 1px;"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(habitCard);

        QLabel *nameLabel = new QLabel(QString::fromStdString("名称: " + habit.name));
        QLabel *countLabel = new QLabel(QString("目标: %1").arg(QString::number(habit.target_count)));
        QLabel *startLabel = new QLabel(QString::fromStdString("开始: " + toString(habit.start_date)));
        QLabel *endLabel = new QLabel(QString::fromStdString("结束: " + toString(habit.end_date)));

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *modifyBtn = createButton(":/assets/images/modify.png", "修改习惯", 40, 22);;
        QPushButton *deleteBtn = createButton(":/assets/images/delete.png", "删除习惯", 40, 22);;
        QPushButton *enableBtn = createButton(":/assets/images/active.png", "启用习惯", 40, 22);;

        connect(modifyBtn, &QPushButton::clicked, [this, habit]()
        {
            habitUpdateView(habit);
        });
        connect(deleteBtn, &QPushButton::clicked, [this, habit]()
        {
            if (QMessageBox::question(this, "确认删除", "确定删除该习惯吗？") == QMessageBox::Yes)
            {
                if (sv_Layer.deleteHabit(habit.habit_id))
                {
                    QMessageBox::information(this, "提示", "删除成功");
                    initHabitManageView();  // 重新刷新
                }
                else
                {
                    QMessageBox::warning(this, "错误", "删除失败");
                }
            }
        });
        connect(enableBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.activeHabit(habit.habit_id))
            {
                QMessageBox::information(this, "启用成功", QString::fromStdString(habit.name));
                initHabitManageView();  // 重新刷新
            }
            else
            {
                QMessageBox::warning(this, "启用失败", QString::fromStdString(habit.name));
            }
});

        buttonLayout->addWidget(modifyBtn);
        buttonLayout->addWidget(deleteBtn);
        buttonLayout->addWidget(enableBtn);

        cardLayout->addWidget(nameLabel);
        cardLayout->addWidget(countLabel);
        cardLayout->addWidget(startLabel);
        cardLayout->addWidget(endLabel);
        cardLayout->addLayout(buttonLayout);

        inactiveRow = i / habitsPerRow;
        inactiveCol = i % habitsPerRow;
        inactiveHabitGridLayout->addWidget(habitCard, inactiveRow, inactiveCol);
    }

    inactiveHabitListContainer->setLayout(inactiveHabitGridLayout);
    inactiveScrollArea->setWidget(inactiveHabitListContainer);
    inactiveScrollArea->setWidgetResizable(true);
    gridLayout->addWidget(inactiveScrollArea, 2, 0, 1, 4);  // 占据第2行，4列
}

void ViewLayer::initNavigationView() {
    const auto navigation_bar = dynamic_cast<NavigationBar*>(navigation_widget);
    // 连接导航栏信号
    connect(navigation_bar, &NavigationBar::buttonClicked,
            this, [this](const NavigationBar::NavButton button) {
        switch (button) {
            case NavigationBar::NavButton::MAIN_VIEW:
                setCurrentView(ViewType::MAIN_VIEW); break;
            case NavigationBar::NavButton::HABIT_MANAGE_VIEW:
                setCurrentView(ViewType::HABIT_MANAGE_VIEW); break;
            case NavigationBar::NavButton::EVENT_MANAGE_VIEW:
                setCurrentView(ViewType::EVENT_MANAGE_VIEW); break;
            case NavigationBar::NavButton::POMODORO_VIEW:
                setCurrentView(ViewType::POMODORO_VIEW); break;
            case NavigationBar::NavButton::TIMELINE_VIEW:
                setCurrentView(ViewType::TIMELINE_VIEW); break;
            case NavigationBar::NavButton::CALENDAR_VIEW:
                setCurrentView(ViewType::CALENDAR_VIEW); break;
            case NavigationBar::NavButton::SETTINGS_VIEW:
                setCurrentView(ViewType::SETTINGS_VIEW); break;
        }
    });

    // 加载导航栏的初始主题
    const QString current_theme = sv_Layer.getCurrentThemeName();
    const QJsonObject theme_config = sv_Layer.getThemeConfig(current_theme);
    navigation_bar->loadTheme(theme_config);
}

void ViewLayer::onBackToNavigation()
{
    setCurrentView(ViewType::MAIN_VIEW);
    initMainView();
}

void ViewLayer::onAddEventClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新建事项", "请输入事项名称：", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty())
    {
        QMessageBox::warning(this, "错误", "事项名称不能为空！");
        return;
    }
    event_name_input = name.toStdString();

    QString dateStr = QInputDialog::getText(this, "新建事项", "请输入事项到期日期 (YYYY-MM-DD)：", QLineEdit::Normal, "", &ok);
    if (!ok || !parseDate(dateStr.toStdString(), start_date_input))
    {
        QMessageBox::warning(this, "错误", "日期格式不正确！");
        return;
    }

    QString timeStr = QInputDialog::getText(this, "新建事项", "请输入事项到期时间 (HH:MM:SS)：", QLineEdit::Normal, "", &ok);
    if (!ok || !parseTime(timeStr.toStdString(), event_time_input))
    {
        QMessageBox::warning(this, "错误", "时间格式不正确！");
        return;
    }

    QString remindFlagStr = QInputDialog::getText(this, "新建事项", "是否开启提醒？（1: 是, 0: 否）", QLineEdit::Normal, "0", &ok);
    if (!ok)
        return;

    bool remindFlag = (remindFlagStr.trimmed() == "1");
    Time remindTime(std::chrono::seconds(0));
    if (remindFlag)
    {
        QString remindTimeStr = QInputDialog::getText(this, "新建事项", "请输入提前提醒时间 (HH:MM:SS)：", QLineEdit::Normal, "", &ok);
        if (!ok || !parseTime(remindTimeStr.toStdString(), remindTime))
        {
            QMessageBox::warning(this, "错误", "提醒时间格式不正确！");
            return;
        }
    }

    // 插入事项
    sv_Layer.insertEvent(event_name_input, start_date_input, event_time_input, remindFlag, remindTime);

    emit eventAdded();
    initEventManageView(); // 刷新界面
}

void ViewLayer::onAddHabitClicked()
{
    if (habit_name_input.empty())
    {
        QMessageBox::warning(this, "错误", "请输入习惯名称！");
        return;
    }
    if (sv_Layer.insertHabit(habit_name_input, start_date_input, end_date_input, habit_target_count_input))
    {
        emit habitAdded();
        QMessageBox::information(this, "成功", "添加习惯成功！");
    }
    else
    {
        QMessageBox::information(this, "出错", "添加习惯失败！");
    }
}

void ViewLayer::setCurrentView(ViewType view)
{
    if (cur_view_type == view)
        return;

    this->resetCurrentView(view);
}

QDate ViewLayer::showCalendarDialog(const QDate& default_date) {
    CalendarDialog dialog(this);
    dialog.setSelectedDate(default_date);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedDate();
    }
    return default_date;
}

bool ViewLayer::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == dialogLabel && event->type() == QEvent::MouseButtonPress) {
        // 随机选一句
        int idx = QRandomGenerator::global()->bounded(dialogTexts.size());
        dialogLabel->setText(dialogTexts[idx]);
        return true; // 事件已处理
    }
    return QWidget::eventFilter(watched, event);
}

void ViewLayer::updateMainPomodoroDisplay()
{
    if (!pomodoro_widget_component) {
        showNoPomodoro();
        return;
    }

    // 检查番茄钟状态
    if (pomodoro_widget_component->getState() == PomodoroWidget::IDLE) {
        showNoPomodoro();
        return;
    }

    // 获取番茄钟信息
    QString remark = pomodoro_widget_component->getRemark();
    QString timeText = pomodoro_widget_component->getTimeDisplayText();

    // 更新显示
    if (main_pomodoro_remark_label && main_pomodoro_time_label && main_pomodoro_no_pomodoro_label) {
        main_pomodoro_remark_label->setText(remark);
        main_pomodoro_time_label->setText(timeText);

        main_pomodoro_remark_label->show();
        main_pomodoro_time_label->show();
        main_pomodoro_no_pomodoro_label->hide();
    }
}

void ViewLayer::startMainPomodoroTimer()
{
    if (main_pomodoro_timer) {
        main_pomodoro_timer->start();
    }
}

void ViewLayer::stopMainPomodoroTimer()
{
    if (main_pomodoro_timer) {
        main_pomodoro_timer->stop();
    }
}

void ViewLayer::checkAndRestorePomodoroState()
{
    qDebug() << "开始检查并恢复番茄钟状态...";

    // 检查是否有正在运行的番茄钟
    if (pomodoro_widget_component && pomodoro_widget_component->getState() != PomodoroWidget::IDLE) {
        qDebug() << "发现正在运行的番茄钟，启动主界面定时器";
        startMainPomodoroTimer();
        updateMainPomodoroDisplay();
        return;
    }

    // 尝试从数据库恢复番茄钟状态
    int state, total_seconds, remaining_seconds;
    std::string remark, start_time;

    qDebug() << "尝试从数据库加载番茄钟状态...";
    if (sv_Layer.loadPomodoroState(state, total_seconds, remaining_seconds, remark, start_time)) {
        qDebug() << "成功加载番茄钟状态:";
        qDebug() << "  状态:" << state;
        qDebug() << "  总时长:" << total_seconds;
        qDebug() << "  剩余时长:" << remaining_seconds;
        qDebug() << "  备注:" << QString::fromStdString(remark);
        qDebug() << "  开始时间:" << QString::fromStdString(start_time);

        // 如果有保存的状态，恢复番茄钟
        if (state != 0 && pomodoro_widget_component) { // 0 = IDLE
            qDebug() << "恢复番茄钟状态...";
            pomodoro_widget_component->restoreState(state, total_seconds, remaining_seconds,
                                                   QString::fromStdString(remark),
                                                   QString::fromStdString(start_time));
            startMainPomodoroTimer();
            updateMainPomodoroDisplay();
            qDebug() << "番茄钟状态恢复完成";
        } else {
            qDebug() << "状态为IDLE或番茄钟组件不存在，显示无番茄钟";
            showNoPomodoro();
        }
    } else {
        qDebug() << "没有找到保存的番茄钟状态，显示无番茄钟";
        showNoPomodoro();
    }
}

void ViewLayer::showNoPomodoro()
{
    if (main_pomodoro_remark_label && main_pomodoro_time_label && main_pomodoro_no_pomodoro_label) {
        main_pomodoro_remark_label->hide();
        main_pomodoro_time_label->hide();
        main_pomodoro_no_pomodoro_label->show();
    }
    stopMainPomodoroTimer();
}
