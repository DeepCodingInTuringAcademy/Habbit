#include "SimpleAuthSystem.h"
#include <QMessageBox>
#include <QJsonParseError>

SimpleAuthSystem::SimpleAuthSystem(Settings &settings_ref, QWidget *parent)
    : QWidget(parent), userId(0), settings(settings_ref)
{
    // 设置服务器URL
    serverUrl = "http://47.102.155.138:5000";

    // 创建UI组件
    QVBoxLayout *layout = new QVBoxLayout(this);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("用户名");
    layout->addWidget(usernameEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordEdit);

    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("邮箱 (注册用)");
    layout->addWidget(emailEdit);

    nicknameEdit = new QLineEdit(this);
    nicknameEdit->setPlaceholderText("昵称");
    layout->addWidget(nicknameEdit);

    skinEdit = new QLineEdit(this);
    skinEdit->setPlaceholderText("皮肤 (default/dark/light)");
    layout->addWidget(skinEdit);

    loginButton = new QPushButton("登录", this);
    layout->addWidget(loginButton);

    registerButton = new QPushButton("注册", this);
    layout->addWidget(registerButton);

    updateButton = new QPushButton("更新设置", this);
    layout->addWidget(updateButton);

    logoutButton = new QPushButton("登出", this);
    layout->addWidget(logoutButton);

    statusLabel = new QLabel(this);
    layout->addWidget(statusLabel);

    // 连接信号和槽
    connect(loginButton, &QPushButton::clicked, this, &SimpleAuthSystem::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &SimpleAuthSystem::onRegisterClicked);
    connect(updateButton, &QPushButton::clicked, this, &SimpleAuthSystem::onUpdateSettingsClicked);
    connect(logoutButton, &QPushButton::clicked, this, &SimpleAuthSystem::onLogoutClicked);

    setWindowTitle("简单认证系统");
    resize(300, 400);
}

// 辅助函数：发送HTTP请求
void SimpleAuthSystem::sendHttpRequest(const QString &endpoint, const QString &data,
                                       const QString &method, const QString &token,
                                       std::function<void(bool, const QJsonDocument &)> callback)
{
    QProcess *process = new QProcess(this);

    QStringList args;
    args << "-s";           // 静默模式
    args << "-X" << method; // 请求方法
    args << "-H" << "Content-Type: application/json";

    if (!token.isEmpty())
    {
        args << "-H" << QString("Authorization: Bearer %1").arg(token);
    }

    if (!data.isEmpty())
    {
        args << "-d" << data;
    }

    QString url = serverUrl + endpoint;
    args << url;

    statusLabel->setText("正在发送请求...");

    connect(process, &QProcess::finished, this, [=, this](int exitCode, QProcess::ExitStatus exitStatus)
            {
        QString response = QString::fromUtf8(process->readAllStandardOutput());
        QString error = QString::fromUtf8(process->readAllStandardError());
        
        bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
        
        if (!success) {
            statusLabel->setText("请求失败: " + error);
            process->deleteLater();
            return;
        }
        
        // 解析JSON响应
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &jsonError);
        
        if (jsonError.error != QJsonParseError::NoError) {
            statusLabel->setText("JSON解析错误: " + jsonError.errorString());
            process->deleteLater();
            return;
        }
        
        callback(true, jsonDoc);
        process->deleteLater(); });

    process->start("curl", args);
}

void SimpleAuthSystem::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        statusLabel->setText("用户名和密码不能为空");
        return;
    }

    // 准备JSON数据
    QJsonObject jsonObj;
    jsonObj["username"] = username;
    jsonObj["password"] = password;
    QJsonDocument doc(jsonObj);
    QString jsonData = QString::fromUtf8(doc.toJson());

    // 发送请求
    sendHttpRequest("/api/auth/login", jsonData, "POST", "", [this](bool success, const QJsonDocument &response)
                    {
        if (!success) return;
        
        QJsonObject responseObj = response.object();
        
        if (responseObj["status"].toString() == "success") {
            accessToken = responseObj["access_token"].toString();
            userId = responseObj["user"].toObject()["id"].toInt();
            QString nickname = responseObj["user"].toObject()["nickname"].toString();

            settings.setUserID(userId);
            settings.setUserNickname(nickname.toStdString());

            statusLabel->setText("登录成功");
        } else {
            QString message = responseObj.contains("message") ? 
                              responseObj["message"].toString() : "未知错误";
            statusLabel->setText("登录失败: " + message);
        } });
}

void SimpleAuthSystem::onRegisterClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString email = emailEdit->text().trimmed();
    QString nickname = nicknameEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty() || email.isEmpty())
    {
        statusLabel->setText("用户名、密码和邮箱不能为空");
        return;
    }

    // 准备JSON数据
    QJsonObject jsonObj;
    jsonObj["username"] = username;
    jsonObj["password"] = password;
    jsonObj["email"] = email;
    if (!nickname.isEmpty())
    {
        jsonObj["nickname"] = nickname;
    }
    QJsonDocument doc(jsonObj);
    QString jsonData = QString::fromUtf8(doc.toJson());

    // 发送请求
    sendHttpRequest("/api/auth/register", jsonData, "POST", "", [this](bool success, const QJsonDocument &response)
                    {
        if (!success) return;
        
        QJsonObject responseObj = response.object();
        
        if (responseObj["status"].toString() == "success") {
            accessToken = responseObj["access_token"].toString();
            userId = responseObj["user"].toObject()["id"].toInt();

            QString nickname = responseObj["user"].toObject()["nickname"].toString();
            settings.setUserID(userId);
            settings.setUserNickname(nickname.toStdString());
            statusLabel->setText("注册成功");
        } else {
            QString message = responseObj.contains("message") ? 
                              responseObj["message"].toString() : "未知错误";
            statusLabel->setText("注册失败: " + message);
        } });
}

void SimpleAuthSystem::onUpdateSettingsClicked()
{
    if (accessToken.isEmpty())
    {
        statusLabel->setText("请先登录");
        return;
    }

    QString nickname = nicknameEdit->text().trimmed();
    QString skin = skinEdit->text().trimmed();

    if (nickname.isEmpty())
    {
        statusLabel->setText("昵称不能为空");
        return;
    }

    // 准备JSON数据
    QJsonObject jsonObj;
    jsonObj["nickname"] = nickname;
    jsonObj["current_skin"] = skin.isEmpty() ? "default" : skin;
    QJsonDocument doc(jsonObj);
    QString jsonData = QString::fromUtf8(doc.toJson());

    // 发送请求
    sendHttpRequest("/api/auth/settings", jsonData, "PUT", accessToken, [this](bool success, const QJsonDocument &response)
                    {
        if (!success) return;
        
        QJsonObject responseObj = response.object();
        
        if (responseObj["status"].toString() == "success") {
            statusLabel->setText("设置更新成功");
        } else {
            QString message = responseObj.contains("message") ? 
                              responseObj["message"].toString() : "未知错误";
            statusLabel->setText("更新设置失败: " + message);
        } });
}

void SimpleAuthSystem::onLogoutClicked()
{
    if (accessToken.isEmpty())
    {
        statusLabel->setText("未登录");
        return;
    }

    // 发送请求
    sendHttpRequest("/api/auth/logout", "", "POST", accessToken, [this](bool success, const QJsonDocument &response)
                    {
        // 无论服务器响应如何，都清除本地令牌
        accessToken = "";
        userId = 0;
        statusLabel->setText("已登出"); });
}
