#include "ViewLayer.h"
#include "utility.h"


ViewLayer::ViewLayer(QWidget *parent) {

}

void ViewLayer::init() {

}

bool ViewLayer::parseTime(const std::string &str, Time &result) {
    const auto split_res = utility::split(str, ':');
    if (split_res.size() != 3) return false;

    try {
        auto [hh_str, mm_str, ss_str] = std::tie(split_res[0], split_res[1], split_res[2]);
        const int hh = std::stoi(hh_str);
        const int mm = std::stoi(mm_str);
        const int ss = std::stoi(ss_str);

        if (hh < 0 || hh >= 24 || mm < 0 || mm >= 60 || ss < 0 || ss >= 60)
            return false;

        const std::chrono::seconds total = std::chrono::hours{hh} + std::chrono::minutes{mm} + std::chrono::seconds{ss};
        result = Time{total};
        return true;
    } catch (...) {
        return false;
    }
}

void ViewLayer::showView(QWidget *view) {

}

bool ViewLayer::parseDate(const std::string &str, Date &result) {
    const auto split_res = utility::split(str, '-');
    if (split_res.size() != 3) return false;

    try
    {
        auto [year_str, month_str, day_str] = std::tie(split_res[0], split_res[1], split_res[2]);
        const int year = std::stoi(year_str);
        const unsigned month = std::stoi(month_str);
        const unsigned day = std::stoi(day_str);

        if (year < 1900 || year >= 2100 || month < 1 || month >= 12 || day < 1 || day >= 31) {
            return false;
        }

        const std::chrono::year_month_day res = std::chrono::year{year} / std::chrono::month{month} / std::chrono::day{day};
        if (!res.ok())
        {
            return false;
        }
        result = res;
    }
    catch (...) {
        return false;
    }
    return true;
}

void ViewLayer::clearLayout(QLayout *layout) {

}

void ViewLayer::initEventManageView() {

}

void ViewLayer::initPomodoroView()
{

}

void ViewLayer::initHabitManageView() {

}

void ViewLayer::initNavigationView() {

}

void ViewLayer::onBackToNavigation() {

}

void ViewLayer::onDeleteEventClicked() {

}

void ViewLayer::onAddEventClicked() {

}

void ViewLayer::onDeleteHabitClicked() {

}

void ViewLayer::onAddHabitClicked() {

}

void ViewLayer::setcurrentView(ViewType view) {

}
