#include "ViewLayer.h"
#include "Times.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>


ViewLayer::ViewLayer(QWidget* parent) :
    QWidget(parent),
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

    initNavigationView();
    initEventManageView();
    initHabitManageView();
    initPomodoroView();

    sv_Layer.init();

    setCurrentView(ViewType::NAVIGATION_VIEW);
}

bool ViewLayer::parseTime(const std::string &str, Time &result)
{
    const auto split_res = Utility::split(str, ':');
    if (split_res.size() != 3) return false;

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

void ViewLayer::showView(QWidget *view)
{

}

bool ViewLayer::parseDate(const std::string &str, Date &result)
{
    const auto split_res = Utility::split(str, '-');
    if (split_res.size() != 3) return false;

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

}

void ViewLayer::initEventManageView()
{

}

void ViewLayer::initPomodoroView()
{

}

void ViewLayer::initHabitManageView()
{
    if (!habit_manage_widget)
    {
        habit_manage_widget = new QWidget(this);
    }

    // set habit manage_widget
    auto* layout = new QVBoxLayout(habit_manage_widget);
    auto* title = new QLabel("习惯管理", habit_manage_widget);

    auto* name_input = new QLineEdit(habit_manage_widget);
    name_input->setPlaceholderText("请输入习惯名称");

    auto* start_input = new QLineEdit(habit_manage_widget);
    auto* end_input = new QLineEdit(habit_manage_widget);
    start_input->setPlaceholderText("请输入起始日期(yyyy-mm-dd)");
    end_input->setPlaceholderText("请输入结束日期(yyyy-mm-dd)");
    parseDate(start_input->text().toStdString(), start_date_input);
    parseDate(end_input->text().toStdString(), end_date_input);

    auto* target_count_input = new QSpinBox(habit_manage_widget);
    target_count_input->setRange(1, 1000);
    target_count_input->setPrefix("每日目标次数: ");

    auto* add_habit_button = new QPushButton("添加习惯", habit_manage_widget);
    auto* del_habit_button = new QPushButton("删除习惯", habit_manage_widget);

    // 连接信号槽
    // lambda 捕获 name_input、target_count_input
    connect
            (
            add_habit_button,
            &QPushButton::clicked,
            this,
            [=]()
            {
                habit_name_input = name_input->text().toStdString();
                habit_target_count_input = target_count_input->value();
                onAddHabitClicked();
            }
            );

    connect
            (
            del_habit_button,
            &QPushButton::clicked,
            this,
            [=]()
            {
                habit_name_input = name_input->text().toStdString();
                onDeleteHabitClicked();
            }
            );

    layout->addWidget(title);
    layout->addWidget(name_input);
    layout->addWidget(start_input);
    layout->addWidget(end_input);
    layout->addWidget(target_count_input);
    layout->addWidget(add_habit_button);
    layout->addWidget(del_habit_button);
}

void ViewLayer::initNavigationView()
{

}

void ViewLayer::onBackToNavigation()
{

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

void ViewLayer::onDeleteHabitClicked()
{
    if (habit_name_input.empty())
    {
        QMessageBox::warning(this, "错误", "请输入要删除的习惯名称！");
        return;
    }

    Habit habit(0, 0, habit_name_input);
    sv_Layer.deleteHabit(0);
    emit habitDeleted(habit);
    QMessageBox::information(this, "成功", "删除习惯成功！");
}

void ViewLayer::onAddHabitClicked()
{
    if (habit_name_input.empty())
    {
        QMessageBox::warning(this, "错误", "请输入习惯名称！");
        return;
    }
    sv_Layer.insertHabit(habit_name_input, start_date_input, end_date_input, habit_target_count_input);
    emit habitAdded();
    QMessageBox::information(this, "成功", "添加习惯成功！");
}

void ViewLayer::setCurrentView(ViewType view)
{
    if (cur_view_type == view)
        return;

    QLayoutItem* item;
    while ((item = main_layout->takeAt(0)) != nullptr)
    {
        if (QWidget* w = item->widget())
        {
            w->setParent(nullptr);
        }
        delete item;
    }

    cur_view_type = view;

    switch (view)
    {
        case ViewType::NAVIGATION_VIEW:
            main_layout->addWidget(navigation_widget);
        break;
        case ViewType::HABIT_MANAGE_VIEW:
            main_layout->addWidget(habit_manage_widget);
        break;
        case ViewType::EVENT_MANAGE_VIEW:
            main_layout->addWidget(event_manage_widget);
        break;
        case ViewType::POMODORO_VIEW:
            main_layout->addWidget(new QLabel("番茄钟 - TODO", this));
        break;
        case ViewType::TIMELINE_VIEW:
            main_layout->addWidget(new QLabel("时间线 - TODO", this));
        break;
        case ViewType::CALENDAR_VIEW:
            main_layout->addWidget(new QLabel("日历 - TODO", this));
        break;
        default:
            main_layout->addWidget(new QLabel("待开发的视图", this));
        break;
    }
}
