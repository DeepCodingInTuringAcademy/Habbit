#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QComboBox>
#include <QLabel>
#include <QSpinBox>

#include "Calendar.h"
#include "ServiceLayer.h"

class CalendarView final : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarView(ServiceLayer* service, QWidget *parent = nullptr);
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
    void updateButtonStyles() const;
    QString getDisplayTextForDate(const QDate& date) const;

    Calendar calendar_util;
    ServiceLayer* m_service;
    QDate m_today;
    QDate m_selected_date;
    QDate m_current_month;

    QVBoxLayout* main_layout = nullptr;
    QGridLayout* grid_layout = nullptr;
    QLabel* month_year_label = nullptr;
    QSpinBox* year_spin_box = nullptr;
    QComboBox* month_combo_box = nullptr;
};

#endif // CALENDARVIEW_H