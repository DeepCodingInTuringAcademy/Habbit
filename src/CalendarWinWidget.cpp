#include "CalendarWinWidget.h"


CalendarWinWidget::CalendarWinWidget(QWidget* parent)
    : QWidget(parent), current_month(QDate::currentDate())
{
    // 设置无边框+透明背景+置顶
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QWidget { background-color: rgba(255, 255, 255, 220); border: 1px solid gray; border-radius: 10px; }");

    setFixedWidth(300);
    buildUI();
    refreshCalendar();
}


void CalendarWinWidget::buildUI()
{
    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(10, 10, 10, 10);
    main_layout->setSpacing(5);

    auto top_layout = new QHBoxLayout();
    top_layout->setSpacing(5);

    // 顶部月份标题
    month_label = new QLabel(current_month.toString("yyyy年 M月"));
    auto prev_button = new QPushButton("◀");
    prev_button->setFixedSize(20, 20);
    connect(prev_button, &QPushButton::clicked, this, &CalendarWinWidget::onPrevMonth);
    auto next_button = new QPushButton("▶");
    next_button->setFixedSize(20, 20);
    connect(next_button, &QPushButton::clicked, this, &CalendarWinWidget::onNextMonth);
    month_label->setAlignment(Qt::AlignCenter);
    month_label->setStyleSheet("font-weight: bold; font-size: 16px;");
    top_layout->addWidget(prev_button);
    top_layout->addWidget(month_label);
    top_layout->addWidget(next_button);
    main_layout->addLayout(top_layout);

    // 日历网格布局
    calendar_layout = new QGridLayout();
    calendar_layout->setSpacing(3);
    main_layout->addLayout(calendar_layout);

    // 日历工具类
    calendar_util = new Calendar(this);
}

void CalendarWinWidget::refreshCalendar() const
{
    // 更新标题
    month_label->setText(current_month.toString("yyyy年 M月"));

    // 构建日历
    calendar_util->setCurrentDate(QDate::currentDate());
    calendar_util->buildCalendarGrid(calendar_layout, current_month);
}

void CalendarWinWidget::onPrevMonth()
{
    current_month = current_month.addMonths(-1);
    refreshCalendar();
}

void CalendarWinWidget::onNextMonth()
{
    current_month = current_month.addMonths(1);
    refreshCalendar();
}

void CalendarWinWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        drag_position = event->globalPosition().toPoint() - frameGeometry().topLeft();
}

void CalendarWinWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        move(event->globalPosition().toPoint() - drag_position);
}

