//
// Created by qwert on 25-7-21.
//

#include "settings.h"

// ================== Settings类空实现 ==================
#include <vector>
#include <string>
#include "settings.h"

std::size_t Settings::getUserID() const {
    return user_id_;
}

std::string Settings::getUserNickname() const {
    return user_nickname_;
}

void Settings::setUserNickname(const std::string& nickname) {
    user_nickname_ = nickname;
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

bool Settings::addFriend(const FriendInfo& friend_info) {

    return false;
}

bool Settings::removeFriend(std::size_t friend_id) {

    return false;
}

std::vector<FriendInfo> Settings::getAllFriends() const {

    return {};
}

