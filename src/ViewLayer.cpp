#include "ViewLayer.h"


ViewLayer::ViewLayer(QWidget *parent)
{

}

void ViewLayer::init()
{
    sv_Layer.init();
    setcurrentView(ViewType::NAVIGATION_VIEW);
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

}

void ViewLayer::initNavigationView()
{

}

void ViewLayer::onBackToNavigation()
{

}

void ViewLayer::onDeleteEventClicked()
{

}

void ViewLayer::onAddEventClicked()
{

}

void ViewLayer::onDeleteHabitClicked()
{

}

void ViewLayer::onAddHabitClicked()
{

}

void ViewLayer::setCurrentView(ViewType view)
{
    // 当前视图已显示，避免重复刷新
    if (cur_view_type == view)
        return;

    // 清空主布局中的旧视图
    QLayoutItem* item;
    while ((item = main_layout->takeAt(0)) != nullptr)
    {
        if (QWidget* w = item->widget())
        {
            // 保留 widget 对象
            w->setParent(nullptr);
        }
        // 删除布局项（layout item）
        delete item;
    }

    cur_view_type = view;

    // 添加新的视图部件
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
