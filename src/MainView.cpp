#include "MainView.h"
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QRandomGenerator>
#include <QPixmap>
#include <QHBoxLayout>

MainView::MainView(ServiceLayer& service, PomodoroWidget& pomodoro, QWidget *parent)
    : QWidget(parent), service_(service), pomodoro_(pomodoro)
{
    // 初始化对话框文本
    dialogTexts_ = {
        "这是今天需要完成的事情哦~",
        "加油！你一定可以坚持下去！",
        "别忘了打卡和休息哦！",
        "每一天都值得被记录！",
        "习惯的力量很强大！"
    };

    initUI();
    setupConnections();
}

void MainView::initUI()
{
    // 主布局
    mainLayout_ = new QGridLayout(this);
    mainLayout_->setContentsMargins(15, 15, 15, 15);
    mainLayout_->setSpacing(15);

    // 设置行列比例
    mainLayout_->setColumnStretch(0, 2); // 习惯区域占2份
    mainLayout_->setColumnStretch(1, 1); // 右侧区域占1份
    mainLayout_->setRowStretch(0, 2);    // 上部分占2份
    mainLayout_->setRowStretch(1, 1);    // 下部分占1份

    setupHabitsSection();
    setupEventsSection();
    setupPomodoroSection();
    setupMascotSection();

    refreshAll();
}

void MainView::setupConnections()
{
    // 连接番茄钟状态变化信号
    connect(&pomodoro_, &PomodoroWidget::stateChanged, this, &MainView::refreshPomodoro);
    connect(&pomodoro_, &PomodoroWidget::timerUpdated, this, &MainView::refreshPomodoro);
}

void MainView::setupHabitsSection()
{
    habitsGroup_ = new QGroupBox("待打卡习惯", this);
    habitsGroup_->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");

    habitsScrollArea_ = new QScrollArea(habitsGroup_);
    habitsScrollArea_->setWidgetResizable(true);
    habitsScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    habitsContainer_ = new QWidget();
    habitsLayout_ = new QVBoxLayout(habitsContainer_);
    habitsLayout_->setAlignment(Qt::AlignTop);
    habitsLayout_->setSpacing(10);

    habitsScrollArea_->setWidget(habitsContainer_);

    QVBoxLayout* groupLayout = new QVBoxLayout(habitsGroup_);
    groupLayout->addWidget(habitsScrollArea_);

    mainLayout_->addWidget(habitsGroup_, 0, 0, 2, 1);
}

void MainView::setupEventsSection()
{
    eventsGroup_ = new QGroupBox("活跃事项", this);
    eventsGroup_->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");

    eventsScrollArea_ = new QScrollArea(eventsGroup_);
    eventsScrollArea_->setWidgetResizable(true);
    eventsScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    eventsContainer_ = new QWidget();
    eventsLayout_ = new QVBoxLayout(eventsContainer_);
    eventsLayout_->setAlignment(Qt::AlignTop);
    eventsLayout_->setSpacing(10);

    eventsScrollArea_->setWidget(eventsContainer_);

    QVBoxLayout* groupLayout = new QVBoxLayout(eventsGroup_);
    groupLayout->addWidget(eventsScrollArea_);

    mainLayout_->addWidget(eventsGroup_, 0, 1);
}

void MainView::setupPomodoroSection()
{
    pomodoroGroup_ = new QGroupBox("当前番茄钟", this);
    pomodoroGroup_->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; }");

    QVBoxLayout* layout = new QVBoxLayout(pomodoroGroup_);
    layout->setAlignment(Qt::AlignCenter);

    pomodoroTimeLabel_ = new QLabel("00:00:00", pomodoroGroup_);
    pomodoroTimeLabel_->setStyleSheet("font-size: 24px; font-weight: bold; color: #1890ff;");
    pomodoroTimeLabel_->setAlignment(Qt::AlignCenter);

    pomodoroRemarkLabel_ = new QLabel("", pomodoroGroup_);
    pomodoroRemarkLabel_->setStyleSheet("font-size: 16px; color: #666;");
    pomodoroRemarkLabel_->setAlignment(Qt::AlignCenter);
    pomodoroRemarkLabel_->setWordWrap(true);

    layout->addWidget(pomodoroTimeLabel_);
    layout->addWidget(pomodoroRemarkLabel_);

    mainLayout_->addWidget(pomodoroGroup_, 1, 1);
}

void MainView::setupMascotSection()
{
    mascotGroup_ = new QGroupBox("", this);
    mascotGroup_->setStyleSheet("QGroupBox { border: none; }");

    // 外层横向布局（撑开，把吉祥物部分推到右侧）
    QHBoxLayout* outerLayout = new QHBoxLayout(mascotGroup_);
    outerLayout->addStretch(); // 左边留空，把内容推到右边

    // 右侧垂直布局（对话框在上，吉祥物在下）
    QHBoxLayout* rightLayout = new QHBoxLayout();

    // 对话框
    QGroupBox* dialogGroup = new QGroupBox(mascotGroup_);
    dialogGroup->setStyleSheet(
        "QGroupBox {"
        "    background: #fffbe6;"
        "    border: 2px solid #ffd666;"
        "    border-radius: 16px;"
        "    padding: 16px;"
        "    min-width: 220px;"
        "    max-width: 320px;"
        "}"
    );

    dialogLabel_ = new QLabel(dialogTexts_.first(), dialogGroup);
    dialogLabel_->setStyleSheet(
        "QLabel {"
        "    color: #ad6800;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 4px 8px;"
        "    background: transparent;"
        "}"
    );
    dialogLabel_->setWordWrap(true);
    dialogLabel_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogGroup);
    dialogLayout->addWidget(dialogLabel_);

    // 吉祥物
    mascotLabel_ = new QLabel(mascotGroup_);
    QPixmap mascotPixmap(":/assets/images/logo.png");
    if (!mascotPixmap.isNull()) {
        mascotLabel_->setPixmap(
            mascotPixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        mascotLabel_->setText("LOGO");
    }
    mascotLabel_->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    // 添加到右侧布局
    rightLayout->addWidget(dialogGroup, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(mascotLabel_, 0, Qt::AlignRight | Qt::AlignBottom);

    // 把右侧整体加入外层布局
    outerLayout->addLayout(rightLayout);

    mainLayout_->addWidget(mascotGroup_, 2, 1);
}


void MainView::refreshAll()
{
    refreshTodoArea();
    refreshMascotDialog();
}

void MainView::refreshTodoArea()
{
    refreshHabits();
    refreshEvents();
    refreshPomodoro();
}

void MainView::refreshHabits()
{
    clearLayout(habitsLayout_);

    auto habits = service_.getActiveHabits();
    if (habits.empty()) {
        QLabel* noHabitLabel = new QLabel("暂无习惯", habitsContainer_);
        noHabitLabel->setAlignment(Qt::AlignCenter);
        noHabitLabel->setStyleSheet("color: #888; font-size: 16px;");
        habitsLayout_->addWidget(noHabitLabel);
    } else {
        for (const auto& habit : habits) {
            habitsLayout_->addWidget(createHabitCard(habit));
        }
    }
}

void MainView::refreshEvents()
{
    clearLayout(eventsLayout_);

    auto events = service_.getActiveEvents();
    if (events.empty()) {
        QLabel* noEventLabel = new QLabel("暂无事项", eventsContainer_);
        noEventLabel->setAlignment(Qt::AlignCenter);
        noEventLabel->setStyleSheet("color: #888; font-size: 16px;");
        eventsLayout_->addWidget(noEventLabel);
    } else {
        for (const auto& event : events) {
            eventsLayout_->addWidget(createEventCard(event));
        }
    }
}

void MainView::refreshPomodoro() const
{
    if (pomodoro_.getState() != PomodoroWidget::IDLE) {
        pomodoroTimeLabel_->setText(pomodoro_.getTimeDisplayText());
        pomodoroRemarkLabel_->setText(pomodoro_.getRemark());
        pomodoroRemarkLabel_->show();
    } else {
        pomodoroTimeLabel_->setText("暂无番茄钟");
        pomodoroRemarkLabel_->hide();
    }
}

void MainView::refreshMascotDialog()
{
    int index = QRandomGenerator::global()->bounded(dialogTexts_.size());
    dialogLabel_->setText(dialogTexts_[index]);
}

QWidget* MainView::createHabitCard(const Habit& habit)
{
    QWidget* card = new QWidget(habitsContainer_);
    card->setStyleSheet(
        "QWidget {"
        "    background: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "}"
    );

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);

    // 习惯信息
    QLabel* infoLabel = new QLabel(
        QString("%1 (%2/%3)")
            .arg(QString::fromStdString(habit.name))
            .arg(service_.getHabitCheckInCount(habit))
            .arg(habit.target_count),
        card
    );
    infoLabel->setStyleSheet("font-size: 14px;");

    // 打卡按钮
    QPushButton* checkBtn = new QPushButton("✅", card);
    checkBtn->setFixedSize(60, 25);
    checkBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #52c41a;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #389e0d;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #d9d9d9;"
        "}"
    );

    // 如果已完成打卡目标，禁用按钮
    if (service_.getHabitCheckInCount(habit) >= habit.target_count) {
        checkBtn->setEnabled(false);
    }

    // 编辑按钮
    QPushButton* editBtn = new QPushButton("✏️", card);
    editBtn->setFixedSize(50, 25);
    editBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #faad14;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d48806;"
        "}"
    );

    // 删除按钮
    QPushButton* deleteBtn = new QPushButton("🗑️", card);
    deleteBtn->setFixedSize(50, 25);
    deleteBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #ff4d4f;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #cf1322;"
        "}"
    );

    // 连接信号
    connect(checkBtn, &QPushButton::clicked, this, [this, habit]
    {
        if (service_.checkinHabit(habit)) {
            emit habitCheckedIn(habit);
            refreshHabits();
        }
    });

    connect(editBtn, &QPushButton::clicked, this, [this, habit]
    {
        emit habitEdited(habit);
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this, habit]
    {
        if (QMessageBox::question(this, "确认删除",
            QString("确定删除习惯「%1」吗？").arg(QString::fromStdString(habit.name))) == QMessageBox::Yes) {
            emit habitDeleted(habit);
        }
    });

    layout->addWidget(infoLabel, 1);
    layout->addWidget(checkBtn);
    layout->addWidget(editBtn);
    layout->addWidget(deleteBtn);

    return card;
}

QWidget* MainView::createEventCard(const Event& event)
{
    QWidget* card = new QWidget(eventsContainer_);
    card->setStyleSheet(
        "QWidget {"
        "    background: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 8px;"
        "    padding: 10px;"
        "}"
    );

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);

    // 事项信息
    QLabel* infoLabel = new QLabel(
        QString("%1\n%2 %3")
            .arg(QString::fromStdString(event.title))
            .arg(QString::fromStdString(toString(event.event_date)))
            .arg(QString::fromStdString(toString(event.event_time))),
        card
    );
    infoLabel->setStyleSheet("font-size: 14px;");

    // 编辑按钮
    QPushButton* editBtn = new QPushButton("✏️", card);
    editBtn->setFixedSize(50, 25);
    editBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #1890ff;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #096dd9;"
        "}"
    );

    // 删除按钮
    QPushButton* deleteBtn = new QPushButton("🗑️", card);
    deleteBtn->setFixedSize(50, 25);
    deleteBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #ff4d4f;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #cf1322;"
        "}"
    );

    // 连接信号
    connect(editBtn, &QPushButton::clicked, this, [this, event]() {
        emit eventEdited(event);
    });

    connect(deleteBtn, &QPushButton::clicked, this, [this, event]() {
        if (QMessageBox::question(this, "确认删除",
            QString("确定删除事项「%1」吗？").arg(QString::fromStdString(event.title))) == QMessageBox::Yes) {
            emit eventDeleted(event);
        }
    });

    layout->addWidget(infoLabel, 1);
    layout->addWidget(editBtn);
    layout->addWidget(deleteBtn);

    return card;
}

void MainView::clearLayout(QLayout* layout)
{
    if (!layout) return;

    QLayoutItem* item;
    while ((item = layout->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}