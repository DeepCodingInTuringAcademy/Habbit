#include "CalendarView.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

CalendarView::CalendarView(QWidget *parent)
    : QWidget(parent),
      selected_date(QDate::currentDate()),
      current_month(QDate::currentDate())
{
    initUI();
}

void CalendarView::initUI()
{
    main_layout = new QVBoxLayout(this);

    // 月份导航
    auto header_layout = new QHBoxLayout();

    // 上个月按钮
    auto prev_button = new QPushButton("◀");
    connect(prev_button, &QPushButton::clicked, this, &CalendarView::onPrevMonth);
    header_layout->addWidget(prev_button);

    // 年份选择
    year_spin_box = new QSpinBox();
    year_spin_box->setRange(1900, 2100);
    year_spin_box->setValue(current_month.year());
    connect(year_spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CalendarView::onYearChanged);
    header_layout->addWidget(year_spin_box);

    // 月份选择
    month_combo_box = new QComboBox();
    month_combo_box->addItems({"1月","2月","3月","4月","5月","6月",
                              "7月","8月","9月","10月","11月","12月"});
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    connect(month_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalendarView::onMonthChanged);
    header_layout->addWidget(month_combo_box);

    // 月份年份显示
    month_year_label = new QLabel(current_month.toString("yyyy年 M月"));
    header_layout->addWidget(month_year_label);

    // 下个月按钮
    auto next_button = new QPushButton("▶");
    connect(next_button, &QPushButton::clicked, this, &CalendarView::onNextMonth);
    header_layout->addWidget(next_button);

    main_layout->addLayout(header_layout);

    // 日历网格
    grid_layout = new QGridLayout();
    createCalendarGrid();
    main_layout->addLayout(grid_layout);
}

void CalendarView::createCalendarGrid()
{
    // 清除旧按钮
    QLayoutItem* item;
    while ((item = grid_layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 添加星期标题
    QStringList weekDays = {"日", "一", "二", "三", "四", "五", "六"};
    for (int i = 0; i < 7; ++i) {
        auto label = new QLabel(weekDays[i]);
        label->setAlignment(Qt::AlignCenter);
        grid_layout->addWidget(label, 0, i);
    }

    calendar_util.setCurrentDate(selected_date);
    connect(&calendar_util, &Calendar::dateClicked, this, [this](const QDate &date) {
        selected_date = date;
        emit dateClicked(date);
    });

    calendar_util.buildCalendarGrid(grid_layout, current_month);
    month_year_label->setText(current_month.toString("yyyy年 M月"));
}

QDate CalendarView::selectedDate() const
{
    return selected_date;
}

void CalendarView::setCurrentDate(const QDate &date)
{
    if (date.isValid()) {
        selected_date = date;
        current_month = QDate(date.year(), date.month(), 1);
        year_spin_box->setValue(date.year());
        month_combo_box->setCurrentIndex(date.month() - 1);
        createCalendarGrid();
    }
}

void CalendarView::onMonthChanged(int index)
{
    current_month.setDate(current_month.year(), index + 1, 1);
    createCalendarGrid();
}

void CalendarView::onYearChanged(int year)
{
    current_month.setDate(year, current_month.month(), 1);
    createCalendarGrid();
}

void CalendarView::onPrevMonth()
{
    current_month = current_month.addMonths(-1);
    year_spin_box->setValue(current_month.year());
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    createCalendarGrid();
}

void CalendarView::onNextMonth()
{
    current_month = current_month.addMonths(1);
    year_spin_box->setValue(current_month.year());
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    createCalendarGrid();
}