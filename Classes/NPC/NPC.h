#ifndef __NPC_H__
#define __NPC_H__

#include "cocos2d.h"

class NPC : public cocos2d::Node {
public:
    NPC();
    ~NPC();

    // 初始化NPC，设置名字、好感度、位置、对话内容
    bool init(const cocos2d::Vec2& position, const std::string& name, int affection,
        const std::vector<std::string>& chatTexts);

    // 创建NPC对象
    static NPC* create(const cocos2d::Vec2& position, const std::string& name, int affection,
        const std::vector<std::string>& chatTexts);

    // NPC交互功能：触发聊天
    void startChat();

    // 处理玩家与NPC的交互
    void interact();

    // 访问NPC的聊天状态
    bool isChattingStatus() const { return isChatting; }

    // 获取NPC的名字
    std::string getNPCName() const { return npcName; }

    // 获取NPC的好感度
    int getAffection() const { return affection; }

    void showChatButton(bool show);


private:
    cocos2d::Sprite* npcSprite; // NPC精灵
    std::string npcName;        // NPC名字
    int affection;              // NPC对玩家的好感度
    cocos2d::MenuItemLabel* chatButton; // “聊天”按钮
    std::vector<std::string> chatMessages; // 聊天文本
    size_t currentChatIndex;   // 当前聊天文本索引
    bool isChatting;           // 是否正在聊天
    cocos2d::Vec2 position;    // NPC位置
    cocos2d::Label* currentChatLabel;  // 用于存储当前显示的聊天标签

};

#endif // __NPC_H__