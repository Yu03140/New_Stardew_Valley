#include "Shop_board_Layer.h"
#include "Global/Global.h"

cocos2d::Scene* Shop_Board_Layer::createScene() {
    auto scene = cocos2d::Scene::create(); 
    auto layer = Shop_Board_Layer::create();  
    scene->addChild(layer); 
    return scene; 
}

bool Shop_Board_Layer::init() {
    if (!Layer::init()) { 
        return false; 
    }

    // 创建任务栏
    auto shop_board = Shop_Board::create([]() {
        });

    // 设置任务栏到屏幕中心
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    shop_board->setAnchorPoint(Vec2(0.5, 0.5));
    shop_board->setPosition(cocos2d::Vec2(visibleSize.width / 2 - visibleSize.width * (0.1), visibleSize.height / 2 - visibleSize.height * (0.05)));

    // 获得taskBar的高度和宽度
    auto shop_boardSize = shop_board->getContentSize();
    CCLOG("shop_boardSize.width = %f, shop_boardSize.height = %f", shop_boardSize.width, shop_boardSize.height);
    shop_board->setVisible(false); // 初始不可见
    shop_board->setScale(4.0);
    this->addChild(shop_board, 2); 

    // 创建按钮，用于显示任务栏
    auto show_shop_board_Button = cocos2d::MenuItemImage::create(
        "shop_button1.png", // 按钮的正常状态图像
        "shop_button1.png", // 按钮的选中状态图像
        [shop_board](Ref* sender) { // 按钮回调函数，点击后显示任务栏
            shop_board->show();
        });

    show_shop_board_Button->setScale(1.5);
    show_shop_board_Button->setPosition(cocos2d::Vec2(visibleSize.width * 0.72, visibleSize.height * 0.6)); // 设置按钮位置
    CCLOG("show_shop_board_Button->getPosition().x = %f", show_shop_board_Button->getPosition().x);

    // 创建菜单并将按钮添加到菜单中
    auto menu = cocos2d::Menu::create(show_shop_board_Button, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO + Vec2(visibleSize.width * (-0.01), visibleSize.width * (-0.05))); // 设置菜单位置为原点
    menu->setScale(1.5);
    this->addChild(menu, 2); // 将菜单添加到当前层中


    return true; // 初始化成功
}