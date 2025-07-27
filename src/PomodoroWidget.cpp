#include "PomodoroWidget.h"
#include"Pomodoro.h"

PomodoroWidget::PomodoroWidget(QWidget *parent)
    : QWidget(parent), state_(IDLE), total_seconds_(0), remaining_seconds_(0), pause_duration_(0)
{
    const QFont font("Microsoft YaHei", 20);

    hours_edit_ = new QLineEdit("00", this);
    minutes_edit_ = new QLineEdit("00", this);
    seconds_edit_ = new QLineEdit("00", this);
    control_button_ = new QPushButton("开始", this);
    time_display_ = new QLabel("00:00:00", this);

    hours_edit_->setFont(font);
    minutes_edit_->setFont(font);
    seconds_edit_->setFont(font);
    control_button_->setFont(font);
    time_display_->setFont(font);
    time_display_->setAlignment(Qt::AlignCenter);

    hours_edit_->setFixedWidth(60);
    minutes_edit_->setFixedWidth(60);
    seconds_edit_->setFixedWidth(60);

    const auto time_layout = new QHBoxLayout;
    time_layout->addWidget(hours_edit_);
    time_layout->addWidget(new QLabel(":"));
    time_layout->addWidget(minutes_edit_);
    time_layout->addWidget(new QLabel(":"));
    time_layout->addWidget(seconds_edit_);

    const auto main_layout = new QVBoxLayout(this);
    main_layout->addLayout(time_layout);
    main_layout->addWidget(control_button_);
    main_layout->addWidget(time_display_);

    connect(control_button_, &QPushButton::clicked, this, &PomodoroWidget::handleControlButton);

    timer_ = new QTimer(this);
    timer_->setInterval(1000); // 每秒更新
    connect(timer_, &QTimer::timeout, this, &PomodoroWidget::updateTimer);
}

PomodoroWidget::PomodoroWidget(const Pomodoro& pomo, QWidget* parent)
    : QWidget(parent), state_(RUNNING), pause_duration_(0)
{
    const QFont font("Microsoft YaHei", 20);

    hours_edit_ = new QLineEdit("00", this);
    minutes_edit_ = new QLineEdit("00", this);
    seconds_edit_ = new QLineEdit("00", this);
    control_button_ = new QPushButton("暂停", this);
    time_display_ = new QLabel("00:00:00", this);

    hours_edit_->setFont(font);
    minutes_edit_->setFont(font);
    seconds_edit_->setFont(font);
    control_button_->setFont(font);
    time_display_->setFont(font);
    time_display_->setAlignment(Qt::AlignCenter);

    hours_edit_->setFixedWidth(60);
    minutes_edit_->setFixedWidth(60);
    seconds_edit_->setFixedWidth(60);

    const auto time_layout = new QHBoxLayout;
    time_layout->addWidget(hours_edit_);
    time_layout->addWidget(new QLabel(":"));
    time_layout->addWidget(minutes_edit_);
    time_layout->addWidget(new QLabel(":"));
    time_layout->addWidget(seconds_edit_);

    const auto main_layout = new QVBoxLayout(this);
    main_layout->addLayout(time_layout);
    main_layout->addWidget(control_button_);
    main_layout->addWidget(time_display_);

    // 设置初始时间
    int h = std::chrono::duration_cast<std::chrono::hours>(pomo.pomodoro_time.to_duration()).count();
    int m = std::chrono::duration_cast<std::chrono::minutes>(pomo.pomodoro_time.to_duration()).count() % 60;
    int s = std::chrono::duration_cast<std::chrono::seconds>(pomo.pomodoro_time.to_duration()).count() % 60;
    hours_edit_->setText(QString::number(h).rightJustified(2, '0'));
    minutes_edit_->setText(QString::number(m).rightJustified(2, '0'));
    seconds_edit_->setText(QString::number(s).rightJustified(2, '0'));

    total_seconds_ = h * 3600 + m * 60 + s;
    remaining_seconds_ = total_seconds_;
    start_time_ = QTime::currentTime();
    remark_ = QString::fromStdString(pomo.record);

    connect(control_button_, &QPushButton::clicked, this, &PomodoroWidget::handleControlButton);

    timer_ = new QTimer(this);
    timer_->setInterval(1000); // 每秒更新
    connect(timer_, &QTimer::timeout, this, &PomodoroWidget::updateTimer);
    timer_->start();
    control_button_->setText("暂停");
}

void PomodoroWidget::handleControlButton()
{
    switch (state_) {
    case IDLE: {
        const int h = hours_edit_->text().toInt();
        const int m = minutes_edit_->text().toInt();
        const int s = seconds_edit_->text().toInt();
        total_seconds_ = h * 3600 + m * 60 + s;

        if (total_seconds_ == 0) {
            QMessageBox::warning(this, "错误", "时间不能为零！");
            return;
        }

        bool ok;
        const QString input = QInputDialog::getText(this, "备注", "请输入番茄钟备注：", QLineEdit::Normal, "", &ok);
        if (!ok) return;
        remark_ = input;

        remaining_seconds_ = total_seconds_;
        start_time_ = QTime::currentTime();
        timer_->start();
        state_ = RUNNING;
        control_button_->setText("暂停");
        break;
    }
    case RUNNING:
        pause_start_ = QTime::currentTime();
        timer_->stop();
        state_ = PAUSED;
        control_button_->setText("继续");
        break;
    case PAUSED:
        pause_duration_ += pause_start_.secsTo(QTime::currentTime());
        timer_->start();
        state_ = RUNNING;
        control_button_->setText("暂停");
        break;
    }
}

void PomodoroWidget::updateTimer()
{
    const int elapsed = start_time_.secsTo(QTime::currentTime()) - pause_duration_;
    remaining_seconds_ = total_seconds_ - elapsed;

    if (remaining_seconds_ <= 0) {
        timer_->stop();
        handleTimeUp();
        return;
    }

    const int h = remaining_seconds_ / 3600;
    const int m = remaining_seconds_ % 3600 / 60;
    const int s = remaining_seconds_ % 60;

    time_display_->setText(QString("%1:%2:%3")
        .arg(h, 2, 10, QLatin1Char('0'))
        .arg(m, 2, 10, QLatin1Char('0'))
        .arg(s, 2, 10, QLatin1Char('0')));
}

void PomodoroWidget::handleTimeUp()
{
    QMessageBox::information(this, "时间到", QString("番茄钟已完成！\n备注：%1").arg(remark_));
    state_ = IDLE;
    control_button_->setText("开始");
    remaining_seconds_ = 0;
    pause_duration_ = 0;
    time_display_->setText("00:00:00");
}

