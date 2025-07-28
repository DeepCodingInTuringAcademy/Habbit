#ifndef SIMPLEAUTHSYSTEM_H
#define SIMPLEAUTHSYSTEM_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <functional>
#include "Settings.h"

class SimpleAuthSystem : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleAuthSystem(Settings &settings_ref, QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onUpdateSettingsClicked();
    void onLogoutClicked();

private:
    Settings &settings;
    // UI组件
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *emailEdit;
    QLineEdit *nicknameEdit;
    QLineEdit *skinEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *updateButton;
    QPushButton *logoutButton;
    QLabel *statusLabel;

    // 存储数据
    QString serverUrl;
    QString accessToken;
    int userId;

    // 辅助函数：发送HTTP请求
    void sendHttpRequest(const QString &endpoint, const QString &data,
                         const QString &method, const QString &token,
                         std::function<void(bool, const QJsonDocument &)> callback);
};

#endif // SIMPLEAUTHSYSTEM_H
