/**
 * @file ViewLayer.h
 * @brief 视图层头文件，基于Qt实现用户界面，处理用户交互
 * @author Rain 阿浪 Darling 冰柠
 * @date 2025年06月18日
 */

#ifndef VIEWLAYER_H
#define VIEWLAYER_H

/* Set header file*/
#include <QApplication>
#include <QDateEdit>
#include <QScrollArea>
#include <QWidget>

#include "Times.h"
#include "Event.h"
#include "Habit.h"
#include "PomodoroWidget.h"
#include "ServiceLayer.h"
#include "Utility.h"

/**
 * @class ViewLayer
 * @brief 视图层类，基于Qt实现用户界面，处理用户交互逻辑
 * @author Rain 阿浪 Darling 冰柠
 */
class ViewLayer final : public QWidget
{
    Q_OBJECT;

public:
    /**
     * @enum ViewType
     * @brief 定义系统支持的视图类型
     */
    enum class ViewType
    {
        NAVIGATION_VIEW,        /**< 导航视图 */
        HABIT_MANAGE_VIEW,      /**< 习惯管理视图 */
        EVENT_MANAGE_VIEW,      /**< 事项管理视图 */
        POMODORO_VIEW,          /**< 番茄钟视图 */
        TIMELINE_VIEW,          /**< 时间线视图 */
        CALENDAR_VIEW           /**< 日历视图 */
    };

    /**
     * @brief 构造函数，初始化视图层对象
     * @param parent 父窗口部件，默认为nullptr
     * @author Rain
     */
    explicit ViewLayer(QWidget* parent = nullptr);

    /**
     * @brief 初始化视图层
     * @author Rain
     */
    void init();

    /**
     * @brief 重置当前显示的视图
     * @param view 要显示的视图类型
     * @author 冰柠
     */
    void resetCurrentView(ViewType view);

    /**
     * @brief 设置当前显示的视图
     * @param view 要显示的视图类型
     * @author Rain
     */
    void setCurrentView(ViewType view);

    /**
     * @brief 显示主界面吉祥物（如兔子看板娘）
     * @author 
     */
    void showMascot();

    /**
     * @brief 显示主界面对话框提示信息
     * @author 
     * @param message 提示信息内容
     */
    void showMainDialogMessage(const QString& message);

    /**
     * @brief 进入预览模块
     * @author 
     */
    void enterPreviewModule();

    /**
     * @brief 跳转到指定预览模块
     * @param module_id 预览模块ID
     * @author 
     */
    void jumpToPreviewModule(int module_id);

    /**
     * @brief 分类习惯，区分轻重缓急
     */
    void classifyHabits();

signals:
    /**
     * @brief 习惯添加信号
     */
    void habitAdded();

    /**
     * @brief 习惯删除信号
     * @param habit 被删除的习惯对象
     */
    void habitDeleted(const Habit &habit);

    /**
     * @brief 事项添加信号
     */
    void eventAdded();

    /**
     * @brief 事项删除信号
     * @param event 被删除的事项对象
     */
    void eventDeleted(const Event &event);

    /**
     * @brief 事项修改信号
     * @param event 被修改的事项对象
     */
    void eventModified(const Event &event);

private slots:
    /**
     * @brief 添加习惯按钮点击槽函数
     * @author Rain
     */
    void onAddHabitClicked();

    /**
     * @brief 删除习惯按钮点击槽函数
     * @author Rain
     */
    void onDeleteHabitClicked();

    /**
     * @brief 添加事项按钮点击槽函数
     * @author 阿浪
     */
    void onAddEventClicked();

    /**
     * @brief 删除事项按钮点击槽函数
     * @author 阿浪
     */
    void onDeleteEventClicked();

    /**
     * @brief 返回导航视图槽函数
     * @author Darling
     */
    void onBackToNavigation();

private:
    ServiceLayer sv_Layer; /**< 服务层对象，用于调用业务逻辑 */
    ViewType cur_view_type; /**< 当前显示的视图类型 */

    QVBoxLayout* main_layout{}; /**< 主布局 */
    QWidget* navigation_widget{}; /**< 导航视图部件 */
    QWidget* habit_manage_widget{}; /**< 习惯管理视图部件 */
    QWidget* event_manage_widget{}; /**< 事项管理视图部件 */
    QWidget* pomodoro_widget{}; /**< 番茄钟视图部件 */
    PomodoroWidget* pomodoro_widget_component{}; /**< 番茄钟视图内部部件 */
    QWidget* timeline_widget{}; /**< 时间线 */

    // 输入变量
    std::string habit_name_input; /**< 习惯名称输入 */
    std::size_t habit_target_count_input{}; /**< 习惯目标次数输入 */
    std::string event_name_input; /**< 事项名称输入 */

    Date start_date_input{}; /**< 开始日期输入 */
    Date end_date_input{}; /**< 结束日期输入 */
    Time event_time_input; /**< 事项时间输入 */

    // 时间线变量
    QDateEdit *dateEdit = nullptr;
    QScrollArea *timeline_scroll_area = nullptr;
    QWidget *timeline_content_widget = nullptr;
    QVBoxLayout *timeline_layout = nullptr;

    // ================= 各视图初始化 =================
    /**
     * @brief 初始化导航视图
     * @author Darling
     */
    void initNavigationView();

    /**
     * @brief 初始化习惯管理视图
     * @author Rain
     */
    void initHabitManageView();

    /**
     * @brief 新建习惯弹窗
     * @author Rain
     */
    void habitInsertView();

    /**
     * @brief 修改习惯弹窗
     * @author Rain
     */
    void habitUpdateView(const Habit &habit);

    /**
     * @brief 初始化事项管理视图
     * @author Darling
     */
    void initEventManageView();

    /**
    * @brief 初始化时间线视图
    * @author Rain
    */
    void initTimelineView();

    /**
     * @brief 初始化番茄钟视图
     * @author 冰柠
     */
    void initPomodoroView();

    // 动态更新视图
    /**
     * @brief 清空布局
     * @param layout 要清空的布局
     * @author 冰柠
     */
    static void clearLayout(QLayout* layout);

    /**
     * @brief 显示指定视图
     * @param type 要显示的视图部件类型
     * @author 冰柠
     */
    void showView(ViewType type);

    // 时间解析工具（可选）
    /**
     * @brief 解析日期字符串
     * @param str 日期字符串
     * @param result 解析结果存储的Date对象
     * @return 解析是否成功
     * @author 冰柠
     */
    static bool parseDate(const std::string& str, Date& result);

    /**
     * @brief 解析时间字符串
     * @param str 时间字符串
     * @param result 解析结果存储的Time对象
     * @return 解析是否成功
     * @author 冰柠
     */
    static bool parseTime(const std::string& str, Time& result);
};

#endif //VIEWLAYER_H