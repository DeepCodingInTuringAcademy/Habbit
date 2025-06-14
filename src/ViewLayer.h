#ifndef VIEWLAYER_H
#define VIEWLAYER_H
#include "Times.h"
#include "Habit.h"
#include "Event.h"
#include "QWidget"
#include "ServiceLayer.h"
#include "QVBoxLayout"


class ViewLayer : public QWidget
{
    Q_OBJECT;

public:
    enum class ViewType {
        NAVIGATION_VIEW,
        HABIT_MANAGE_VIEW,
        EVENT_MANAGE_VIEW,
        POMODORO_VIEW,
        TIMELINE_VIEW,
        CALENDAR_VIEW
    };

    explicit ViewLayer(QWidget* parent = nullptr);

    void init();
    void setcurrentView(ViewType view);

    signals:
    // 你可以添加信号（如用户点了“添加事项”、“删除事项”等）
    void habitAdded();
    void habitDeleted(const Habit &habit);
    void eventAdded();
    void eventDeleted(const Event &event);

private slots:
    // 控件槽函数
    void onAddHabitClicked();
    void onDeleteHabitClicked();
    void onAddEventClicked();
    void onDeleteEventClicked();
    void onBackToNavigation();

private:
    ServiceLayer sv_Layer;

    ViewType cur_view_type = ViewType::NAVIGATION_VIEW;

    QVBoxLayout* main_layout;
    QWidget* navigation_widget;
    QWidget* habit_manage_widget;
    QWidget* event_manage_widget;

    // 输入变量
    std::string habit_name_input;
    std::size_t habit_target_count_input = 0;
    std::string event_name_input;

    Date start_date_input;
    Date end_date_input;
    Time event_time_input;

    // ================= 各视图初始化 =================
    void initNavigationView();
    void initHabitManageView();
    void initEventManageView();

    // 动态更新视图
    void clearLayout(QLayout* layout);
    void showView(QWidget* view);

    // 时间解析工具（可选）
    static bool parseDate(const std::string& str, Date& result);
    static bool parseTime(const std::string& str, Time& result);
};

#endif //VIEWLAYER_H
