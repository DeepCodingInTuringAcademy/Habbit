#include "CalendarDialog.h"


CalendarDialog::CalendarDialog(QWidget* parent)
    : QDialog(parent),
      selected_date(QDate::currentDate()),
      current_month(QDate::currentDate())
{
    setWindowTitle("选择日期");
    setFixedSize(400, 400);

    main_layout = new QVBoxLayout(this);

    createHeader();

    grid_layout = new QGridLayout();
    createCalendarGrid();
    main_layout->addLayout(grid_layout);

    createFooter();
}

QDate CalendarDialog::selectedDate() const
{
    return selected_date;
}

void CalendarDialog::setSelectedDate(const QDate& date)
{
    if (date.isValid()) {
        selected_date = date;
        current_month = QDate(date.year(), date.month(), 1);
        year_spin_box->setValue(date.year());
        month_combo_box->setCurrentIndex(date.month() - 1);
        createCalendarGrid();
    }
}

void CalendarDialog::createHeader()
{
    auto header_layout = new QHBoxLayout();

    auto prev_button = new QPushButton("◀");
    connect(prev_button, &QPushButton::clicked, this, &CalendarDialog::onPrevMonth);
    header_layout->addWidget(prev_button);

    year_spin_box = new QSpinBox;
    year_spin_box->setRange(1900, 2100);
    year_spin_box->setValue(current_month.year());
    connect(year_spin_box, QOverload<int>::of(&QSpinBox::valueChanged), this, &CalendarDialog::onYearChanged);
    header_layout->addWidget(year_spin_box);

    month_combo_box = new QComboBox;
    month_combo_box->addItems({"1月","2月","3月","4月","5月","6月","7月","8月","9月","10月","11月","12月"});
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    connect(month_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalendarDialog::onMonthChanged);
    header_layout->addWidget(month_combo_box);

    month_year_label = new QLabel(current_month.toString("yyyy年 M月"));
    header_layout->addWidget(month_year_label);

    auto next_button = new QPushButton("▶");
    connect(next_button, &QPushButton::clicked, this, &CalendarDialog::onNextMonth);
    header_layout->addWidget(next_button);

    main_layout->addLayout(header_layout);
}

void CalendarDialog::createCalendarGrid()
{
    // 清除旧按钮
    QLayoutItem* item;
    while ((item = grid_layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    calendar_util.setCurrentDate(selected_date);

    // 连接信号
    connect(&calendar_util, &Calendar::dateClicked, this, [this](const QDate& date) {
        selected_date = date;
    });

    calendar_util.buildCalendarGrid(grid_layout, current_month);
    month_year_label->setText(current_month.toString("yyyy年 M月"));
}

void CalendarDialog::createFooter()
{
    const auto footer_layout = new QHBoxLayout();
    footer_layout->addStretch();

    const auto ok_btn = new QPushButton("确定");
    connect(ok_btn, &QPushButton::clicked, this, &QDialog::accept);
    footer_layout->addWidget(ok_btn);

    const auto cancelBtn = new QPushButton("取消");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    footer_layout->addWidget(cancelBtn);

    footer_layout->addStretch();
    main_layout->addLayout(footer_layout);
}

void CalendarDialog::onPrevMonth()
{
    current_month = current_month.addMonths(-1);
    year_spin_box->setValue(current_month.year());
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    createCalendarGrid();
}

void CalendarDialog::onNextMonth()
{
    current_month = current_month.addMonths(1);
    year_spin_box->setValue(current_month.year());
    month_combo_box->setCurrentIndex(current_month.month() - 1);
    createCalendarGrid();
}

void CalendarDialog::onYearChanged(int year)
{
    current_month.setDate(year, current_month.month(), 1);
    createCalendarGrid();
}

void CalendarDialog::onMonthChanged(int index)
{
    current_month.setDate(current_month.year(), index + 1, 1);
    createCalendarGrid();
}
