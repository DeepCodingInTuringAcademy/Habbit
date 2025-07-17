/**
 * @file PomodoroWidget.h
 * @brief 番茄钟界面控件类，实现番茄钟计时器功能
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef POMODOROWIDGET_H
#define POMODOROWIDGET_H

#include <QFont>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QTime>

/**
 * @class PomodoroWidget
 * @brief 实现番茄钟计时器功能的Qt控件
 */
class PomodoroWidget final : public QWidget
{
Q_OBJECT

public:
    /**
     * @brief 构造函数，初始化番茄钟控件
     * @param parent 父窗口部件，默认为nullptr
     */
    explicit PomodoroWidget(QWidget* parent = nullptr);

private slots:
    /**
     * @brief 处理控制按钮点击事件
     */
    void handleControlButton();

    /**
     * @brief 更新计时器显示
     */
    void updateTimer();

    /**
     * @brief 处理时间到事件
     */
    void handleTimeUp();

private:
    /**
     * @enum State
     * @brief 定义番茄钟的状态
     */
    enum State
    {
        IDLE,   /**< 空闲状态 */
        RUNNING,/**< 运行状态 */
        PAUSED /**< 暂停状态 */
    };

    State state_;                /**< 当前番茄钟状态 */
    QTimer* timer_;             /**< Qt计时器对象 */
    QTime start_time_;          /**< 开始时间 */
    int total_seconds_;         /**< 总秒数 */
    int remaining_seconds_;     /**< 剩余秒数 */
    int pause_duration_;        /**< 暂停持续时间 */
    QTime pause_start_;         /**< 暂停开始时间 */

    QLineEdit* hours_edit_;     /**< 小时输入框 */
    QLineEdit* minutes_edit_;   /**< 分钟输入框 */
    QLineEdit* seconds_edit_;   /**< 秒输入框 */
    QPushButton* control_button_;/**< 控制按钮 */
    QLabel* time_display_;      /**< 时间显示标签 */
    QString remark_;            /**< 备注信息 */
};

#endif // POMODOROWIDGET_H