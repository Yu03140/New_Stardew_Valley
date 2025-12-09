#include "MinesScene.h"

USING_NS_CC;

MinesScene* MinesScene::instance = nullptr;

MinesScene* MinesScene::getInstance() {
    if (instance == nullptr) {
        instance = MinesScene::create();
    }
    return instance;
}

Scene* MinesScene::createScene() {
    return getInstance();
}


// 初始化瓦片地图
void MinesScene::initTileMap()
{
    // 加载Mines.tmx瓦片地图
    tileMap = TMXTiledMap::create("Mines.tmx");

    //----------------------------------------------------
    // 功能：矿石模块
    // 说明：在地图上添加矿石格子，用于生成矿石
    // 图层：Playerlayer
    //----------------------------------------------------
    // 获取对象层（每个作物格子的位置）
    auto objectGroup_stone = tileMap->getObjectGroup("stone");
    if (!objectGroup_stone) {
        CCLOG("Failed to get object group 'stone'");
    }

    // 石头格子管理
    GoodsManager* stone_manager = GoodsManager::create();
    GoodsManager* mine_manager = GoodsManager::create();
    GoodsManager* bigstone_manager = GoodsManager::create();
    auto objects = objectGroup_stone->getObjects();

    for (const auto& object : objects) {
        // 通过 object 中的数据判断是否是名称为 'stone' 的对象
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        auto sprite = getable_goods::create("goods.plist");
        CCLOG("objectName: %s", objectName.c_str());
        //处理石头
        if (objectName == "stone") {
            sprite->add_in(dict, sprite, "stones", tileMap);
            //加入石头格子管理器
            stone_manager->add_goods(sprite);
        }
        //处理矿石
        else if (objectName == "mine") {
            sprite->add_in(dict, sprite, "mine", tileMap);
            //加入矿石格子管理器
            mine_manager->add_goods(sprite);
        }
        //处理大石头
        else if (objectName == "big_stone") {
            sprite->add_in(dict, sprite, "bigstone", tileMap);
            //加入大石头格子管理器
            bigstone_manager->add_goods(sprite);
        }
    }
    this->addChild(stone_manager);
    stone_manager->schedule([stone_manager](float delta) {
        stone_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
    this->addChild(mine_manager);
    mine_manager->schedule([mine_manager](float delta) {
        mine_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
    this->addChild(bigstone_manager);
    bigstone_manager->schedule([bigstone_manager](float delta) {
        bigstone_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");

}

void MinesScene::changeScene(Event* event)
{
    // 获取鼠标点击事件
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent)
    {
        return;
    }

    // 获取点击位置
    Vec2 clickLocation = mouseEvent->getLocationInView();

    // 获取对象层
    auto objectGroup = tileMap->getObjectGroup("Button");
    if (!objectGroup)
    {
        CCLOG("Button object layer not found.");
        return;
    }
    // 获取名为 "Door" 的对象
    auto doorObject = objectGroup->getObject("Door");
    if (doorObject.empty())
    {
        CCLOG("Door object not found.");
        return;
    }
    // 获取对象的属性和范围
    float x = doorObject["x"].asFloat();
    float y = doorObject["y"].asFloat();
    float width = doorObject["width"].asFloat() * MapSize;
    float height = doorObject["height"].asFloat() * MapSize;
    CCLOG("x:%f,y:%f", x, y);
    auto sprite = Sprite::create();
    sprite->setPosition(Vec2(x, y));
    sprite->setAnchorPoint(Vec2(0, 0));
    sprite->setContentSize(Size(width, height));
    tileMap->addChild(sprite);
    Vec2 pos = sprite->convertToWorldSpace(Vec2(0, 0));
    CCLOG("POS: %f, %f,%f,%f", pos.x, pos.y, width, height);

    Rect doorRect = Rect(pos.x, pos.y, width, height);

    // 检查点击点是否在对象范围内
    if (doorRect.containsPoint(clickLocation))
    {
        CCLOG("Door clicked! Switching scenes...");
        Director::getInstance()->popScene();
    }
}