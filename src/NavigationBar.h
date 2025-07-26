#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H
#include <QPushButton>
#include <QHBoxLayout>
#include <QMap>
#include <QJsonObject>

class NavigationBar final : public QWidget
{
    Q_OBJECT

public:
    enum class NavButton {
        MAIN_VIEW,
        HABIT_MANAGE_VIEW,
        EVENT_MANAGE_VIEW,
        POMODORO_VIEW,
        TIMELINE_VIEW,
        CALENDAR_VIEW,
        SETTINGS_VIEW
    };

    explicit NavigationBar(QWidget* parent = nullptr);
    void loadTheme(const QJsonObject &theme);
    void setCurrentButton(NavButton button);

    signals:
        void buttonClicked(NavButton button);

private:
    void setupButtons();
    void updateButtonStyles();

    QHBoxLayout* layout;
    QMap<NavButton, QPushButton*> buttons;
    NavButton current_button_;
    QJsonObject current_theme_;
};

#endif //NAVIGATIONBAR_H
