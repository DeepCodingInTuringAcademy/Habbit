#ifndef CALENDAR_H
#define CALENDAR_H
#include <QDate>
#include <QPushButton>
#include <QGridLayout>
#include <QList>

/**
 * @class Calendar
 * @brief 通用日历基类，提供月份管理、日期网格绘制、高亮选中等功能
 * @author 冰柠
 */
class Calendar final : public QObject
{
    Q_OBJECT
public:
    ~Calendar() override = default;
    explicit Calendar(QObject* parent = nullptr);

    /**
     * @brief 构建日历网格到布局中（会清除旧按钮）
     * @param layout 网格布局指针（一般是 QGridLayout）
     * @param display_month 要显示的年月
     */
    void buildCalendarGrid(QGridLayout* layout, const QDate& display_month);

    /**
     * @brief 当前选中的日期（用于高亮）
     */
    [[nodiscard]] QDate getCurrentDate() const;

    /**
     * @brief 设置选中日期（高亮）
     */
    void setCurrentDate(const QDate& date);

protected:
    QDate current_date;                      ///< 当前选中的日期
    QList<QPushButton*> date_buttons;        ///< 所有的日期按钮

    /**
     * @brief 为指定日期生成显示文本（可重写）
     */
    QString getDisplayTextForDate(const QDate& date) const;

    /**
     * @brief 日期点击后的处理逻辑（可重写）
     */
    void onDateClicked(QPushButton* sender_button);

    /**
     * @brief 给指定按钮添加高亮样式
     */
    void highlightButton(QPushButton* button);

    /**
     * @brief 清除原有按钮
     */
    void clearButtons(QGridLayout* layout);

    signals:
    void dateClicked(const QDate& date);
};

#endif // CALENDAR_H
