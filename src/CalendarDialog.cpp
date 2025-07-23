#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

#include "CalendarDialog.h"

CalendarDialog::CalendarDialog(QWidget* parent)
        : QDialog(parent),
          current_date(QDate::currentDate())
{
    setWindowTitle("选择日期");
    setFixedSize(400, 400);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // 创建头部（年月选择和导航按钮）
    createHeader();
    mainLayout->addLayout(grid_layout);

    // 创建日历网格（包含星期标题和日期按钮）
    createCalendarGrid();
    mainLayout->addLayout(grid_layout, 1);

    // 创建底部（确定/取消按钮）
    createFooter();
}

QDate CalendarDialog::selectedDate() const {
    return current_date;
}

void CalendarDialog::setSelectedDate(const QDate &date) {
    if (date.isValid()) {
        current_date = date;
        year_spin_box->setValue(date.year());
        month_combo_box->setCurrentIndex(date.month() - 1);
        updateCalendar();
    }
}

void CalendarDialog::createHeader()
{
    auto header_layout = new QHBoxLayout;

    // 上个月按钮
    auto prev_button = new QPushButton("◀");
    connect(prev_button, &QPushButton::clicked, this, &CalendarDialog::onPrevMonth);
    header_layout->addWidget(prev_button);

    // 年月显示和选择
    month_year_label = new QLabel;
    month_year_label->setAlignment(Qt::AlignCenter);

    year_spin_box = new QSpinBox;
    year_spin_box->setRange(1900, 2100);
    year_spin_box->setValue(current_date.year());
    connect(year_spin_box, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CalendarDialog::onYearChanged);

    month_combo_box = new QComboBox;
    month_combo_box->addItems(QStringList() << "1月" << "2月" << "3月" << "4月"
                                          << "5月" << "6月" << "7月" << "8月"
                                          << "9月" << "10月" << "11月" << "12月");
    month_combo_box->setCurrentIndex(current_date.month() - 1);
    connect(month_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CalendarDialog::onMonthChanged);

    header_layout->addWidget(year_spin_box);
    header_layout->addWidget(month_combo_box);
    header_layout->addWidget(month_year_label);

    // 下个月按钮
    auto nextButton = new QPushButton("▶");
    connect(nextButton, &QPushButton::clicked, this, &CalendarDialog::onNextMonth);
    header_layout->addWidget(nextButton);

    grid_layout = new QGridLayout;
    grid_layout->addLayout(header_layout, 0, 0, 1, 7);
}

void CalendarDialog::createCalendarGrid()
{
    // 清除旧的日期按钮
    for (auto button : date_buttons) {
        delete button;
    }
    date_buttons.clear();

    // 获取当前月份的第一天和最后一天
    QDate firstDay(current_date.year(), current_date.month(), 1);
    QDate lastDay = firstDay.addMonths(1).addDays(-1);

    // 计算第一天是星期几 (0=周日, 1=周一, ..., 6=周六)
    int firstDayOfWeek = firstDay.dayOfWeek() % 7;

    // 创建星期标题行
    QStringList weekDays = {"日", "一", "二", "三", "四", "五", "六"};
    for (int i = 0; i < 7; ++i) {
        QLabel *dayLabel = new QLabel(weekDays[i]);
        dayLabel->setAlignment(Qt::AlignCenter);
        grid_layout->addWidget(dayLabel, 1, i); // 第1行显示星期标题
    }

    // 创建日期按钮
    int row = 2; // 从第2行开始放置日期按钮
    int col = firstDayOfWeek;
    for (int day = 1; day <= lastDay.day(); ++day) {
        auto button = new QPushButton(QString::number(day));
        button->setProperty("date", QDate(current_date.year(), current_date.month(), day));
        connect(button, &QPushButton::clicked, this, &CalendarDialog::onDateClicked);

        if (day == current_date.day()) {
            button->setStyleSheet("background-color: #4CAF50; color: white;");
        }

        grid_layout->addWidget(button, row, col);
        date_buttons.append(button);

        col++;
        if (col > 6) {
            col = 0;
            row++;
        }
    }

    // 更新年月显示
    month_year_label->setText(current_date.toString("yyyy年 M月"));
}

void CalendarDialog::createFooter()
{
    auto footerLayout = new QHBoxLayout;

    auto okButton = new QPushButton("确定");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    auto cancelButton = new QPushButton("取消");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    footerLayout->addStretch();
    footerLayout->addWidget(okButton);
    footerLayout->addWidget(cancelButton);
    footerLayout->addStretch();

    int lastRow = grid_layout->rowCount();
    grid_layout->addLayout(footerLayout, lastRow, 0, 1, 7);
}

void CalendarDialog::updateCalendar()
{
    createCalendarGrid();
}

void CalendarDialog::onDateClicked()
{
    auto button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // 重置所有按钮样式
        for (auto btn : date_buttons) {
            btn->setStyleSheet("");
        }

        // 设置选中按钮样式
        button->setStyleSheet("background-color: #4CAF50; color: white;");

        // 更新当前选中日期
        current_date = button->property("date").toDate();
    }
}

void CalendarDialog::onMonthChanged(int index)
{
    current_date = QDate(current_date.year(), index + 1, 1);
    updateCalendar();
}

void CalendarDialog::onYearChanged(int value)
{
    current_date = QDate(value, current_date.month(), 1);
    updateCalendar();
}

void CalendarDialog::onPrevMonth()
{
    current_date = current_date.addMonths(-1);
    year_spin_box->setValue(current_date.year());
    month_combo_box->setCurrentIndex(current_date.month() - 1);
    updateCalendar();
}

void CalendarDialog::onNextMonth()
{
    current_date = current_date.addMonths(1);
    year_spin_box->setValue(current_date.year());
    month_combo_box->setCurrentIndex(current_date.month() - 1);
    updateCalendar();
}