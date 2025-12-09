#include "MenuScene.h"
#include "FarmGround.h"

USING_NS_CC;

// 点击菜单项时的回调函数
void MenuScene::onMenuItemClicked(Ref* sender) {
    // 切换到 FarmScene 场景
    Director::getInstance()->replaceScene(FarmScene::getInstance());
}

// 创建场景
Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// 处理加载文件时的错误信息
static void problemLoading(const char* filename)
{
    printf("加载错误: %s\n", filename);
    printf("根据你的编译方式，可能需要在文件名前加上 'Resources/' 路径\n");
}

// 在初始化时进行实例化
bool MenuScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // 获取屏幕可视区域的大小和原点
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建一个“关闭”按钮用于退出程序
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(MenuScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' 和 'CloseSelected.png' 文件");
    }
    else
    {
        // 设置“关闭”按钮的位置
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // 添加背景图片
    auto background = Sprite::create("homepage.jpg");
    if (!background) {
        CCLOG("加载背景图片失败.");
        return false;
    }
    // 设置背景位置为屏幕中央
    background->setPosition(origin + visibleSize / 2);
    background->setContentSize(visibleSize);
    this->addChild(background, -1);

    // 加载原始图片纹理
    auto texture = Director::getInstance()->getTextureCache()->addImage("menu.png");
    if (!texture) {
        CCLOG("加载菜单图片失败.");
        return false;
    }
    auto title_sprite = Sprite::createWithTexture(texture);  // 创建裁剪后的精灵
   
    title_sprite->setScale(1.5f); // 放大标题精灵 1.5 倍
    title_sprite->setPosition(origin + Vec2(visibleSize.width / 2, visibleSize.height * 3 / 4)); // 设置标题位置（屏幕顶部中间）
    this->addChild(title_sprite, 0); // z-order 为 0，确保标题在背景上方

    // 创建一个菜单项按钮
    auto menuItem1 = MenuItemImage::create("load1.png", "load2.png"); // 创建按钮，指定普通状态和选中状态的图片
    menuItem1->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    // 创建鼠标事件监听器
    auto listener = EventListenerMouse::create();
    listener->onMouseMove = [=](Event* event) {
        // 获取鼠标位置
        Vec2 mousePos = Director::getInstance()->convertToGL(static_cast<EventMouse*>(event)->getLocationInView());

        // 判断鼠标是否悬停在按钮上
        Rect menuItemRect = menuItem1->getBoundingBox();
        if (menuItemRect.containsPoint(mousePos)) {
            // 鼠标悬停在按钮上，放大按钮并切换到 load2.png
            menuItem1->setScale(1.2f);  // 放大按钮
            menuItem1->setNormalImage(Sprite::create("load2.png")); // 切换到 load2.png
        }
        else {
            // 鼠标离开按钮，恢复原大小
            menuItem1->setScale(1.0f);  // 恢复原始大小
            menuItem1->setNormalImage(Sprite::create("load1.png")); // 恢复为 load1.png
        }
        };

    // 添加事件监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // 定义点击按钮时切换场景的回调函数
    auto onButtonClicked = [](Ref* sender) {
        // 点击按钮后，切换到 FarmScene 场景
        Director::getInstance()->replaceScene(FarmScene::getInstance());
        };

    // 设置按钮的点击回调函数
    menuItem1->setCallback(onButtonClicked);

    // 创建菜单并添加到场景中
    Menu* menu = Menu::create(menuItem1, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 0);


}

// 关闭按钮的回调函数
void MenuScene::menuCloseCallback(Ref* pSender)
{
    // 关闭 Cocos2d-x 游戏场景并退出应用程序
    Director::getInstance()->end();

}
