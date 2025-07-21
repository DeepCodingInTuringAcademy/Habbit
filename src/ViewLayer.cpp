#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>

#include "ViewLayer.h"

ViewLayer::ViewLayer(QWidget *parent) : QWidget(parent),
                                        cur_view_type(ViewType::NAVIGATION_VIEW)
{
    main_layout = new QVBoxLayout(this);

    init();
}

void ViewLayer::init()
{
    navigation_widget = new QWidget(this);
    habit_manage_widget = new QWidget(this);
    event_manage_widget = new QWidget(this);
    pomodoro_widget = new QWidget(this);
    timeline_widget = new QWidget(this);

    initNavigationView();
    initEventManageView();
    initHabitManageView();
    initPomodoroView();
    initTimelineView();

    resetCurrentView(ViewType::NAVIGATION_VIEW);
}

void ViewLayer::resetCurrentView(ViewType view)
{
    // 隐藏所有视图
    navigation_widget->hide();
    habit_manage_widget->hide();
    event_manage_widget->hide();
    pomodoro_widget->hide();
    timeline_widget->hide();

    // 清空主布局
    clearLayout(main_layout);

    cur_view_type = view;

    switch (view)
    {
    case ViewType::NAVIGATION_VIEW:
        main_layout->addWidget(navigation_widget);
        navigation_widget->show();
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
        main_layout->addWidget(new QLabel("日历 - TODO", this));
        break;
    default:
        main_layout->addWidget(new QLabel("待开发的视图", this));
        break;
    }
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

        if (year < 1900 || year >= 2100 || month < 1 || month >= 12 || day < 1 || day >= 31)
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
    {
        event_manage_widget = new QWidget(this);
    }

    // 设置事项管理视图的布局
    auto layout = new QVBoxLayout(event_manage_widget);

    // 添加标题
    auto *title = new QLabel("事项管理", event_manage_widget);
    layout->addWidget(title);

    // 事项名称输入框
    auto *name_input = new QLineEdit(event_manage_widget);
    name_input->setPlaceholderText("请输入事项名称");
    layout->addWidget(name_input);

    // 事项日期输入框
    auto *date_input = new QLineEdit(event_manage_widget);
    date_input->setPlaceholderText("请输入事项日期 (yyyy-mm-dd)");
    layout->addWidget(date_input);

    // 事项时间输入框
    auto *time_input = new QLineEdit(event_manage_widget);
    time_input->setPlaceholderText("请输入事项时间 (hh:mm:ss)");
    layout->addWidget(time_input);

    // 是否提醒复选框
    auto *remind_checkbox = new QCheckBox("开启提醒", event_manage_widget);
    layout->addWidget(remind_checkbox);

    // 提醒时间输入框
    auto *remind_time_input = new QLineEdit(event_manage_widget);
    remind_time_input->setPlaceholderText("请输入提醒时间 (hh:mm:ss)");
    layout->addWidget(remind_time_input);

    // 添加事项按钮
    auto *add_event_button = new QPushButton("添加事项", event_manage_widget);
    connect(add_event_button, &QPushButton::clicked, [=, this]
    {
        std::string event_name = name_input->text().toStdString();
        std::string event_date_str = date_input->text().toStdString();
        std::string event_time_str = time_input->text().toStdString();
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
            emit eventAdded();
        }
        else
        {
            QMessageBox::warning(this, "错误", "事项添加失败，请检查输入！");
        }
    });
    layout->addWidget(add_event_button);

    // 删除事项按钮
    auto *delete_event_button = new QPushButton("删除事项", event_manage_widget);
    connect(delete_event_button, &QPushButton::clicked, [=, this]
    {
        bool ok;
        QString event_id_str = QInputDialog::getText(this, "删除事项", "请输入要删除的事项ID：", QLineEdit::Normal, "", &ok);
        if (ok && !event_id_str.isEmpty())
        {
            try
            {
                std::size_t event_id = std::stoul(event_id_str.toStdString());
                if (sv_Layer.deleteEvent(event_id))
                {
                    QMessageBox::information(this, "提示", "事项删除成功！");
                    emit eventDeleted(sv_Layer.getEventByID(event_id));
                }
                else
                {
                    QMessageBox::warning(this, "错误", "事项删除失败，请检查ID！");
                }
            }
            catch (...)
            {
                QMessageBox::warning(this, "错误", "输入的ID格式不正确！");
            }
        }
    });
    layout->addWidget(delete_event_button);

    // 添加返回导航按钮
    const auto backButton = new QPushButton("返回主页", event_manage_widget);
    connect(backButton, &QPushButton::clicked, this, &ViewLayer::onBackToNavigation);
    layout->addWidget(backButton);

    // 将事项管理视图部件添加到主布局中
    main_layout->addWidget(event_manage_widget);
}

void ViewLayer::initTimelineView()
{
    const auto layout = new QVBoxLayout(timeline_widget);
    auto *title = new QLabel("时间线", timeline_widget);
    layout->addWidget(title);// 添加标题

    // 添加返回导航按钮
    const auto backButton = new QPushButton("返回主页", timeline_widget);
    connect(backButton, &QPushButton::clicked, this, &ViewLayer::onBackToNavigation);
    layout->addWidget(backButton);
}

void ViewLayer::initPomodoroView()
{
    if (!pomodoro_widget)
    {
        pomodoro_widget = new QWidget(this);
    }

    if (!pomodoro_widget_component)
    {
        pomodoro_widget_component = new PomodoroWidget(pomodoro_widget);
    }

    const auto layout = new QVBoxLayout(pomodoro_widget);
    layout->addWidget(pomodoro_widget_component);

    auto *title = new QLabel("番茄钟", pomodoro_widget);
    layout->addWidget(title);// 添加标题

    // 添加返回导航按钮
    const auto backButton = new QPushButton("返回主页", pomodoro_widget);
    connect(backButton, &QPushButton::clicked, this, &ViewLayer::onBackToNavigation);
    layout->addWidget(backButton);
}

void ViewLayer::habitInsertView()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建习惯");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("请输入习惯名称");

    QLineEdit *startEdit = new QLineEdit();
    startEdit->setPlaceholderText("请输入开始日期（yyyy-mm-dd）");

    QLineEdit *endEdit = new QLineEdit();
    endEdit->setPlaceholderText("请输入结束日期（yyyy-mm-dd）");

    QSpinBox *targetCountSpin = new QSpinBox();
    targetCountSpin->setRange(1, 1000);
    targetCountSpin->setPrefix("每日目标次数：");

    layout->addWidget(nameEdit);
    layout->addWidget(startEdit);
    layout->addWidget(endEdit);
    layout->addWidget(targetCountSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string name = nameEdit->text().toStdString();
        Date start, end;
        if (!parseDate(startEdit->text().toStdString(), start) || !parseDate(endEdit->text().toStdString(), end))
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

    QLineEdit *startEdit = new QLineEdit(QString::fromStdString(toString(habit.start_date)));
    QLineEdit *endEdit = new QLineEdit(QString::fromStdString(toString(habit.end_date)));

    QSpinBox *targetCountSpin = new QSpinBox();
    targetCountSpin->setRange(1, 1000);
    targetCountSpin->setValue(habit.target_count);
    targetCountSpin->setPrefix("每日目标次数：");

    layout->addWidget(nameEdit);
    layout->addWidget(startEdit);
    layout->addWidget(endEdit);
    layout->addWidget(targetCountSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string newName = nameEdit->text().toStdString();
        Date newStart, newEnd;
        parseDate(startEdit->text().toStdString(), newStart);
        parseDate(endEdit->text().toStdString(), newEnd);

        int newCount = targetCountSpin->value();
        if (sv_Layer.updateHabit(habit.habit_id, newStart, newEnd, newCount, true))
        {
            QMessageBox::information(this, "成功", "修改成功！");
            initHabitManageView();  // 重新刷新列表
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
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(habit_manage_widget->layout());
    if (!layout) {
        layout = new QVBoxLayout();
        habit_manage_widget->setLayout(layout);
    }

    // 顶部标题 + 返回按钮
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("习惯管理", habit_manage_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    QPushButton *backButton = new QPushButton("返回主页", habit_manage_widget);
    connect(backButton, &QPushButton::clicked, this, &ViewLayer::onBackToNavigation);
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(backButton);
    layout->addLayout(topLayout);

    // 习惯展示区（滚动区域）
    QScrollArea *scrollArea = new QScrollArea(habit_manage_widget);
    QWidget *habitListContainer = new QWidget();
    QVBoxLayout *habitListLayout = new QVBoxLayout(habitListContainer);

    std::vector<Habit> habits = sv_Layer.getActiveHabits();
    const int habitsPerRow = 4;
    QHBoxLayout *currentRowLayout = nullptr;

    for (size_t i = 0; i < habits.size(); ++i)
    {
        if (i % habitsPerRow == 0) {
            currentRowLayout = new QHBoxLayout();
            currentRowLayout->setSpacing(16);
            habitListLayout->addLayout(currentRowLayout);
        }

        const Habit &habit = habits[i];
        QWidget *habitCard = new QWidget();
        habitCard->setFixedSize(150, 150);
        habitCard->setStyleSheet
        (
            "background-color: #fefefe;"
            "border: 1px solid #cccccc;"
            "padding: 1px;"
            "margin: 1px;"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(habitCard);

        QLabel *nameLabel = new QLabel(QString::fromStdString("名称: " + habit.name));
        QLabel *countLabel = new QLabel(QString("目标: %1").arg(habit.target_count));
        QLabel *startLabel = new QLabel(QString::fromStdString("开始: " + toString(habit.start_date)));
        QLabel *endLabel = new QLabel(QString::fromStdString("结束: " + toString(habit.end_date)));

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *modifyBtn = new QPushButton("修改");
        QPushButton *deleteBtn = new QPushButton("删除");
        QPushButton *checkinBtn = new QPushButton("打卡");

        modifyBtn->setFixedSize(40, 22);
        deleteBtn->setFixedSize(40, 22);
        checkinBtn->setFixedSize(40, 22);

        connect(modifyBtn, &QPushButton::clicked, [this, habit]() {
            habitUpdateView(habit);
        });
        connect(deleteBtn, &QPushButton::clicked, [this, habit]() {
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
            if (sv_Layer.insertHabitRecord(habit))
            {
                QMessageBox::information(this, "打卡成功", QString::fromStdString(habit.name));
            }
            else
            {
                QMessageBox::information(this, "打卡失败", QString::fromStdString(habit.name));
            }
        });

        buttonLayout->addWidget(modifyBtn);
        buttonLayout->addWidget(deleteBtn);
        buttonLayout->addWidget(checkinBtn);

        cardLayout->addWidget(nameLabel);
        cardLayout->addWidget(countLabel);
        cardLayout->addWidget(startLabel);
        cardLayout->addWidget(endLabel);
        cardLayout->addLayout(buttonLayout);

        currentRowLayout->addWidget(habitCard);
    }

    habitListContainer->setLayout(habitListLayout);
    scrollArea->setWidget(habitListContainer);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);

    // 添加习惯按钮
    QPushButton *addHabitButton = new QPushButton("添加习惯", habit_manage_widget);
    addHabitButton->setFixedSize(120, 36);
    connect(addHabitButton, &QPushButton::clicked, this, [this]() {
        habitInsertView();  // 弹出添加弹窗
    });

    layout->addWidget(addHabitButton, 0, Qt::AlignCenter);
}

void ViewLayer::initNavigationView() {
    // 清空旧布局内容
    clearLayout(navigation_widget->layout());

    QVBoxLayout* nav_layout = new QVBoxLayout(navigation_widget);
    navigation_widget->setLayout(nav_layout);

    // 创建导航按钮
    QPushButton* habit_manage_button = new QPushButton("习惯管理", navigation_widget);
    QPushButton* event_manage_button = new QPushButton("事项管理", navigation_widget);
    QPushButton* pomodoro_button = new QPushButton("番茄钟", navigation_widget);
    QPushButton* timeline_button = new QPushButton("时间线", navigation_widget);
    QPushButton* calendar_button = new QPushButton("日历", navigation_widget);

    // 将按钮添加到布局中
    nav_layout->addWidget(habit_manage_button);
    nav_layout->addWidget(event_manage_button);
    nav_layout->addWidget(pomodoro_button);
    nav_layout->addWidget(timeline_button);
    nav_layout->addWidget(calendar_button);
    nav_layout->addStretch();

    // 连接按钮的点击信号
    connect(habit_manage_button, &QPushButton::clicked, [this]() {
        setCurrentView(ViewType::HABIT_MANAGE_VIEW);
    });

    connect(event_manage_button, &QPushButton::clicked, [this]()
    {
        setCurrentView(ViewType::EVENT_MANAGE_VIEW);
    });

    connect(pomodoro_button, &QPushButton::clicked, [this]()
    {
        setCurrentView(ViewType::POMODORO_VIEW);
    });

    connect(timeline_button, &QPushButton::clicked, [this]()
    {
        setCurrentView(ViewType::TIMELINE_VIEW);
    });

    connect(calendar_button, &QPushButton::clicked, [this]()
    {
        setCurrentView(ViewType::CALENDAR_VIEW);
    });

    auto *title = new QLabel("页面导航", navigation_widget);
    // 将导航视图部件添加到主布局中
    main_layout->addWidget(title);
    main_layout->addWidget(navigation_widget);
}

// 返回导航视图槽函数
void ViewLayer::onBackToNavigation()
{
    setCurrentView(ViewType::NAVIGATION_VIEW);
}

void ViewLayer::onDeleteEventClicked()
{
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

void ViewLayer::onDeleteHabitClicked()
{
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
