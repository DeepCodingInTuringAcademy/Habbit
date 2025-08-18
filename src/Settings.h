/**
 * @file Settings.h
 * @brief 软件设置相关功能声明，包含用户信息、数据同步、UI皮肤、事项集成、DDL提醒、多平台移植、好友管理、禅定模式等
 * @date 2025年07月21日
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include "DBLayer.h"

/**
 * @struct FriendInfo
 * @brief 好友信息结构体
 */
struct FriendInfo
{
    std::size_t id;       /**< 好友唯一标识ID */
    std::string nickname; /**< 好友昵称 */
    std::string remark;   /**< 备注 */
};

/**
 * @class Settings
 * @brief 软件设置类，管理各项设置功能
 */
class Settings
{
public:
    /**
     * @brief 获取当前用户ID
     * @author 遥远
     * @return 用户ID
     */
    std::size_t getUserID() const;

    /**
     * @brief 获取当前用户昵称
     * @author 遥远
     * @return 用户昵称
     */
    std::string getUserNickname() const;

    /**
     * @brief 设置用户昵称
     * @author 遥远
     * @param nickname 新昵称
     */
    void setUserNickname(const std::string &nickname);

    /**
     * @brief 注销当前账号
     * @author XTUG
     */
    void logout();

    /**
     * @brief 同步本地与服务器数据
     * @author Darling
     * @return 是否同步成功
     */
    bool syncData();

    /**
     * @brief 获取当前UI皮肤名
     * @author Darling
     * @return 皮肤名称
     */
    std::string getCurrentSkin() const;

    /**
     * @brief 切换UI皮肤
     * @param skin_name 皮肤名称
     * @author Darling
     */
    void setSkin(const std::string &skin_name);

    /**
     * @brief 集成事项到系统日历
     * @author Darling
     * @return 是否集成成功
     */
    bool integrateEventsToSystemCalendar();

    /**
     * @brief 设置DDL提醒（邮件/短信）
     * @author XTUG
     * @param enable 是否启用
     * @param email 邮箱地址
     * @return 是否设置成功
     */
    bool setDDLReminder(bool enable, const std::string &email = "");

    /**
     * @brief 添加好友
     * @author
     * @param friend_info 好友信息
     * @return 是否添加成功
     */
    bool addFriend(const FriendInfo &friend_info);

    /**
     * @brief 删除好友
     * @author
     * @param friend_id 好友ID
     * @return 是否删除成功
     */
    bool removeFriend(std::size_t friend_id);

    /**
     * @brief 查询所有好友信息
     * @author
     * @return 好友信息列表
     */
    std::vector<FriendInfo> getAllFriends() const;

    void setUserID(int user_id);

private:
    std::size_t user_id_;
    std::string user_nickname_;
};

#endif // SETTINGS_H
