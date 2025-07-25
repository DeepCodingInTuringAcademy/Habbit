#include "CalendarView.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

CalendarView::CalendarView(ServiceLayer* service, QWidget *parent)
    : QWidget(parent),
      m_service(service),
      m_today(QDate::currentDate()),
      m_selected_date(QDate::currentDate()),
      m_current_month(QDate::currentDate())
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

    calendar_util.setCurrentDate(m_selected_date);
    connect(&calendar_util, &Calendar::dateClicked, this, [this](const QDate &date) {
        m_selected_date = date;
        updateButtonStyles();
        emit dateClicked(date);
    });

    // 构建日历网格
    QDate firstDay(m_current_month.year(), m_current_month.month(), 1);
    int startCol = firstDay.dayOfWeek() % 7;
    QDate lastDay = firstDay.addMonths(1).addDays(-1);

    int row = 1;
    int col = startCol;
    for (int day = 1; day <= lastDay.day(); ++day)
    {
        QDate date(m_current_month.year(), m_current_month.month(), day);
        QString text = getDisplayTextForDate(date);

        QPushButton* btn = new QPushButton(text);
        btn->setProperty("date", date);
        btn->setMinimumSize(60, 50);

        // 设置初始样式
        if (date == m_today) {
            btn->setStyleSheet("background-color: #2196F3; color: white;");
        } else if (date == m_selected_date) {
            btn->setStyleSheet("background-color: #4CAF50; color: white;");
        }

        connect(btn, &QPushButton::clicked, [this, btn]() {
            m_selected_date = btn->property("date").toDate();
            updateButtonStyles();
            emit dateClicked(m_selected_date);
        });

        grid_layout->addWidget(btn, row, col);

        if (++col > 6) {
            col = 0;
            ++row;
        }
    }

    month_year_label->setText(m_current_month.toString("yyyy年 M月"));
}

void CalendarView::updateButtonStyles() const
{
    for (int i = 1; i < grid_layout->rowCount(); ++i) {
        for (int j = 0; j < grid_layout->columnCount(); ++j) {
            QLayoutItem* item = grid_layout->itemAtPosition(i, j);
            if (item && item->widget()) {
                QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
                if (btn) {
                    QDate btnDate = btn->property("date").toDate();
                    if (btnDate == m_today) {
                        btn->setStyleSheet("background-color: #2196F3; color: white;");
                    } else if (btnDate == m_selected_date) {
                        btn->setStyleSheet("background-color: #4CAF50; color: white;");
                    } else {
                        btn->setStyleSheet("");
                    }
                }
            }
        }
    }
}

QString CalendarView::getDisplayTextForDate(const QDate& date) const
{
    QString text = QString::number(date.day());

    if (m_service) {
        // 从服务层获取数据
        auto habits = m_service->getHabitsByDate(date);
        auto events = m_service->getEventsByDate(date);

        if (!habits.empty()) {
            text += "\n习惯:";
            for (const auto& h : habits) {
                text += "\n" + QString::fromStdString(h.name);
            }
        }

        if (!events.empty()) {
            text += "\n事项:";
            for (const auto& e : events) {
                text += "\n" + QString::fromStdString(e.title);
            }
        }
    }

    return text;
}


QDate CalendarView::selectedDate() const
{
    return m_selected_date;
}

void CalendarView::setCurrentDate(const QDate &date)
{
    if (date.isValid()) {
        m_selected_date = date;
        m_current_month = QDate(date.year(), date.month(), 1);
        year_spin_box->setValue(date.year());
        month_combo_box->setCurrentIndex(date.month() - 1);
        createCalendarGrid();
    }
}

void CalendarView::onMonthChanged(int index)
{
    m_current_month.setDate(m_current_month.year(), index + 1, 1);
    createCalendarGrid();
}

void CalendarView::onYearChanged(int year)
{
    m_current_month.setDate(year, m_current_month.month(), 1);
    createCalendarGrid();
}

void CalendarView::onPrevMonth()
{
    m_current_month = m_current_month.addMonths(-1);
    year_spin_box->setValue(m_current_month.year());
    month_combo_box->setCurrentIndex(m_current_month.month() - 1);
    createCalendarGrid();
}

void CalendarView::onNextMonth()
{
    m_current_month = m_current_month.addMonths(1);
    year_spin_box->setValue(m_current_month.year());
    month_combo_box->setCurrentIndex(m_current_month.month() - 1);
    createCalendarGrid();
}