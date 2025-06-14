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

    ViewType curviewtype = ViewType::NAVIGATION_VIEW;

    QVBoxLayout* mainLayout;
    QWidget* navigationWidget;
    QWidget* habitManageWidget;
    QWidget* eventManageWidget;

    // 输入变量
    std::string habitNameInput;
    int habitTargetCountInput = 0;
    std::string eventNameInput;

    Date startDateInput;
    Date endDateInput;
    Time eventTimeInput;

    // ================= 各视图初始化 =================
    void initNavigationView();
    void initHabitManageView();
    void initEventManageView();

    // 动态更新视图
    void clearLayout(QLayout* layout);
    void showView(QWidget* view);

    // 时间解析工具（可选）
    static bool parse_date(const std::string& str, Date& result);
    static bool parse_time(const std::string& str, Time& result);
};

#endif //VIEWLAYER_H
