#include "CalendarView.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

CalendarView::CalendarView(ServiceLayer* service, QWidget* parent)
    : QWidget(parent),
      calendar_util(this, service, true), // 启用详情显示
      m_service(service),
      m_current_month(QDate::currentDate())
{
    initUI();
    refreshCalendar();
}

void CalendarView::initUI()
{
    main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(10);

    // 月份导航
    auto header_layout = new QHBoxLayout();

    // 上个月按钮
    auto prev_button = new QPushButton("◀");
    prev_button->setFixedSize(25, 25);
    connect(prev_button, &QPushButton::clicked, this, &CalendarView::onPrevMonth);
    header_layout->addWidget(prev_button);

    // 年份选择
    year_spin_box = new QSpinBox();
    year_spin_box->setRange(1900, 2100);
    year_spin_box->setValue(m_current_month.year());
    connect(year_spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CalendarView::onYearChanged);
    header_layout->addWidget(year_spin_box);

    // 月份选择
    month_combo_box = new QComboBox();
    month_combo_box->addItems({"1月","2月","3月","4月","5月","6月",
                              "7月","8月","9月","10月","11月","12月"});
    month_combo_box->setCurrentIndex(m_current_month.month() - 1);
    connect(month_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalendarView::onMonthChanged);
    header_layout->addWidget(month_combo_box);

    // 月份年份显示
    month_year_label = new QLabel(m_current_month.toString("yyyy年 M月"));
    month_year_label->setStyleSheet("font-weight: bold; font-size: 14px;");
    header_layout->addWidget(month_year_label);

    // 下个月按钮
    auto next_button = new QPushButton("▶");
    next_button->setFixedSize(25, 25);
    connect(next_button, &QPushButton::clicked, this, &CalendarView::onNextMonth);
    header_layout->addWidget(next_button);

    main_layout->addLayout(header_layout);

    // 日历网格
    grid_layout = new QGridLayout();
    grid_layout->setSpacing(3);
    main_layout->addLayout(grid_layout);
}

void CalendarView::refreshCalendar()
{
    // 设置当前日期并构建日历
    calendar_util.setCurrentDate(QDate::currentDate());
    connect(&calendar_util, &Calendar::dateClicked, this, &CalendarView::dateClicked);

    calendar_util.buildCalendarGrid(grid_layout, m_current_month);
    month_year_label->setText(m_current_month.toString("yyyy年 M月"));
}

QDate CalendarView::selectedDate() const
{
    return calendar_util.getCurrentDate();
}

void CalendarView::setCurrentDate(const QDate& date)
{
    if (date.isValid()) {
        calendar_util.setCurrentDate(date);
        m_current_month = QDate(date.year(), date.month(), 1);
        year_spin_box->setValue(date.year());
        month_combo_box->setCurrentIndex(date.month() - 1);
        refreshCalendar();
    }
}

void CalendarView::onMonthChanged(int index)
{
    m_current_month.setDate(m_current_month.year(), index + 1, 1);
    refreshCalendar();
}

void CalendarView::onYearChanged(int year)
{
    m_current_month.setDate(year, m_current_month.month(), 1);
    refreshCalendar();
}

void CalendarView::onPrevMonth()
{
    m_current_month = m_current_month.addMonths(-1);
    year_spin_box->setValue(m_current_month.year());
    month_combo_box->setCurrentIndex(m_current_month.month() - 1);
    refreshCalendar();
}

void CalendarView::onNextMonth()
{
    m_current_month = m_current_month.addMonths(1);
    year_spin_box->setValue(m_current_month.year());
    month_combo_box->setCurrentIndex(m_current_month.month() - 1);
    refreshCalendar();
}