// MenuSceneFactory.cpp
#include "MenuSceneFactory.h"
#include "SceneFactory.h"

USING_NS_CC;

//----------------------------------------------------
// MenuSceneFactory::createScene()
// 功能：创建MenuScene（使用默认参数）
//----------------------------------------------------
Scene* MenuSceneFactory::createScene() {
    auto scene = createSceneBase();
    if (!scene) {
        CCLOG("Error: Failed to create MenuSceneProduct");
        return nullptr;
    }
    
    // MenuScene不需要地图和背包，所以不调用initSceneCommon
    // 直接返回场景即可
    return scene;
}

//----------------------------------------------------
// MenuSceneFactory::createSceneWithParams()
// 功能：使用参数创建MenuScene（MenuScene不使用参数）
//----------------------------------------------------
Scene* MenuSceneFactory::createSceneWithParams(const SceneCreateParams& params) {
    // MenuScene不使用参数，直接创建
    return createScene();
}

//----------------------------------------------------
// MenuSceneFactory::createSceneBase()
// 功能：创建MenuSceneProduct实例
//----------------------------------------------------
SceneBase* MenuSceneFactory::createSceneBase() {
    return MenuSceneProduct::create();
}

//----------------------------------------------------
// MenuSceneProduct::init()
// 功能：初始化MenuScene
// 注意：MenuScene不需要地图和背包，所以跳过这些初始化
//----------------------------------------------------
bool MenuSceneProduct::init() {
    if (!SceneBase::init()) {
        return false;
    }
    
    // MenuScene不需要地图和背包层，所以跳过这些初始化
    // 基类已经初始化了鼠标和键盘监听器
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 不创建关闭按钮，用户通过窗口关闭按钮退出
    
    // 添加背景图片
    auto background = Sprite::create("homepage.jpg");
    if (background) {
        background->setPosition(origin + visibleSize / 2);
        background->setContentSize(visibleSize);
        this->addChild(background, -1);
    }
    
    // 添加标题
    auto texture = Director::getInstance()->getTextureCache()->addImage("menu.png");
    if (texture) {
        auto title_sprite = Sprite::createWithTexture(texture);
        title_sprite->setScale(1.5f);
        title_sprite->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 3 / 4));
        this->addChild(title_sprite, 0);
    }
    
    // 创建菜单按钮
    auto menuItem1 = MenuItemImage::create("load1.png", "load2.png");
    menuItem1->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    
    // 鼠标移动监听
    auto listener = EventListenerMouse::create();
    listener->onMouseMove = [=](Event* event) {
        Vec2 mousePos = Director::getInstance()->convertToGL(static_cast<EventMouse*>(event)->getLocationInView());
        Rect menuItemRect = menuItem1->getBoundingBox();
        if (menuItemRect.containsPoint(mousePos)) {
            menuItem1->setScale(1.2f);
            menuItem1->setNormalImage(Sprite::create("load2.png"));
        }
        else {
            menuItem1->setScale(1.0f);
            menuItem1->setNormalImage(Sprite::create("load1.png"));
        }
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    // 设置按钮回调
    auto onButtonClicked = [](Ref* sender) {
        auto factoryManager = SceneFactoryManager::getInstance();
        auto farmScene = factoryManager->createScene(SceneType::FARMGROUND_SCENE);
        if (farmScene) {
            Director::getInstance()->replaceScene(farmScene);
        }
    };
    
    menuItem1->setCallback(onButtonClicked);
    
    // 创建菜单
    Menu* menu = Menu::create(menuItem1, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 0);
    
    return true;
}

//----------------------------------------------------
// MenuSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void MenuSceneProduct::onEnter() {
    SceneBase::onEnter();
}

//----------------------------------------------------
// MenuSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void MenuSceneProduct::onExit() {
    SceneBase::onExit();
}

//----------------------------------------------------
// MenuSceneProduct::menuCloseCallback()
// 功能：关闭按钮回调
//----------------------------------------------------
void MenuSceneProduct::menuCloseCallback(Ref* pSender) {
    Director::getInstance()->end();
}

//----------------------------------------------------
// MenuSceneProduct::onMenuItemClicked()
// 功能：菜单项点击回调
//----------------------------------------------------
void MenuSceneProduct::onMenuItemClicked(Ref* sender) {
    auto factoryManager = SceneFactoryManager::getInstance();
    auto farmScene = factoryManager->createScene(SceneType::FARMGROUND_SCENE);
    if (farmScene) {
        Director::getInstance()->replaceScene(farmScene);
    }
}

