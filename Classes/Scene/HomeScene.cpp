#include "HomeScene.h"
#include "FarmGround.h"

USING_NS_CC;

HomeScene* HomeScene::instance = nullptr;

//实现单例模式
HomeScene* HomeScene::getInstance() {
    if (instance == nullptr) {
        instance = HomeScene::create();
    }
    return instance;
}

cocos2d::Scene* HomeScene::createScene() {
    return getInstance();
}

// 初始化瓦片地图
void HomeScene::initTileMap()
{
    // 加载Mines.tmx瓦片地图
    tileMap = TMXTiledMap::create("HomeScene.tmx");

}

void HomeScene::changeScene(Event* event)
{
    // 获取鼠标点击事件
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent)
    {
        return;
    }

    Vec2 clickLocation = mouseEvent->getLocationInView();       // 获取点击位置
    auto objectGroup = tileMap->getObjectGroup("Button");       // 获取对象层
    if (!objectGroup)
    {
        CCLOG("Button object layer not found.");
        return;
    }

    auto doorObject = objectGroup->getObject("Door");
    auto kitchenObject = objectGroup->getObject("Kitchen");

    // 场景切换按钮
    auto sprite1 = Sprite::create();
    sprite1->setPosition(Vec2(doorObject["x"].asFloat(), doorObject["y"].asFloat()));
    sprite1->setAnchorPoint(Vec2(0, 0));
    sprite1->setContentSize(Size(doorObject["width"].asFloat() * MapSize, doorObject["height"].asFloat() * MapSize));
    tileMap->addChild(sprite1);
    Vec2 pos1 = sprite1->convertToWorldSpace(Vec2(0, 0));
    CCLOG("POS: %f, %f,%f,%f", clickLocation.x, clickLocation.y, doorObject["width"].asFloat() * MapSize, doorObject["height"].asFloat() * MapSize);
    Rect doorRect = Rect(pos1.x, pos1.y, doorObject["width"].asFloat() * MapSize, doorObject["height"].asFloat() * MapSize);
    if (doorRect.containsPoint(clickLocation))
    {
        CCLOG("Door clicked! Switching scenes...");
        Director::getInstance()->popScene();
    }

    // 制作食物按钮
    auto sprite2 = Sprite::create();
    sprite2->setPosition(Vec2(kitchenObject["x"].asFloat(), kitchenObject["y"].asFloat()));
    sprite2->setAnchorPoint(Vec2(0, 0));
    sprite2->setContentSize(Size(kitchenObject["width"].asFloat() * MapSize, kitchenObject["height"].asFloat() * MapSize));
    tileMap->addChild(sprite2);
    Vec2 pos2 = sprite2->convertToWorldSpace(Vec2(0, 0));
    //CCLOG("POS: %f, %f,%f,%f", pos.x, pos.y, width, height);
    Rect kitchenRect = Rect(pos2.x, pos2.y, kitchenObject["width"].asFloat() * MapSize, kitchenObject["height"].asFloat() * MapSize);

	if (kitchenRect.containsPoint(clickLocation))
	{
        auto recipeLayer = RecipeLayer::create();
        this->addChild(recipeLayer, Taskbarlayer);
	}
}


// 每帧更新
void HomeScene::update(float delta)
{
    // 示例：可以在这里更新角色逻辑或其他动态内容
    //CCLOG("MinesScene is updating...");
}

