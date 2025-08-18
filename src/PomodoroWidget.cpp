#include "PomodoroWidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>

PomodoroWidget::PomodoroWidget(ServiceLayer& service, QWidget *parent)
        : QWidget(parent), service_(service), state_(IDLE), total_seconds_(0), remaining_seconds_(0)
{
    // 设置固定大小
    setFixedSize(400, 400);

    // 初始化圆形钟参数
    circle_radius_ = 140;
    circle_center_ = QPoint(200, 200);

    initCircularInterface();
    setupButtonStyles();

    // 初始化计时器
    timer_ = new QTimer(this);
    timer_->setInterval(1000); // 每秒更新
    connect(timer_, &QTimer::timeout, this, &PomodoroWidget::updateTimer);
    connect(this, &PomodoroWidget::timerFinished, this, &PomodoroWidget::insertPomo);

    // 设置初始时间显示
    updateTimeDisplay();
    updateRemarkDisplay();

    // 强制重绘
    update();
}

PomodoroWidget::PomodoroWidget(ServiceLayer& service, const Pomodoro& pomo, QWidget* parent)
        : QWidget(parent), service_(service), state_(RUNNING)
{
    // 设置固定大小
    setFixedSize(400, 400);

    // 初始化圆形钟参数
    circle_radius_ = 140;
    circle_center_ = QPoint(200, 200);

    initCircularInterface();
    setupButtonStyles();

    // 设置初始时间
    int h = std::chrono::duration_cast<std::chrono::hours>(pomo.pomodoro_duration.to_duration()).count();
    int m = std::chrono::duration_cast<std::chrono::minutes>(pomo.pomodoro_duration.to_duration()).count() % 60;
    int s = std::chrono::duration_cast<std::chrono::seconds>(pomo.pomodoro_duration.to_duration()).count() % 60;

    total_seconds_ = h * 3600 + m * 60 + s;
    remaining_seconds_ = total_seconds_;
    start_time_ = QTime::currentTime();
    remark_ = QString::fromStdString(pomo.record);

    // 初始化计时器
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

void PomodoroWidget::insertPomo(const Pomodoro& pomo) const
{
    if (service_.insertPomoRecord(pomo))
    {
        qDebug() << "番茄钟数据插入成功";
    }
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
    image_button_->setFixedSize(40, 40);
    image_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 30px; }");

    music_button_ = new QPushButton("🎵", top_container);
    music_button_->setFixedSize(40, 40);
    music_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 30px; }");

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
    time_layout->setSpacing(0);

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
    control_button_->setFixedSize(40, 40);
    control_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 40px; }");

    reset_button_ = new QPushButton("🔄", bottom_container);
    reset_button_->setFixedSize(40, 40);
    reset_button_->setStyleSheet("QPushButton { border: none; background: transparent; font-size: 30px; }");

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

            // 从中心开始绘制，向右偏移20像素
            int x_offset = center_x - total_width / 2 - 5;
            int y_offset = center_y - fm.height() / 2+10; // 保持白色框在自然中心位置

            for (int i = 0; i < 3; ++i) {
                int text_width = fm.horizontalAdvance(time_parts[i]);
                QRect box_rect(x_offset - 8, y_offset - 8, text_width + 16, fm.height() + 16);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(180, 180, 180), 1));
                painter.setBrush(QBrush(QColor(245, 245, 245)));
                painter.drawRoundedRect(box_rect.translated(1, 1), 8, 8);

                // 绘制主框
                painter.setPen(QPen(QColor(200, 200, 200), 1));
                painter.setBrush(QBrush(QColor(255, 255, 255)));
                painter.drawRoundedRect(box_rect, 8, 8);

                x_offset += text_width + fm.horizontalAdvance(" : ") + 5; // 增加5像素的额外间距
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

            // 从中心开始绘制，向右偏移20像素
            int x_offset = center_x - total_width / 2 -5;
            int y_offset = center_y - fm.height() / 2+10; // 保持白色框在自然中心位置

            for (int i = 0; i < 3; ++i) {
                int text_width = fm.horizontalAdvance(time_parts[i]);
                QRect box_rect(x_offset - 8, y_offset - 8, text_width + 16, fm.height() + 16);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(180, 180, 180), 1));
                painter.setBrush(QBrush(QColor(245, 245, 245)));
                painter.drawRoundedRect(box_rect.translated(1, 1), 8, 8);

                // 绘制主框
                painter.setPen(QPen(QColor(200, 200, 200), 1));
                painter.setBrush(QColor(255, 255, 255));
                painter.drawRoundedRect(box_rect, 8, 8);

                x_offset += text_width + fm.horizontalAdvance(" : ") + 5; // 增加5像素的额外间距
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
}

void PomodoroWidget::handleControlButton()
{
    if (state_ == IDLE) {
        // 检查是否已设置时间
        if (total_seconds_ == 0) {
            QMessageBox::warning(this, "提示", "请先设置时间！");
            return;
        }

        // 输入备注
        bool ok;
        QString remark = QInputDialog::getText(this, "输入备注", "请输入备注信息:", QLineEdit::Normal, "", &ok);
        if (!ok) return;

        if (remark.trimmed().isEmpty()) {
            QMessageBox::warning(this, "提示", "备注不能为空！");
            return;
        }

        remark_ = remark.trimmed();
        updateRemarkDisplay();

        // 开始计时
        timer_->start();
        state_ = RUNNING;
        control_button_->setText("⏸");

        // 切换到只读显示模式
        time_edit_->hide();
        time_display_->show();
        time_display_->setText(formatTime(remaining_seconds_));

        // 发出状态改变信号
        emit stateChanged();

    } else if (state_ == RUNNING) {
        // 暂停
        timer_->stop();
        state_ = PAUSED;
        control_button_->setText("▶");

        // 发出状态改变信号
        emit stateChanged();

    } else if (state_ == PAUSED) {
        // 继续
        timer_->start();
        state_ = RUNNING;
        control_button_->setText("⏸");

        // 发出状态改变信号
        emit stateChanged();
    }

    update();
}

void PomodoroWidget::handleResetButton()
{
    // 重置所有状态
    timer_->stop();
    state_ = IDLE;
    total_seconds_ = 0;
    remaining_seconds_ = 0;
    remark_.clear();

    // 重置UI
    control_button_->setText("▶");
    time_edit_->setText("00 : 00 : 00");
    time_edit_->show();
    time_display_->hide();
    updateRemarkDisplay();

    update();

    // 发出状态改变信号
    emit stateChanged();
}

void PomodoroWidget::handleTimeEditFinished()
{
    QString time_text = time_edit_->text();
    int hours, minutes, seconds;

    if (parseTimeInput(time_text, hours, minutes, seconds)) {
        total_seconds_ = hours * 3600 + minutes * 60 + seconds;
        remaining_seconds_ = total_seconds_;
        time_edit_->setText(formatTime(total_seconds_));
    } else {
        QMessageBox::warning(this, "格式错误", "时间格式不正确！请使用 HH:MM:SS 格式");
        time_edit_->setText("00 : 00 : 00");
        total_seconds_ = 0;
        remaining_seconds_ = 0;
    }
}

void PomodoroWidget::updateTimer()
{
    if (state_ == RUNNING) {
        if (remaining_seconds_ > 0) {
            remaining_seconds_--;
            time_display_->setText(formatTime(remaining_seconds_));
            update();
            emit timerUpdated();
        }
        if (remaining_seconds_ <= 0) {
            remaining_seconds_ = 0;
            timer_->stop();
            state_ = IDLE;
            control_button_->setText("▶");
            QMessageBox::information(this, "时间到", "番茄钟时间到！");
            emit stateChanged();
            emit timerFinished(Pomodoro{0, Time{std::chrono::seconds(total_seconds_)}, remark_.toStdString()});
        }
    }
}

void PomodoroWidget::updateTimeDisplay()
{
    if (time_edit_) {
        time_edit_->setText(formatTime(total_seconds_));
    }
    if (time_display_) {
        time_display_->setText(formatTime(remaining_seconds_));
    }
}

void PomodoroWidget::updateRemarkDisplay()
{
    if (remark_label_) {
        remark_label_->setText(remark_);
    }
}

bool PomodoroWidget::parseTimeInput(const QString& input, int& hours, int& minutes, int& seconds)
{
    QStringList parts = input.split(":");
    if (parts.size() != 3) return false;

    bool ok1, ok2, ok3;
    hours = parts[0].trimmed().toInt(&ok1);
    minutes = parts[1].trimmed().toInt(&ok2);
    seconds = parts[2].trimmed().toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) return false;
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) return false;

    return true;
}

QString PomodoroWidget::formatTime(int total_seconds) const
{
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    return QString("%1 : %2 : %3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
}

void PomodoroWidget::handleImageButton()
{
    // 图片按钮功能（待实现）
    QMessageBox::information(this, "图片", "图片功能待实现");
}

void PomodoroWidget::handleMusicButton()
{
    // 音乐按钮功能（待实现）
    QMessageBox::information(this, "音乐", "音乐功能待实现");
}

QString PomodoroWidget::getTimeDisplayText() const
{
    if (state_ == IDLE) {
        return "00 : 00 : 00";
    }
    return formatTime(remaining_seconds_);
}

void PomodoroWidget::restoreState(int state, int total_seconds, int remaining_seconds, const QString& remark, const QString& start_time)
{
    if (timer_) timer_->stop();
    state_ = static_cast<State>(state);
    total_seconds_ = total_seconds;
    remaining_seconds_ = remaining_seconds; // 这里已经是计算好的剩余时间
    remark_ = remark;

    if (state_ == IDLE) {
        control_button_->setText("▶");
        time_edit_->setText("00 : 00 : 00");
        time_edit_->show();
        time_display_->hide();
    } else {
        // 对于运行中或暂停的状态
        if (state_ == RUNNING) {
            control_button_->setText("⏸");
            time_edit_->hide();
            time_display_->show();
            time_display_->setText(formatTime(remaining_seconds_));
            timer_->start();
        } else if (state_ == PAUSED) {
            control_button_->setText("▶");
            time_edit_->hide();
            time_display_->show();
            time_display_->setText(formatTime(remaining_seconds_));
        }
    }
    updateRemarkDisplay();
    update();
    emit stateChanged();
}