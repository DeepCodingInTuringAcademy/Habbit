//
// Created by qwert on 25-7-21.
//

#include "settings.h"

// ================== Settings类空实现 ==================
#include <vector>
#include <string>
#include "settings.h"

std::size_t Settings::getUserID() const {
    // TODO: 获取当前用户ID
    return 0;
}

std::string Settings::getUserNickname() const {
    // TODO: 获取当前用户昵称
    return {};
}

void Settings::setUserNickname(const std::string& nickname) {
    // TODO: 设置用户昵称
}

void Settings::logout() {
    // TODO: 注销当前账号
}

bool Settings::syncData() {
    // TODO: 同步本地与服务器数据
    return false;
}

std::string Settings::getCurrentSkin() const {
    // TODO: 获取当前UI皮肤名
    return {};
}

void Settings::setSkin(const std::string& skin_name) {
    // TODO: 切换UI皮肤
}

bool Settings::integrateEventsToSystemCalendar() {
    // TODO: 集成事项到系统日历
    return false;
}

bool Settings::setDDLReminder(bool enable, const std::string& email) {
    // TODO: 设置DDL提醒（邮件/短信）
    return false;
}

std::string Settings::getSupportedPlatforms() const {
    // TODO: 获取支持的平台信息
    return {};
}

bool Settings::addFriend(const FriendInfo& friend_info) {
    // TODO: 添加好友
    return false;
}

bool Settings::removeFriend(std::size_t friend_id) {
    // TODO: 删除好友
    return false;
}

std::vector<FriendInfo> Settings::getAllFriends() const {
    // TODO: 查询所有好友信息
    return {};
}

void Settings::setZenMode(bool enable) {
    // TODO: 启用或关闭禅定模式
}

bool Settings::isZenModeEnabled() const {
    // TODO: 查询禅定模式状态
    return false;
}
