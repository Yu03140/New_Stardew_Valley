#include "ShedScene.h"
#include "FarmGround.h"

USING_NS_CC;

ShedScene* ShedScene::instance = nullptr;

//实现单例模式
ShedScene* ShedScene::getInstance() {
    if (instance == nullptr) {
        instance = ShedScene::create();
    }
    return instance;
}

cocos2d::Scene* ShedScene::createScene() {
    return getInstance();
}

// 初始化瓦片地图
void ShedScene::initTileMap()
{
    // 加载shed.tmx瓦片地图
    tileMap = TMXTiledMap::create("shed.tmx");

}

bool ShedScene::init()
{
    SubScene::init();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto objectGroup_animals = tileMap->getObjectGroup("animal");
    if (!objectGroup_animals) {
        CCLOG("Failed to get object group 'object'");
        return false;
    }
    animals_manager = AnimalsManager::create();
    auto objects = objectGroup_animals->getObjects();

    for (const auto& object : objects) {
        // 通过 object 中的数据判断是否是名称为 'animal' 的对象
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        //处理所有名称为草的对象
        CCLOG("%s", objectName.c_str());
        float posX = dict["x"].asFloat();
        float posY = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        //创建精灵
        auto animal = animals::create("Animals.plist");
        animal->set_info(objectName, Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y), Size(100, 100));
        animal->set_imag();
        animal->setPosition(Vec2(posX, posY));        // 设置位置
        animal->setAnchorPoint(Vec2(0, 0));     // 设置锚点
        tileMap->addChild(animal, 2);  // 添加到瓦片地图
        animal->init_mouselistener();
        animals_manager->add_animals(animal);
        animal->scheduleRandomMove(tileMap);
        animal->schedule([animal](float dt) {
            animal->update_day(dt);
            }, "update_animal");
    }
    return true;
}

void ShedScene::changeScene(Event* event)
{
    // 获取鼠标点击事件
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent)
    {
        return;
    }

    // 获取点击位置
    Vec2 clickLocation = mouseEvent->getLocationInView();
    CCLOG("clickLocation: ( %f , %f )", clickLocation.x, clickLocation.y);

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
