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
    
    control_button_->setText("⏸");
    updateTimeDisplay();
    updateRemarkDisplay();
    
    // 强制重绘
    update();
}

void PomodoroWidget::initCircularInterface()
{
    // 设置圆形钟容器
    setFixedSize(400, 400);
    
    // 创建主布局，不使用边距
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    
    // 创建圆形钟容器
    QWidget* circle_container = new QWidget(this);
    circle_container->setFixedSize(400, 400);
    circle_container->setStyleSheet("background: transparent;");
    
    // 创建圆形钟布局，使用绝对定位
    QVBoxLayout* circle_layout = new QVBoxLayout(circle_container);
    circle_layout->setContentsMargins(0, 0, 0, 0);
    circle_layout->setSpacing(0);
    
    // 创建顶部按钮容器 - 更靠近时间显示
    QWidget* top_container = new QWidget(circle_container);
    top_container->setFixedHeight(50);
    top_container->setStyleSheet("background: transparent;");
    
    QHBoxLayout* top_layout = new QHBoxLayout(top_container);
    top_layout->setContentsMargins(0, 0, 0, 0);
    top_layout->setSpacing(50);
    
    image_button_ = new QPushButton("📷", top_container);
    image_button_->setFixedSize(30, 30);
    image_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 25px; }");

    music_button_ = new QPushButton("🎵", top_container);
    music_button_->setFixedSize(30, 30);
    music_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 25px; }");

    top_layout->addStretch();
    top_layout->addWidget(image_button_);
    top_layout->addWidget(music_button_);
    top_layout->addStretch();

    // 创建备注显示区域
    QWidget* remark_container = new QWidget(circle_container);
    remark_container->setFixedHeight(15);
    remark_container->setStyleSheet("background: transparent;");

    QVBoxLayout* remark_layout = new QVBoxLayout(remark_container);
    remark_layout->setContentsMargins(0, 0, 0, 0);
    remark_layout->setSpacing(0);

    remark_label_ = new QLabel("", remark_container);
    remark_label_->setAlignment(Qt::AlignCenter);
    remark_label_->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-family: 'Arial';"
        "    font-size: 20px;"
        "    font-weight: normal;"
        "    background: transparent;"
        "}"
    );
    remark_layout->addWidget(remark_label_, 0, Qt::AlignCenter);

    // 创建中间时间显示容器 - 占据更多空间
    QWidget* time_container = new QWidget(circle_container);
    time_container->setFixedHeight(220);
    time_container->setStyleSheet("background: transparent;");

    QVBoxLayout* time_layout = new QVBoxLayout(time_container);
    time_layout->setContentsMargins(0, 0, 0, 0);
    time_layout->setSpacing(20);

    // 时间输入框（可编辑）
    time_edit_ = new QLineEdit("00 : 00 : 00", time_container);
    time_edit_->setAlignment(Qt::AlignCenter);
    time_edit_->setStyleSheet(
        "QLineEdit {"
        "    color: #333333;"
        "    font-family: 'Arial';"
        "    font-size: 36px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "    margin-top: -5px;"
        "}"
    );

    // 时间显示标签（只读）
    time_display_ = new QLabel("00 : 00 : 00", time_container);
    time_display_->setAlignment(Qt::AlignCenter);
    time_display_->setStyleSheet(
        "QLabel {"
        "    color: #333333;"
        "    font-family: 'Arial';"
        "    font-size: 36px;"
        "    font-weight: bold;"
        "    background: transparent;"
        "    margin-top: -5px;"
        "}"
    );
    time_display_->hide(); // 初始隐藏，显示输入框

    time_layout->addStretch();
    time_layout->addWidget(time_edit_, 0, Qt::AlignCenter);
    time_layout->addWidget(time_display_, 0, Qt::AlignCenter);
    time_layout->addStretch();

    // 创建底部按钮容器 - 更靠近时间显示
    QWidget* bottom_container = new QWidget(circle_container);
    bottom_container->setFixedHeight(50);
    bottom_container->setStyleSheet("background: transparent;");

    QHBoxLayout* bottom_layout = new QHBoxLayout(bottom_container);
    bottom_layout->setContentsMargins(0, 0, 0, 0);
    bottom_layout->setSpacing(50);

    control_button_ = new QPushButton("▶", bottom_container);
    control_button_->setFixedSize(35, 35);
    control_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 30px; }");

    reset_button_ = new QPushButton("🔄", bottom_container);
    reset_button_->setFixedSize(30, 30);
    reset_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 25px; }");
    
    bottom_layout->addStretch();
    bottom_layout->addWidget(control_button_);
    bottom_layout->addWidget(reset_button_);
    bottom_layout->addStretch();
    
    // 组装圆形钟布局
    circle_layout->addWidget(top_container);
    circle_layout->addWidget(remark_container);
    circle_layout->addWidget(time_container);
    circle_layout->addWidget(bottom_container);
    
    main_layout->addWidget(circle_container);
    
    // 连接信号
    connect(control_button_, &QPushButton::clicked, this, &PomodoroWidget::handleControlButton);
    connect(image_button_, &QPushButton::clicked, this, &PomodoroWidget::handleImageButton);
    connect(music_button_, &QPushButton::clicked, this, &PomodoroWidget::handleMusicButton);
    connect(reset_button_, &QPushButton::clicked, this, &PomodoroWidget::handleResetButton);
    connect(time_edit_, &QLineEdit::editingFinished, this, &PomodoroWidget::handleTimeEditFinished);
}

void PomodoroWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制圆形钟背景
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    painter.setBrush(QBrush(QColor(240, 240, 240)));
    painter.drawEllipse(circle_center_, circle_radius_, circle_radius_);
    
    // 绘制时间框背景
    if (time_edit_ && time_edit_->isVisible()) {
        QString time_text = time_edit_->text();
        QStringList time_parts = time_text.split(" : ");

                if (time_parts.size() == 3) {
            QFont font = time_edit_->font();
            QFontMetrics fm(font);
            
            // 计算时间显示区域的中心位置
            int center_x = width() / 2;
            int center_y = height() / 2;
            
            // 计算总文本宽度
            int total_width = 0;
            for (const QString& part : time_parts) {
                total_width += fm.horizontalAdvance(part);
            }
            total_width += fm.horizontalAdvance(" : ") * 2; // 加上两个冒号
            
            // 从中心开始绘制
            int x_offset = center_x - total_width / 2;
            int y_offset = center_y - fm.height() / 2; // 保持白色框在自然中心位置

            for (int i = 0; i < 3; ++i) {
                int text_width = fm.horizontalAdvance(time_parts[i]);
                QRect box_rect(x_offset - 12, y_offset - 12, text_width + 24, fm.height() + 24);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(180, 180, 180), 1));
                painter.setBrush(QBrush(QColor(245, 245, 245)));
                painter.drawRoundedRect(box_rect.translated(1, 1), 8, 8);

                // 绘制主框
                painter.setPen(QPen(QColor(200, 200, 200), 1));
                painter.setBrush(QBrush(QColor(255, 255, 255)));
                painter.drawRoundedRect(box_rect, 8, 8);

                x_offset += text_width + fm.horizontalAdvance(" : ");
            }
        }
    } else if (time_display_ && time_display_->isVisible()) {
        QString time_text = time_display_->text();
        QStringList time_parts = time_text.split(" : ");

                if (time_parts.size() == 3) {
            QFont font = time_display_->font();
            QFontMetrics fm(font);
            
            // 计算时间显示区域的中心位置
            int center_x = width() / 2;
            int center_y = height() / 2;
            
            // 计算总文本宽度
            int total_width = 0;
            for (const QString& part : time_parts) {
                total_width += fm.horizontalAdvance(part);
            }
            total_width += fm.horizontalAdvance(" : ") * 2; // 加上两个冒号
            
            // 从中心开始绘制
            int x_offset = center_x - total_width / 2;
            int y_offset = center_y - fm.height() / 2; // 保持白色框在自然中心位置

            for (int i = 0; i < 3; ++i) {
                int text_width = fm.horizontalAdvance(time_parts[i]);
                QRect box_rect(x_offset - 12, y_offset - 12, text_width + 24, fm.height() + 24);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(180, 180, 180), 1));
                painter.setBrush(QBrush(QColor(245, 245, 245)));
                painter.drawRoundedRect(box_rect.translated(1, 1), 8, 8);

                // 绘制主框
                painter.setPen(QPen(QColor(200, 200, 200), 1));
                painter.setBrush(QColor(255, 255, 255));
                painter.drawRoundedRect(box_rect, 8, 8);

                x_offset += text_width + fm.horizontalAdvance(" : ");
            }
        }
    }
}

void PomodoroWidget::setupButtonStyles()
{
    // 设置按钮悬停效果
    QString hover_style = "QPushButton:hover { background-color: rgba(0, 0, 0, 0.1); border-radius: 15px; }";
    
    if (image_button_) image_button_->setStyleSheet(image_button_->styleSheet() + hover_style);
    if (music_button_) music_button_->setStyleSheet(music_button_->styleSheet() + hover_style);
    if (control_button_) control_button_->setStyleSheet(control_button_->styleSheet() + hover_style);
    if (reset_button_) reset_button_->setStyleSheet(reset_button_->styleSheet() + hover_style);
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

