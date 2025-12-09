#include "NPC.h"


NPC::NPC()
    : npcSprite(nullptr), npcName("Unnamed"), affection(0), currentChatIndex(0), isChatting(false) {}

NPC::~NPC() {}

bool NPC::init(const cocos2d::Vec2& position, const std::string& name, int affection,
    const std::vector<std::string>& chatTexts) {

    // 创建NPC精灵
    npcSprite = cocos2d::Sprite::create("Bouncer.png"); 
    if (!npcSprite) return false;  
    this->setPosition(position); 
    this->addChild(npcSprite); 

    // 初始化NPC属性
    npcName = name;  
    this->affection = affection;  
    chatMessages = chatTexts; 
    currentChatIndex = 0; 
    currentChatLabel = nullptr;  

    // 创建聊天按钮
    chatButton = cocos2d::MenuItemLabel::create(
        cocos2d::Label::createWithSystemFont("Chat", "Arial", 24),  
        CC_CALLBACK_0(NPC::interact, this)  
    );
    chatButton->setScale(0.5); // 放大四倍

    // 创建菜单并添加聊天按钮
    auto menu = cocos2d::Menu::create(chatButton, nullptr); 
    menu->setPosition(cocos2d::Vec2(0, -50));  

    this->addChild(menu, 16);
    //this->addChild(menu);  

    // 隐藏聊天按钮
    chatButton->setVisible(false);

    return true; 
}




NPC* NPC::create(const cocos2d::Vec2& position, const std::string& name, int affection,
    const std::vector<std::string>& chatTexts) {

    //  创建 NPC 对象
    NPC* npc = new NPC();  

    // 初始化 NPC 对象
    if (npc && npc->init(position, name, affection, chatTexts)) {
        npc->autorelease();  
        return npc;  
    }
    else {
        delete npc;  
        return nullptr;  
    }
}


void NPC::startChat() {
    if (currentChatIndex < chatMessages.size()) {
        if (currentChatLabel) {
            currentChatLabel->removeFromParent(); 
        }

        // 输出当前聊天内容
        cocos2d::log("NPC [%s]: %s", npcName.c_str(), chatMessages[currentChatIndex].c_str());

        // 创建新的聊天标签并显示
        currentChatLabel = cocos2d::Label::createWithTTF(chatMessages[currentChatIndex], "fonts/arial.ttf", 24);
        currentChatLabel->setPosition(getPosition() + cocos2d::Vec2(0, 100)); 
        currentChatLabel->setScale(2); 
        currentChatLabel->setTextColor(cocos2d::Color4B::BLACK);
        this->getParent()->addChild(currentChatLabel);  

        // 更新聊天索引
        currentChatIndex++;
        currentChatLabel->setVisible(true);
    }
    else {
        // 如果没有更多对话，结束对话
        cocos2d::log("End of conversation with NPC [%s].", npcName.c_str());
        currentChatIndex = 0;
        currentChatLabel->setVisible(false);
        isChatting = false;  // 结束聊天

    }
}


void NPC::interact() {
    // 添加日志查看是否进入到函数
    cocos2d::log("NPC [%s] clicked, isChatting: %d", npcName.c_str(), isChatting);

    if (isChatting) {
        startChat();
    }
    else {
        isChatting = true;
        startChat();  // 开始对话
    }
}

void NPC::showChatButton(bool show) {
    if (chatButton) {
        chatButton->setVisible(show);  // 根据参数控制按钮是否可见
    }
}