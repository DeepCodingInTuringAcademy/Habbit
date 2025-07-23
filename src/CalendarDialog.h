#ifndef HABBIT_CALENDARDIALOG_H
#define HABBIT_CALENDARDIALOG_H
#include <QDialog>
#include <QDate>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

class CalendarDialog : public QDialog
{
    Q_OBJECT;

public:
    explicit CalendarDialog(QWidget *parent = nullptr);
    [[nodiscard]] QDate selectedDate() const;
    void setSelectedDate(const QDate &date);

private slots:
    void updateCalendar();
    void onDateClicked();
    void onMonthChanged(int index);
    void onYearChanged(int value);
    void onPrevMonth();
    void onNextMonth();

private:
    void createHeader();
    void createCalendarGrid();
    void createFooter();

    QDate current_date;
    QGridLayout* grid_layout;
    QLabel* month_year_label;
    QSpinBox* year_spin_box;
    QComboBox* month_combo_box;
    QList<QPushButton*> date_buttons;
};

#endif //HABBIT_CALENDARDIALOG_H
