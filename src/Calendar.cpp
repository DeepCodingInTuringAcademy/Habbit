#include <QLabel>

#include "Calendar.h"

Calendar::Calendar(QObject* parent)
    : QObject(parent), current_date(QDate::currentDate())
{
}

void Calendar::buildCalendarGrid(QGridLayout* layout, const QDate& display_month)
{
    if (!layout || !display_month.isValid())
        return;

    clearButtons(layout);

    // 添加星期标题
    QStringList weekDays = {"日", "一", "二", "三", "四", "五", "六"};
    for (int i = 0; i < 7; ++i) {
        auto label = new QLabel(weekDays[i]);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedHeight(20);
        layout->addWidget(label, 0, i);
    }

    QDate firstDay(display_month.year(), display_month.month(), 1);
    int startCol = firstDay.dayOfWeek() % 7;
    QDate lastDay = firstDay.addMonths(1).addDays(-1);

    int row = 1;
    int col = startCol;
    for (int day = 1; day <= lastDay.day(); ++day)
    {
        QDate date(display_month.year(), display_month.month(), day);
        QString text = getDisplayTextForDate(date);

        QPushButton* btn = new QPushButton(text);
        btn->setProperty("date", date);
        btn->setMinimumSize(35, 50);

        QString style = getBtnStyle();

        if (date == current_date)
            style += getBtnStyle(btn_highlight::TODAY);

        if (date == current_date)
            style += getBtnStyle(btn_highlight::SELECTED);

        btn->setStyleSheet(style);
        connect(btn, &QPushButton::clicked, this, [this, btn] {
            onDateClicked(btn);
        });

        layout->addWidget(btn, row, col);
        date_buttons.append(btn);

        if (++col > 6) {
            col = 0;
            ++row;
        }
    }
}

void Calendar::clearButtons(QGridLayout* layout)
{
    for (auto* btn : date_buttons) {
        layout->removeWidget(btn);
        delete btn;
    }
    date_buttons.clear();
}

void Calendar::highlightButton(QPushButton* button)
{
    for (auto* btn : date_buttons) {
        btn->setStyleSheet("");
    }

    if (button)
        button->setStyleSheet("background-color: green; color: white;");
}

QDate Calendar::getCurrentDate() const
{
    return current_date;
}

void Calendar::setCurrentDate(const QDate& date)
{
    if (date.isValid())
        current_date = date;
}

QString Calendar::getDisplayTextForDate(const QDate& date) const
{
    // 默认仅显示数字
    return QString::number(date.day());
}

void Calendar::onDateClicked(const QPushButton* sender_button)
{
    if (!sender_button) return;

    current_date = sender_button->property("date").toDate();

    // 更新所有按钮样式
    for (auto* btn : date_buttons) {
        QDate btnDate = btn->property("date").toDate();
        QString style = getBtnStyle();

        if (btnDate == QDate::currentDate()) {
            style += getBtnStyle(btn_highlight::TODAY);
        }
        if (btnDate == current_date) {
            style += getBtnStyle(btn_highlight::SELECTED);
        }

        btn->setStyleSheet(style);
    }

    emit dateClicked(current_date);
}

QString Calendar::getBtnStyle(const btn_highlight type)
{
    switch (type)
    {
        default:
        case btn_highlight::DEFAULT:
            return R"(
                QPushButton {
                    font-size: 10px;
                    background-color: white;
                    border: 1px solid lightgray;
                    border-radius: 5px;
                }
            )";
        case btn_highlight::SELECTED:
            return R"(
                QPushButton {
                    border: 3px solid red;
                }
            )";
        case btn_highlight::TODAY:
            return R"(
                QPushButton {
                    background-color: lightgreen;
                }
            )";
    }
}
