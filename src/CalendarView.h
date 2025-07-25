#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QComboBox>
#include <QLabel>
#include <QSpinBox>

#include "Calendar.h"

class CalendarView : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarView(QWidget *parent = nullptr);
    ~CalendarView() override = default;

    void setCurrentDate(const QDate &date);
    QDate selectedDate() const;

    signals:
        void dateClicked(const QDate &date);

private slots:
    void onMonthChanged(int index);
    void onYearChanged(int year);
    void onPrevMonth();
    void onNextMonth();

private:
    void initUI();
    void createCalendarGrid();

    Calendar calendar_util;
    QDate current_month;
    QDate selected_date;

    QVBoxLayout* main_layout = nullptr;
    QGridLayout* grid_layout = nullptr;
    QLabel* month_year_label = nullptr;
    QSpinBox* year_spin_box = nullptr;
    QComboBox* month_combo_box = nullptr;
};

#endif // CALENDARVIEW_H