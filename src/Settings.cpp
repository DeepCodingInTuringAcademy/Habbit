//
// Created by qwert on 25-7-21.
//

// ================== Settings类空实现 ==================
#include <vector>
#include <string>
#include <QDateTime>
#include "Settings.h"

Settings g_settings;

std::size_t Settings::getUserID() const
{
    return user_id_;
}

std::string Settings::getUserNickname() const
{
    return user_nickname_;
}

void Settings::setUserNickname(const std::string &nickname)
{
    user_nickname_ = nickname;
}

void Settings::logout()
{
    // 获取当前登录的用户ID
    DBLayer db_layer;
    std::size_t current_user_id = db_layer.getCurrentUserID();

    // 如果没有用户登录，直接返回
    if (current_user_id == 0)
    {
        return;
    }

    // 获取当前用户的设置
    UserSettings user_settings = db_layer.getUserSettings(current_user_id);

    // 更新登录状态为未登录
    user_settings.is_logged_in = false;

    // 记录登出时间
    QDateTime current_time = QDateTime::currentDateTime();
    user_settings.last_login_time = current_time.toString("yyyy-MM-dd HH:mm:ss").toStdString();

    // 更新用户设置到数据库
    db_layer.updateUserSettings(user_settings);

    // 清除当前用户信息
    user_id_ = 0;
    user_nickname_ = "";
}

bool Settings::syncData()
{
    // TODO: 同步本地与服务器数据
    return false;
}

std::string Settings::getCurrentSkin() const
{
    // TODO: 获取当前UI皮肤名
    return {};
}

void Settings::setSkin(const std::string &skin_name)
{
    // TODO: 切换UI皮肤
}

bool Settings::integrateEventsToSystemCalendar()
{
    // TODO: 集成事项到系统日历
    return false;
}

bool Settings::setDDLReminder(bool enable, const std::string &email)
{
    // 获取当前登录的用户ID
    DBLayer db_layer;
    std::size_t current_user_id = db_layer.getCurrentUserID();

    // 如果没有用户登录，返回失败
    if (current_user_id == 0)
    {
        return false;
    }

    // 验证邮箱格式（如果启用且提供了邮箱）
    if (enable && !email.empty())
    {
        // 简单的邮箱格式验证
        if (email.find('@') == std::string::npos || email.find('.') == std::string::npos)
        {
            return false; // 邮箱格式不正确
        }
    }

    // 获取当前用户的设置
    UserSettings user_settings = db_layer.getUserSettings(current_user_id);

    // 更新DDL提醒设置
    user_settings.ddl_reminder_enabled = enable;
    if (!email.empty())
    {
        user_settings.ddl_reminder_email = email;
    }
    else if (enable && user_settings.ddl_reminder_email.empty())
    {
        // 如果启用提醒但没有提供邮箱，且之前也没有设置过邮箱，则返回失败
        return false;
    }

    // 更新用户设置到数据库
    bool success = db_layer.updateUserSettings(user_settings);

    return success;
}

bool Settings::addFriend(const FriendInfo &friend_info)
{

    return false;
}

bool Settings::removeFriend(std::size_t friend_id)
{

    return false;
}

std::vector<FriendInfo> Settings::getAllFriends() const
{

    return {};
}

void Settings::setUserID(int user_id)
{
    user_id_ = user_id;
}
