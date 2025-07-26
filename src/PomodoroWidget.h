/**
 * @file PomodoroWidget.h
 * @brief 番茄钟界面控件类，实现番茄钟计时器功能
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef POMODOROWIDGET_H
#define POMODOROWIDGET_H

#include <QVBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QIcon>
#include <QTimer>
#include <QTime>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QFontMetrics>
#include "Pomodoro.h"

class PomodoroWidget : public QWidget
{
    Q_OBJECT

public:
    enum State {
        IDLE,       // 空闲状态
        RUNNING,    // 运行状态
        PAUSED      // 暂停状态
    };

    explicit PomodoroWidget(QWidget *parent = nullptr);
    explicit PomodoroWidget(const Pomodoro& pomo, QWidget* parent = nullptr);
    
    // 公共方法，供外部获取状态和信息
    State getState() const { return state_; }
    QString getRemark() const { return remark_; }
    QString getTimeDisplayText() const;
    int getRemainingSeconds() const { return remaining_seconds_; }
    int getTotalSeconds() const { return total_seconds_; }
    
    // 恢复番茄钟状态
    void restoreState(int state, int total_seconds, int remaining_seconds, const QString& remark, const QString& start_time);

signals:
    void stateChanged(); // 状态改变信号
    void timerUpdated(); // 定时器更新信号

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void handleImageButton();
    void handleMusicButton();
    void handleControlButton();
    void handleResetButton();
    void handleTimeEditFinished();
    void updateTimer();

private:

    State state_;
    int total_seconds_;
    int remaining_seconds_;
    QTimer* timer_;
    QTime start_time_;
    QString remark_;

    // UI组件
    QPushButton* image_button_;
    QPushButton* music_button_;
    QPushButton* control_button_;
    QPushButton* reset_button_;
    QLineEdit* time_edit_;      // 时间输入框
    QLabel* remark_label_;      // 备注显示标签
    QLabel* time_display_;      // 时间显示标签

    // 圆形钟参数
    int circle_radius_;
    QPoint circle_center_;

    // 私有方法
    void initCircularInterface();
    void setupButtonStyles();
    void updateTimeDisplay();
    void updateRemarkDisplay();
    bool parseTimeInput(const QString& input, int& hours, int& minutes, int& seconds);
    QString formatTime(int total_seconds) const;
};

#endif // POMODOROWIDGET_H