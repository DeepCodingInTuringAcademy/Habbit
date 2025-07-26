#ifndef CALENDARWINWIDGET_H
#define CALENDARWINWIDGET_H
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include "Calendar.h"


class CalendarWinWidget final : public QWidget
{
    Q_OBJECT;

public:
    explicit CalendarWinWidget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QVBoxLayout* main_layout = nullptr;
    QGridLayout* calendar_layout = nullptr;
    Calendar* calendar_util = nullptr;

    QLabel* month_label = nullptr;
    QDate current_month;

    QPoint drag_position;

    void buildUI();
    void refreshCalendar() const;
};

#endif //CALENDARWINWIDGET_H
