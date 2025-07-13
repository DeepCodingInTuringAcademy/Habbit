#ifndef POMODOROWIDGET_H
#define POMODOROWIDGET_H

#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class PomodoroWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit PomodoroWidget(QWidget* parent = nullptr);

private slots:
    void handleControlButton();
    void updateTimer();
    void handleTimeUp();

private:
    enum State
    {
        IDLE, RUNNING, PAUSED
    };

    State state_;
    QTimer* timer_;
    QTime start_time_;
    int total_seconds_;
    int remaining_seconds_;
    int pause_duration_;
    QTime pause_start_;

    QLineEdit* hours_edit_;
    QLineEdit* minutes_edit_;
    QLineEdit* seconds_edit_;
    QPushButton* control_button_;
    QLabel* time_display_;
    QString remark_;
};

#endif // POMODOROWIDGET_H
