#include "Shop_board.h"
#include "Charactor/BackpackLayer.h"
#include "Global/Global.h"
//#include "UI_Scene.h"

// 创建任务栏对象，任务描述、NPC名字和完成任务的回调函数作为参数
Shop_Board* Shop_Board::create(std::function<void()> Shop_BoardCallback) {
    Shop_Board* shop_Board = new (std::nothrow) Shop_Board();
    if (shop_Board && shop_Board->init(Shop_BoardCallback)) {
        shop_Board->autorelease(); // 自动释放内存
        return shop_Board;
    }
    delete shop_Board; // 如果初始化失败，释放内存
    return nullptr;
}

// 初始化任务栏对象
// 参数：任务描述、NPC名字、完成任务的回调函数
bool Shop_Board::init(std::function<void()>Shop_BoardCallback) {
    if (!Node::init()) { // 调用父类初始化方法
        return false;
    }

    //this->taskDescription = taskDescription; // 设置任务描述
    //this->npcName = npcName; // 设置NPC名字
    this->isOpen = false; // 初始化任务未完成状态
    this->Shop_BoardCallback = Shop_BoardCallback; // 设置完成任务回调函数

    // 创建任务栏背景图片
    auto background = cocos2d::Sprite::create("shop_board.png");
    background->setScale(0.3);
    this->addChild(background); // 添加到节点树

    // 创建任务描述标签
   //descriptionLabel = cocos2d::Label::createWithSystemFont("Ten dollar each\nout-of-season items are marked up by 20%", "Arial", 24);

    descriptionLabel = cocos2d::Label::createWithTTF("Ten dollar each\nout-of-season items are marked up by 20%", "fonts/Marker Felt.ttf", 20);
    // 设置字体颜色为黑色
    descriptionLabel->setTextColor(cocos2d::Color4B::BLACK);

    descriptionLabel->setPosition(cocos2d::Vec2(background->getContentSize().width / 3 * 2, background->getContentSize().height / 3));
    descriptionLabel->setLocalZOrder(10); // 设置标签的 Z-order 高于背景
    background->addChild(descriptionLabel); // 将标签添加到背景



    //创建商品1 草莓种子
    item_strawberry = cocos2d::MenuItemImage::create(
        "strawberry.png", "strawberry.png",
        CC_CALLBACK_0(Shop_Board::buyitem_strawberry, this)); // 设置按钮回调函数
    item_strawberry->setScale(5);
    item_strawberry->setPosition(cocos2d::Vec2(background->getContentSize().width / 10,
        background->getContentSize().height / 6 * 5));

    //创建商品2 南瓜种子
    item_pumpkin = cocos2d::MenuItemImage::create(
        "pumpkin.png", "pumpkin.png",
        CC_CALLBACK_0(Shop_Board::buyitem_pumpkin, this)); // 设置按钮回调函数
    item_pumpkin->setScale(5);
    item_pumpkin->setPosition(cocos2d::Vec2(background->getContentSize().width / 4,
        background->getContentSize().height / 6 * 5));



    // 创建“关闭”按钮
    closeButton = cocos2d::MenuItemImage::create(
        "XforTaskBar.png", "XforTaskBar.png",
        CC_CALLBACK_0(Shop_Board::onClose, this)); // 设置按钮回调函数
    closeButton->setPosition(cocos2d::Vec2(background->getContentSize().width - 30,
        background->getContentSize().height - 30)); // 设置按钮位置

    // 创建菜单并添加按钮
    auto menu = cocos2d::Menu::create(item_strawberry, item_pumpkin, closeButton, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO); // 设置菜单位置
    background->addChild(menu); // 将菜单添加到背景

    return true;
}

// 买商品1——草莓种子的回调函数
void Shop_Board::buyitem_strawberry() {
    Player* player = Player::getInstance("me");
    if (isOpen == 1 && player->playerproperty.getMoney() > judgePrice()) { // 如果任务未完成
        player->playerproperty.addExperience(50);
        player->playerproperty.addReputation(50);
        player->playerproperty.addMoney((-1) * judgePrice());
        backpackLayer->removeItem("strawberry", -1);
        cocos2d::log("玩家财产已更新！");
        if (Shop_BoardCallback) {
            Shop_BoardCallback(); // 执行完成任务的回调函数

        }
        //descriptionLabel->setString("Done!"); // 更新任务描述为“任务完成！”
    }
}

// 买商品2——南瓜种子的回调函数
void Shop_Board::buyitem_pumpkin() {
    Player* player = Player::getInstance("me");
    if (isOpen == 1 && player->playerproperty.getMoney() > judgePrice()) { // 如果任务未完成
        player->playerproperty.addExperience(50);
        player->playerproperty.addReputation(50);
        player->playerproperty.addMoney((-1) * judgePrice());
        backpackLayer->removeItem("pumpkin", -1);
        cocos2d::log("玩家财产已更新！");
        if (Shop_BoardCallback) {
            Shop_BoardCallback(); // 执行完成任务的回调函数

        }
        //descriptionLabel->setString("Done!"); // 更新任务描述为“任务完成！”
    }
}




// “关闭”按钮的回调函数
void Shop_Board::onClose() {
    isOpen = 1;
    this->hide(); // 隐藏任务栏
}

// 显示任务栏
void Shop_Board::show() {
    isOpen = 1;
    this->setVisible(true);
}

// 隐藏任务栏
void Shop_Board::hide() {
    isOpen = 0;
    this->setVisible(false);
}