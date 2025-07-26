#include "NavigationBar.h"
#include <QIcon>
#include <QSize>

NavigationBar::NavigationBar(QWidget *parent) : QWidget(parent), current_button_(NavButton::MAIN_VIEW)
{
    layout = new QHBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 5, 10, 5);
    
    setupButtons();
}

void NavigationBar::setupButtons()
{
    // 创建按钮
    buttons[NavButton::MAIN_VIEW] = new QPushButton("主页", this);
    buttons[NavButton::HABIT_MANAGE_VIEW] = new QPushButton("习惯管理", this);
    buttons[NavButton::EVENT_MANAGE_VIEW] = new QPushButton("事项管理", this);
    buttons[NavButton::POMODORO_VIEW] = new QPushButton("番茄钟", this);
    buttons[NavButton::TIMELINE_VIEW] = new QPushButton("时间线", this);
    buttons[NavButton::CALENDAR_VIEW] = new QPushButton("日历", this);
    buttons[NavButton::SETTINGS_VIEW] = new QPushButton("设置", this);

    // 添加到布局
    for (const auto button : buttons.values()) {
        layout->addWidget(button);
    }

    // 连接信号
    connect(buttons[NavButton::MAIN_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::MAIN_VIEW);
        emit buttonClicked(NavButton::MAIN_VIEW);
    });
    connect(buttons[NavButton::HABIT_MANAGE_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::HABIT_MANAGE_VIEW);
        emit buttonClicked(NavButton::HABIT_MANAGE_VIEW);
    });
    connect(buttons[NavButton::EVENT_MANAGE_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::EVENT_MANAGE_VIEW);
        emit buttonClicked(NavButton::EVENT_MANAGE_VIEW);
    });
    connect(buttons[NavButton::POMODORO_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::POMODORO_VIEW);
        emit buttonClicked(NavButton::POMODORO_VIEW);
    });
    connect(buttons[NavButton::TIMELINE_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::TIMELINE_VIEW);
        emit buttonClicked(NavButton::TIMELINE_VIEW);
    });
    connect(buttons[NavButton::CALENDAR_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::CALENDAR_VIEW);
        emit buttonClicked(NavButton::CALENDAR_VIEW);
    });
    connect(buttons[NavButton::SETTINGS_VIEW], &QPushButton::clicked, [this]() {
        setCurrentButton(NavButton::SETTINGS_VIEW);
        emit buttonClicked(NavButton::SETTINGS_VIEW);
    });
}

void NavigationBar::loadTheme(const QJsonObject &theme)
{
    current_theme_ = theme;
    updateButtonStyles();
}

void NavigationBar::setCurrentButton(NavButton button)
{
    current_button_ = button;
    updateButtonStyles();
}

void NavigationBar::updateButtonStyles()
{
    if (current_theme_.isEmpty()) return;

    const QJsonObject normal_style = current_theme_["normal"].toObject();
    const QJsonObject selected_style = current_theme_["selected"].toObject();

    // 定义按钮类型与图标键的映射关系
    static const QMap<NavButton, QString> button_icon_keys = {
        {NavButton::MAIN_VIEW, "home"},
        {NavButton::HABIT_MANAGE_VIEW, "habit"},
        {NavButton::EVENT_MANAGE_VIEW, "event"},
        {NavButton::POMODORO_VIEW, "pomodoro"},
        {NavButton::TIMELINE_VIEW, "timeline"},
        {NavButton::CALENDAR_VIEW, "calendar"},
        {NavButton::SETTINGS_VIEW, "settings"}
    };

    for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        const auto button = it.value();
        const bool is_selected = it.key() == current_button_;

        const QJsonObject& style = is_selected ? selected_style : normal_style;

        // 构建样式表
        QString style_sheet = QString(
            "QPushButton {"
            "   background-color: %1;"
            "   border: %2;"
            "   border-radius: %3;"
            "   color: %4;"
            "   font-size: %5px;"
            "   font-family: %6;"
            "   padding: %7px %8px;"
            "}"
            "QPushButton:hover {"
            "   background-color: %9;"
            "}"
        ).arg(
            style["background"].toString(),
            style["border"].toString(),
            style["border_radius"].toString(),
            style["text_color"].toString(),
            style["font_size"].toString(),
            style["font_family"].toString(),
            style["padding_vertical"].toString(),
            style["padding_horizontal"].toString(),
            style["hover_background"].toString()
        );

        button->setStyleSheet(style_sheet);

        // 获取图标配置
        const QJsonObject icons = style["icons"].toObject();
        const QString icon_key = button_icon_keys.value(it.key(), "");

        if (const QString icon_path = icons[icon_key].toString(); !icon_path.isEmpty()) {
            const QSize icon_size(
                style["icon_width"].toInt(24),  // 默认值24
                style["icon_height"].toInt(24)  // 默认值24
            );
            button->setIcon(QIcon(icon_path));
            button->setIconSize(icon_size);
        } else {
            button->setIcon(QIcon()); // 清除图标
        }
    }
}