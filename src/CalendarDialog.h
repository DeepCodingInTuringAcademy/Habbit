// CalendarDialog.h
#ifndef HABBIT_CALENDARDIALOG_H
#define HABBIT_CALENDARDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include "Calendar.h"

class CalendarDialog final : public QDialog
{
    Q_OBJECT
public:
    explicit CalendarDialog(QWidget* parent = nullptr);
    [[nodiscard]] QDate selectedDate() const;
    void setSelectedDate(const QDate& date);

private:
    Calendar calendar_util;
    QDate selected_date;
    QDate current_month;

    QVBoxLayout* main_layout = nullptr;
    QGridLayout* grid_layout = nullptr;
    QLabel* month_year_label = nullptr;
    QSpinBox* year_spin_box = nullptr;
    QComboBox* month_combo_box = nullptr;

    void createHeader();
    void refreshCalendar();
    void createFooter();

private slots:
    void onMonthChanged(int index);
    void onYearChanged(int year);
    void onPrevMonth();
    void onNextMonth();
};

#endif // HABBIT_CALENDARDIALOG_H