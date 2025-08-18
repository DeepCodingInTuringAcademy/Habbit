#include <QCheckBox>
#include <QDialogButtonBox>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGroupBox>
#include "ViewLayer.h"
#include <queue>
#include <QRadioButton>
#include "CalendarView.h"
#include "NavigationBar.h"
#include <qtimer.h>
#include <QInputDialog>
#include "Settings.h"

extern Settings g_settings;

ViewLayer::ViewLayer(QWidget *parent) : QWidget(parent),
                                        cur_view_type(ViewType::MAIN_VIEW)
{
    main_layout = new QVBoxLayout(this);

    init();
}

void ViewLayer::init()
{
    navigation_widget = new NavigationBar(this);
    initNavigationView();

    stacked_widget = new QStackedWidget(this);

    habit_manage_widget = new QWidget();
    event_manage_widget = new QWidget();
    pomodoro_widget = new QWidget();
    timeline_widget = new QWidget();
    calendar_widget = new QWidget();
    settings_widget = new QWidget();

    initHabitManageView();
    initEventManageView();
    initPomodoroView();
    initTimelineView();
    initCalendarView();
    initSettingsView();

    main_widget = new MainView(this->sv_Layer, *pomodoro_widget_component);

    // 添加到 stackedWidget 中
    stacked_widget->addWidget(main_widget);
    stacked_widget->addWidget(habit_manage_widget);
    stacked_widget->addWidget(event_manage_widget);
    stacked_widget->addWidget(pomodoro_widget);
    stacked_widget->addWidget(timeline_widget);
    stacked_widget->addWidget(calendar_widget);
    stacked_widget->addWidget(settings_widget);

    // 添加到主布局中
    main_layout->addWidget(stacked_widget);
    main_layout->addWidget(navigation_widget);

    setCurrentView(ViewType::MAIN_VIEW);
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

void ViewLayer::initEventManageView()
{
    if (!event_manage_widget->layout()) {
        auto layout = new QGridLayout();
        event_manage_widget->setLayout(layout);

        // 顶部标题 + 返回按钮
        QHBoxLayout *topLayout = new QHBoxLayout();
        QLabel *title = new QLabel("事项管理", event_manage_widget);
        QFont titleFont;
        titleFont.setPointSize(18);
        titleFont.setBold(true);
        title->setFont(titleFont);

        topLayout->addWidget(title);
        topLayout->addStretch();
        layout->addLayout(topLayout, 0, 0, 1, 4);

        // 初始化滚动区域（只做一次）
        auto scrollArea = new QScrollArea(event_manage_widget);
        scrollArea->setWidgetResizable(true);
        layout->addWidget(scrollArea, 1, 0, 1, 4);

        // 标记 scrollArea 用于刷新时访问（作为成员变量）
        event_scroll_area = scrollArea;
    }

    refreshEventManageView(); // 初始化后立即刷新内容
}

void ViewLayer::refreshEventManageView(){
    if (!event_scroll_area) return;

    // 重新创建内容容器
    QWidget *eventListContainer = new QWidget();
    QGridLayout *grid = new QGridLayout(eventListContainer);
    grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    std::vector<Event> events = sv_Layer.getActiveEvents();
    constexpr int eventsPerRow = 6;

    int i = 0;
    for (const auto& event : events)
    {
        QWidget *eventCard = new QWidget();
        eventCard->setFixedSize(150, 150);
        eventCard->setStyleSheet("background-color: #fefefe; border: 1px solid #cccccc; padding: 1px; margin: 1px;");

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
                if (sv_Layer.deleteEvent(event.event_id)) {
                    QMessageBox::information(this, "提示", "删除成功");
                    refreshEventManageView();  // 只刷新
                } else {
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
        ++i;
    }

    // 添加按钮
    QPushButton *addEventBtn = new QPushButton();
    addEventBtn->setIcon(QIcon(":/assets/images/add.png"));
    addEventBtn->setIconSize(QSize(36, 36));
    addEventBtn->setFixedSize(150, 150);
    connect(addEventBtn, &QPushButton::clicked, this, [this]() {
        eventInsertView();
    });

    int addRow = i / eventsPerRow;
    int addCol = i % eventsPerRow;
    grid->addWidget(addEventBtn, addRow, addCol);

    eventListContainer->setLayout(grid);
    event_scroll_area->setWidget(eventListContainer); // ✅ 替换旧内容
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
            refreshEventManageView();  // 重新刷新列表
        }
        else
        {
            QMessageBox::warning(this, "失败", "修改失败！");
        }
    }
}

void ViewLayer::initTimelineView()
{
    auto* layout = new QVBoxLayout(timeline_widget);

    // ===== 顶部标题栏 =====
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *title = new QLabel("时间线", timeline_widget);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);

    topLayout->addWidget(title);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    // ===== 日期选择栏 =====
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

    // ===== 滚动区域和内容部件 =====
    timeline_scroll_area = new QScrollArea(timeline_widget);
    timeline_scroll_area->setWidgetResizable(true);

    timeline_content_widget = new QWidget();
    timeline_layout = new QVBoxLayout(timeline_content_widget);
    timeline_layout->setAlignment(Qt::AlignTop);
    timeline_content_widget->setLayout(timeline_layout);

    timeline_scroll_area->setWidget(timeline_content_widget);
    layout->addWidget(timeline_scroll_area);

    // ===== 信号连接 =====
    connect(prevDayButton, &QPushButton::clicked, [this]() {
        dateEdit->setDate(dateEdit->date().addDays(-1));
        refreshTimeline();
    });

    connect(nextDayButton, &QPushButton::clicked, [this]() {
        dateEdit->setDate(dateEdit->date().addDays(1));
        refreshTimeline();
    });

    connect(dateEdit, &QDateEdit::dateChanged, this, &ViewLayer::refreshTimeline);

    // 初次加载
    refreshTimeline();
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

    // 清空 timeline_layout
    QLayoutItem* item;
    while ((item = timeline_layout->takeAt(0)) != nullptr)
    {
        if (item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // 当前日期
    const auto qdate = dateEdit->date();
    Date date = Date{std::chrono::year{qdate.year()} / qdate.month() / qdate.day()};

    // 获取数据
    DateRecord raw_record = sv_Layer.getAllRecordsByDate(date);

    using TimelineItem = std::tuple<Time, std::string, std::string>;
    std::vector<TimelineItem> timeline_items;

    // 插入记录
    for (const auto& [time, habit] : raw_record.habit_records)
        timeline_items.emplace_back(time, "习惯打卡", habit.name);

    for (const auto& [time, pomo] : raw_record.pomodoro_records)
        timeline_items.emplace_back(time, pomo.record, "番茄钟专注 " + toString(pomo.pomodoro_time));

    for (const auto& [time, event] : raw_record.event_records)
        timeline_items.emplace_back(time, "事项", event.title);

    // 排序
    std::sort(timeline_items.begin(), timeline_items.end(),
              [](const TimelineItem &a, const TimelineItem &b) {
                  return std::get<0>(a) < std::get<0>(b);
              });

    // 添加标签到布局
    for (const auto& [time, type, content] : timeline_items) {
        QString display = QString("%1 - %2 - %3")
                .arg(QString::fromStdString(toString(time)))
                .arg(QString::fromStdString(type))
                .arg(QString::fromStdString(content));
        QLabel *label = new QLabel(display);
        label->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 5px;");
        timeline_layout->addWidget(label);
    }
}

void ViewLayer::initPomodoroView()
{
    if (!pomodoro_widget->layout()) {
        pomodoro_main_layout = new QVBoxLayout(pomodoro_widget);
        pomodoro_widget_component = new PomodoroWidget(pomodoro_widget);
        // 顶部栏
        QHBoxLayout* topLayout = new QHBoxLayout();
        QLabel* title = new QLabel("番茄钟", pomodoro_widget);
        QFont titleFont;
        titleFont.setPointSize(18);
        titleFont.setBold(true);
        title->setFont(titleFont);

        topLayout->addWidget(title);
        topLayout->addStretch();
        pomodoro_main_layout->addLayout(topLayout);
        auto pomo_layout = new QHBoxLayout();
        pomo_layout->addWidget(pomodoro_widget_component);
        pomo_layout->setAlignment(Qt::AlignCenter);
        pomodoro_main_layout->addLayout(pomo_layout);

        // 中间展示区（ScrollArea，便于多条记录滚动查看）
        pomodoro_scroll_area = new QScrollArea(pomodoro_widget);
        pomodoro_scroll_area->setWidgetResizable(true);
        pomodoro_main_layout->addWidget(pomodoro_scroll_area);

        connect(pomodoro_widget_component, &PomodoroWidget::timerUpdated, this, [this]
        {
            main_widget->refreshPomodoro();
        });

        connect(pomodoro_widget_component, &PomodoroWidget::stateChanged, this, [this]
        {
            main_widget->refreshPomodoro();
        });
    }

    refreshPomodoroView();  // 初始化后第一次刷新
}

void ViewLayer::refreshPomodoroView()
{
    if (!pomodoro_scroll_area) return;

    // 获取今日番茄钟记录
    Date today = std::chrono::year_month_day(
            std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
    DateRecord todayRecord = sv_Layer.getAllRecordsByDate(today);

    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignTop);

    if (todayRecord.pomodoro_records.empty()) {
        QLabel* noDataLabel = new QLabel("暂无番茄钟记录");
        noDataLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(noDataLabel);
    } else {
        layout->addStretch();
        for (const auto& pair : todayRecord.pomodoro_records) {
            const Pomodoro& pomo = pair.second;
            PomodoroWidget* widget = new PomodoroWidget(pomo, container);
            layout->addWidget(widget);
        }
        layout->addStretch();
    }

    container->setLayout(layout);
    pomodoro_scroll_area->setWidget(container);
}

void ViewLayer::initSettingsView()
{
    if (!settings_widget)
    {
        settings_widget = new QWidget(this);
    }

    QVBoxLayout *layout = new QVBoxLayout(settings_widget);

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

    // 如果已登录，显示用户信息
    if (g_settings.getUserID() != 0)
    {
        QLabel *nicknameLabel = new QLabel(QString("当前昵称：%1").arg(
                                               QString::fromStdString(g_settings.getUserNickname())),
                                           settings_widget);
        layout->addWidget(nicknameLabel);
    }

    // 添加分隔线
    QFrame *line = new QFrame(settings_widget);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    // 下方嵌入认证系统
    if (!authSystem)
    {
        authSystem = new SimpleAuthSystem(g_settings, settings_widget);
    }

    layout->addWidget(authSystem);
}

void ViewLayer::initCalendarView()
{
    if (calendar_widget->layout()) return;

    auto* layout = new QVBoxLayout(calendar_widget);

    // ===== 顶部标题栏 =====
    auto* top_layout = new QHBoxLayout();
    auto* title = new QLabel("日历", calendar_widget);
    title->setFont(QFont("Arial", 18, QFont::Bold));

    top_layout->addWidget(title);
    top_layout->addStretch();
    layout->addLayout(top_layout);

    // ===== CalendarView 部件 =====
    calendar_view = new CalendarView(&sv_Layer, calendar_widget);

    connect(calendar_view, &CalendarView::dateClicked, this, [this](const QDate& date) {
        qDebug() << "点击了日期:" << date;
    });

    layout->addWidget(calendar_view);

    refreshCalendarView();
}

void ViewLayer::refreshCalendarView()
{
    if (calendar_view) {
        calendar_view->refreshCalendar();
    }
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
    if (!habit_manage_widget->layout()) {
        auto gridLayout = new QGridLayout();
        habit_manage_widget->setLayout(gridLayout);

        // 标题 + 返回
        QHBoxLayout *topLayout = new QHBoxLayout();
        QLabel *title = new QLabel("习惯管理", habit_manage_widget);
        QFont titleFont;
        titleFont.setPointSize(18);
        titleFont.setBold(true);
        title->setFont(titleFont);

        topLayout->addWidget(title);
        topLayout->addStretch();
        gridLayout->addLayout(topLayout, 0, 0, 1, 4);

        // === 已启用习惯 标签 ===
        QLabel *activeTitle = new QLabel("已启用习惯", habit_manage_widget);
        QFont sectionFont;
        sectionFont.setPointSize(14);
        sectionFont.setBold(true);
        activeTitle->setFont(sectionFont);
        gridLayout->addWidget(activeTitle, 1, 0, 1, 4);

        // 初始化滚动区域：启用习惯
        activeHabitScrollArea = new QScrollArea(habit_manage_widget);
        activeHabitScrollArea->setWidgetResizable(true);
        gridLayout->addWidget(activeHabitScrollArea, 2, 0, 1, 4);

        // === 已停用习惯 标签 ===
        QLabel *inactiveTitle = new QLabel("已停用习惯", habit_manage_widget);
        inactiveTitle->setFont(sectionFont);
        gridLayout->addWidget(inactiveTitle, 3, 0, 1, 4);

        // 初始化滚动区域：停用习惯
        inactiveHabitScrollArea = new QScrollArea(habit_manage_widget);
        inactiveHabitScrollArea->setWidgetResizable(true);
        gridLayout->addWidget(inactiveHabitScrollArea, 4, 0, 1, 4);
    }

    refreshHabitManageView(); // 初始化后立即刷新内容
}

void ViewLayer::refreshHabitManageView()
{
    if (!activeHabitScrollArea || !inactiveHabitScrollArea) return;

    // ====== 活跃习惯 ======
    QWidget* activeContainer = new QWidget();
    QGridLayout* activeLayout = new QGridLayout(activeContainer);
    activeLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    const std::vector<Habit> activeHabits = sv_Layer.getActiveHabits();
    constexpr int habitsPerRow = 5;
    int i = 0;

    for (const Habit& habit : activeHabits)
    {
        QWidget* card = createHabitCard(habit, true);
        int row = i / habitsPerRow;
        int col = i % habitsPerRow;
        activeLayout->addWidget(card, row, col);
        ++i;
    }

    // 添加习惯按钮
    QPushButton* addHabitButton = createButton(":/assets/images/add.png", "添加习惯", 36, 36);
    addHabitButton->setFixedSize(180, 150);
    connect(addHabitButton, &QPushButton::clicked, this, [this]() {
        habitInsertView();
    });
    int addRow = i / habitsPerRow;
    int addCol = i % habitsPerRow;
    activeLayout->addWidget(addHabitButton, addRow, addCol);

    activeContainer->setLayout(activeLayout);
    activeHabitScrollArea->setWidget(activeContainer);

    // ====== 非活跃习惯 ======
    QWidget* inactiveContainer = new QWidget();
    QGridLayout* inactiveLayout = new QGridLayout(inactiveContainer);
    inactiveLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    const std::vector<Habit> inactiveHabits = sv_Layer.getInactiveHabits();
    int j = 0;

    for (const Habit& habit : inactiveHabits)
    {
        QWidget* card = createHabitCard(habit, false);
        int row = j / habitsPerRow;
        int col = j % habitsPerRow;
        inactiveLayout->addWidget(card, row, col);
        ++j;
    }

    inactiveContainer->setLayout(inactiveLayout);
    inactiveHabitScrollArea->setWidget(inactiveContainer);
}

QWidget* ViewLayer::createHabitCard(const Habit& habit, bool active)
{
    QWidget* card = new QWidget();
    card->setFixedSize(180, 150);
    card->setStyleSheet("background-color: #fefefe; border: 1px solid #cccccc; padding: 1px; margin: 1px;");

    QVBoxLayout* layout = new QVBoxLayout(card);

    layout->addWidget(new QLabel("名称: " + QString::fromStdString(habit.name)));
    layout->addWidget(new QLabel("目标: " + QString::number(habit.target_count)));
    layout->addWidget(new QLabel("开始: " + QString::fromStdString(toString(habit.start_date))));
    layout->addWidget(new QLabel("结束: " + QString::fromStdString(toString(habit.end_date))));

    QHBoxLayout* btnLayout = new QHBoxLayout();

    QPushButton* modifyBtn = createButton(":/assets/images/modify.png", "修改", 35, 20);
    QPushButton* deleteBtn = createButton(":/assets/images/delete.png", "删除", 40, 22);
    connect(modifyBtn, &QPushButton::clicked, [this, habit]() {
        habitUpdateView(habit);
    });
    connect(deleteBtn, &QPushButton::clicked, [this, habit]() {
        if (QMessageBox::question(this, "确认删除", "确定删除该习惯吗？") == QMessageBox::Yes) {
            if (sv_Layer.deleteHabit(habit.habit_id)) {
                QMessageBox::information(this, "提示", "删除成功");
                refreshHabitManageView(); // 只刷新
            } else {
                QMessageBox::warning(this, "错误", "删除失败");
            }
        }
    });

    btnLayout->addWidget(modifyBtn);
    btnLayout->addWidget(deleteBtn);

    if (active) {
        QPushButton* checkinBtn = createButton(":/assets/images/check.png", "打卡", 40, 22);
        QPushButton* disableBtn = createButton(":/assets/images/inactive.png", "停用", 40, 22);
        connect(checkinBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.checkinHabit(habit)) {
                QMessageBox::information(this, "打卡成功", QString::fromStdString(habit.name));
            } else {
                QMessageBox::warning(this, "打卡失败", QString::fromStdString(habit.name));
            }
        });
        connect(disableBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.inactiveHabit(habit.habit_id)) {
                QMessageBox::information(this, "停用成功", QString::fromStdString(habit.name));
                refreshHabitManageView();
            } else {
                QMessageBox::warning(this, "停用失败", QString::fromStdString(habit.name));
            }
        });
        btnLayout->addWidget(checkinBtn);
        btnLayout->addWidget(disableBtn);
    } else {
        QPushButton* enableBtn = createButton(":/assets/images/active.png", "启用", 40, 22);
        connect(enableBtn, &QPushButton::clicked, [this, habit]() {
            if (sv_Layer.activeHabit(habit.habit_id)) {
                QMessageBox::information(this, "启用成功", QString::fromStdString(habit.name));
                refreshHabitManageView();
            } else {
                QMessageBox::warning(this, "启用失败", QString::fromStdString(habit.name));
            }
        });
        btnLayout->addWidget(enableBtn);
    }

    layout->addLayout(btnLayout);
    return card;
}

void ViewLayer::initNavigationView() {
    // 连接导航栏信号
    connect(navigation_widget, &NavigationBar::buttonClicked,
            this, [this](const NavigationBar::NavButton button) {
        switch (button) {
            case NavigationBar::NavButton::MAIN_VIEW:
                main_widget->refreshAll();
                stacked_widget->setCurrentWidget(main_widget);
                break;
            case NavigationBar::NavButton::HABIT_MANAGE_VIEW:
                refreshHabitManageView();
                stacked_widget->setCurrentWidget(habit_manage_widget);
                break;
            case NavigationBar::NavButton::EVENT_MANAGE_VIEW:
                refreshEventManageView();
                stacked_widget->setCurrentWidget(event_manage_widget);
                break;
            case NavigationBar::NavButton::POMODORO_VIEW:
                refreshPomodoroView();
                stacked_widget->setCurrentWidget(pomodoro_widget);
                break;
            case NavigationBar::NavButton::TIMELINE_VIEW:
                refreshTimeline();
                stacked_widget->setCurrentWidget(timeline_widget);
                break;
            case NavigationBar::NavButton::CALENDAR_VIEW:
                refreshCalendarView();
                stacked_widget->setCurrentWidget(calendar_widget);
                break;
            case NavigationBar::NavButton::SETTINGS_VIEW:
                stacked_widget->setCurrentWidget(settings_widget);
                break;
        }
    });

    // 加载导航栏的初始主题
    const QString current_theme = sv_Layer.getCurrentThemeName();
    const QJsonObject theme_config = sv_Layer.getThemeConfig(current_theme);
    navigation_widget->loadTheme(theme_config);
}

void ViewLayer::onBackToNavigation()
{
    navigation_widget->setCurrentButton(NavigationBar::NavButton::MAIN_VIEW);
    main_widget->refreshAll();
    stacked_widget->setCurrentWidget(main_widget);
}

void ViewLayer::setCurrentView(ViewType view)
{
    if (cur_view_type == view)
        return;

    cur_view_type = view;
    switch (view) {
        case ViewType::MAIN_VIEW:
            main_widget->refreshAll();
            stacked_widget->setCurrentWidget(main_widget);
            break;
        case ViewType::HABIT_MANAGE_VIEW:
            initHabitManageView();
            stacked_widget->setCurrentWidget(habit_manage_widget);
            break;
        case ViewType::EVENT_MANAGE_VIEW:
            initEventManageView();
            stacked_widget->setCurrentWidget(event_manage_widget);
            break;
        case ViewType::POMODORO_VIEW:
            initPomodoroView();
            stacked_widget->setCurrentWidget(pomodoro_widget);
            break;
        case ViewType::TIMELINE_VIEW:
            initTimelineView();
            stacked_widget->setCurrentWidget(timeline_widget);
            break;
        case ViewType::CALENDAR_VIEW:
            initCalendarView();
            stacked_widget->setCurrentWidget(calendar_widget);
            break;
        case ViewType::SETTINGS_VIEW:
            initSettingsView();
            stacked_widget->setCurrentWidget(settings_widget);
            break;
    }
}
