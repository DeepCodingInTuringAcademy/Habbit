#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGroupBox>
#include <QGridLayout>
#include <QScrollArea>
#include <QVector>
#include "Habit.h"
#include "Event.h"
#include "PomodoroWidget.h"
#include "ServiceLayer.h"

class MainView final : public QWidget
{
    Q_OBJECT

    void initUI();
    void setupConnections();
    void setupHabitsSection();
    void setupEventsSection();
    void setupPomodoroSection();
    void setupMascotSection();

    QWidget* createHabitCard(const Habit& habit);
    QWidget* createEventCard(const Event& event);
    static void clearLayout(QLayout* layout);

    ServiceLayer& service_;
    PomodoroWidget& pomodoro_;

    // UI组件
    QGridLayout* mainLayout_;

    // 习惯区域
    QGroupBox* habitsGroup_;
    QScrollArea* habitsScrollArea_;
    QWidget* habitsContainer_;
    QVBoxLayout* habitsLayout_;

    // 事项区域
    QGroupBox* eventsGroup_;
    QScrollArea* eventsScrollArea_;
    QWidget* eventsContainer_;
    QVBoxLayout* eventsLayout_;

    // 番茄钟区域
    QGroupBox* pomodoroGroup_;
    QLabel* pomodoroTimeLabel_;
    QLabel* pomodoroRemarkLabel_;

    // 吉祥物区域
    QGroupBox* mascotGroup_;
    QLabel* mascotLabel_;
    QLabel* dialogLabel_;

    // 对话框文本
    QVector<QString> dialogTexts_;

    public:
    explicit MainView(ServiceLayer& service, PomodoroWidget& pomodoro, QWidget *parent = nullptr);

    void refreshAll();
    void refreshTodoArea();
    void refreshHabits();
    void refreshEvents();
    void refreshPomodoro() const;
    void refreshMascotDialog();

    signals:
    void habitCheckedIn(const Habit &habit);
    void habitEdited(const Habit &habit);
    void habitDeleted(const Habit &habit);
    void habitAddRequested();

    void eventFinished(const Event &event);
    void eventEdited(const Event &event);
    void eventDeleted(const Event &event);
    void eventAddRequested();
};

#endif // MAINVIEW_H