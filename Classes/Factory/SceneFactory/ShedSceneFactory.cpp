// ShedSceneFactory.cpp
#include "ShedSceneFactory.h"
#include "GetableItem/animals.h"
#include "SceneFactory.h"

USING_NS_CC;

//----------------------------------------------------
// ShedSceneFactory::createScene()
// 功能：创建ShedScene（使用默认参数）
//----------------------------------------------------
Scene* ShedSceneFactory::createScene() {
    SceneCreateParams params("shed.tmx", 4.0f);
    return createSceneWithParams(params);
}

//----------------------------------------------------
// ShedSceneFactory::createSceneWithParams()
// 功能：使用参数创建ShedScene
//----------------------------------------------------
Scene* ShedSceneFactory::createSceneWithParams(const SceneCreateParams& params) {
    if (!validateParams(params)) {
        CCLOG("Error: Invalid parameters for ShedScene");
        return nullptr;
    }
    
    auto scene = createSceneBase();
    if (!scene) {
        return nullptr;
    }
    
    // 设置场景参数
    scene->setSceneParams(params.tileMapFile, params.mapScale);
    
    // 初始化通用组件（加载地图和设置背包层）
    initSceneCommon(scene, params);
    setupCommonComponents(scene, params);
    
    // 初始化ShedScene特有组件（动物、玩家、UI等）
    initSceneSpecific(scene, params);
    
    return scene;
}

//----------------------------------------------------
// ShedSceneFactory::createSceneBase()
// 功能：创建ShedSceneProduct实例
//----------------------------------------------------
SceneBase* ShedSceneFactory::createSceneBase() {
    return ShedSceneProduct::create();
}

//----------------------------------------------------
// ShedSceneFactory::initSceneSpecific()
// 功能：初始化ShedScene特有组件
//----------------------------------------------------
void ShedSceneFactory::initSceneSpecific(SceneBase* scene, const SceneCreateParams& params) {
    auto shedScene = dynamic_cast<ShedSceneProduct*>(scene);
    if (!shedScene) {
        CCLOG("Error: Failed to cast to ShedSceneProduct");
        return;
    }
    
    // 检查地图是否加载成功
    if (!shedScene->getTileMap()) {
        CCLOG("Error: Tile map not loaded, cannot initialize ShedScene specific components");
        return;
    }
    
    // 初始化ShedScene特有的组件（动物、玩家、UI等）
    shedScene->initShedSpecificComponents();
}

//----------------------------------------------------
// ShedSceneProduct::init()
// 功能：初始化ShedScene
// 注意：地图加载和背包层设置由工厂方法统一处理，这里只初始化ShedScene特有的内容
//----------------------------------------------------
bool ShedSceneProduct::init() {
    if (!SceneBase::init()) {
        return false;
    }
    
    // 设置默认地图文件（如果还没有设置）
    if (tileMapFile.empty()) {
        tileMapFile = "shed.tmx";
    }
    if (mapScale <= 0.0f) {
        mapScale = 4.0f;
    }
    
    // 注意：地图加载和背包层设置由工厂方法的initSceneCommon()统一处理
    // 这里不加载地图，等待工厂方法设置参数后再加载
    
    // 注意：鼠标和键盘监听器已在SceneBase::init()中统一初始化
    // 不需要在子类中重复调用initMouseListener()和initKeyboardListener()
    
    // 调度更新
    scheduleUpdate();
    
    return true;
}

//----------------------------------------------------
// ShedSceneProduct::initShedSpecificComponents()
// 功能：初始化ShedScene特有的组件（动物、玩家、UI等）
//----------------------------------------------------
void ShedSceneProduct::initShedSpecificComponents() {
    // 检查地图是否加载成功
    if (!tileMap) {
        CCLOG("Error: Tile map not loaded, cannot initialize ShedScene specific components");
        return;
    }
    
    // 初始化场景特有内容（动物）
    initSceneSpecific();
    
    // 初始化玩家
    setupPlayer();
    
    // 初始化UI
    setupUI();
    
    CCLOG("ShedScene initialized with map: %s, scale: %f",
        tileMapFile.c_str(), mapScale);
}

//----------------------------------------------------
// ShedSceneProduct::initSceneSpecific()
// 功能：初始化场景特有内容（私有方法，只初始化动物）
//----------------------------------------------------
void ShedSceneProduct::initSceneSpecific() {
    initAnimals();
}

//----------------------------------------------------
// ShedSceneProduct::initAnimals()
// 功能：初始化动物
//----------------------------------------------------
void ShedSceneProduct::initAnimals() {
    if (!tileMap) {
        CCLOG("Error: Tile map not loaded for animal initialization");
        return;
    }
    
    auto objectGroup_animals = tileMap->getObjectGroup("animal");
    if (!objectGroup_animals) {
        CCLOG("Warning: Object group 'animal' not found in tile map");
        return;
    }
    
    animals_manager = AnimalsManager::create();
    if (!animals_manager) {
        CCLOG("Error: Failed to create AnimalsManager");
        return;
    }
    
    auto objects = objectGroup_animals->getObjects();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    for (const auto& object : objects) {
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        
        if (objectName.empty()) {
            continue;
        }
        
        float posX = dict["x"].asFloat();
        float posY = dict["y"].asFloat();
        float width = dict["width"].asFloat();
        float height = dict["height"].asFloat();
        
        auto animal = animals::create("Animals.plist");
        if (!animal) {
            CCLOG("Warning: Failed to create animal: %s", objectName.c_str());
            continue;
        }
        
        Vec2 centerPos(visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y);
        animal->set_info(objectName, centerPos, Size(100, 100));
        animal->set_imag();
        animal->setPosition(Vec2(posX, posY));
        animal->setAnchorPoint(Vec2(0, 0));
        
        tileMap->addChild(animal, 2);
        animal->init_mouselistener();
        animals_manager->add_animals(animal);
        animal->scheduleRandomMove(tileMap);
        animal->schedule([animal](float dt) {
            animal->update_day(dt);
        }, "update_animal");
        
        CCLOG("Added animal: %s at (%.0f, %.0f)", objectName.c_str(), posX, posY);
    }
    
    if (animals_manager) {
        this->addChild(animals_manager);
    }
    
    CCLOG("Initialized %zd animals", objects.size());
}

//----------------------------------------------------
// ShedSceneProduct::setupPlayer()
// 功能：设置玩家
//----------------------------------------------------
void ShedSceneProduct::setupPlayer() {
    // 使用基类的通用玩家设置
    SceneBase::setupPlayer();
}

//----------------------------------------------------
// ShedSceneProduct::setupUI()
// 功能：设置UI
//----------------------------------------------------
void ShedSceneProduct::setupUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto sceneNameLabel = Label::createWithSystemFont("Barn", "Arial", 24);
    sceneNameLabel->setColor(Color3B::YELLOW);
    sceneNameLabel->setPosition(Vec2(origin.x + visibleSize.width - 100,
        origin.y + visibleSize.height - 30));
    this->addChild(sceneNameLabel, 1000);
}

//----------------------------------------------------
// ShedSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void ShedSceneProduct::onEnter() {
    SceneBase::onEnter();
    CCLOG("Entering ShedScene");
}

//----------------------------------------------------
// ShedSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void ShedSceneProduct::onExit() {
    CCLOG("Exiting ShedScene");
    SceneBase::onExit();
}

//----------------------------------------------------
// ShedSceneProduct::update()
// 功能：每帧更新
//----------------------------------------------------
void ShedSceneProduct::update(float delta) {
    SceneBase::update(delta);
}

//----------------------------------------------------
// ShedSceneProduct::onMouseClick()
// 功能：鼠标点击事件处理
//----------------------------------------------------
void ShedSceneProduct::onMouseClick(Event* event) {
    SceneBase::onMouseClick(event);
    
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent) {
        return;
    }
    
    Vec2 clickLocation = mouseEvent->getLocationInView();
    
    if (checkDoorClick(clickLocation)) {
        CCLOG("Door clicked! Switching scenes...");
        
        // 切换到农场场景
        auto factoryManager = SceneFactoryManager::getInstance();
        auto farmScene = factoryManager->createScene(SceneType::FARMGROUND_SCENE);
        if (farmScene) {
            Director::getInstance()->replaceScene(farmScene);
        }
    }
}

//----------------------------------------------------
// ShedSceneProduct::checkDoorClick()
// 功能：检查门点击
//----------------------------------------------------
bool ShedSceneProduct::checkDoorClick(const Vec2& clickLocation) {
    if (!tileMap) {
        return false;
    }
    
    auto objectGroup = tileMap->getObjectGroup("Button");
    if (!objectGroup) {
        CCLOG("Button object layer not found.");
        return false;
    }
    
    auto doorObject = objectGroup->getObject("Door");
    if (doorObject.empty()) {
        CCLOG("Door object not found.");
        return false;
    }
    
    float x = doorObject["x"].asFloat();
    float y = doorObject["y"].asFloat();
    float width = doorObject["width"].asFloat() * mapScale;
    float height = doorObject["height"].asFloat() * mapScale;
    
    auto sprite = Sprite::create();
    sprite->setPosition(Vec2(x, y));
    sprite->setAnchorPoint(Vec2(0, 0));
    sprite->setContentSize(Size(width, height));
    
    Vec2 worldPos = tileMap->convertToWorldSpace(Vec2(x, y));
    Rect doorRect = Rect(worldPos.x, worldPos.y, width, height);
    
    if (doorRect.containsPoint(clickLocation)) {
        return true;
    }
    
    return false;
}

//----------------------------------------------------
// ShedSceneProduct::createDoorDebugSprite()
// 功能：创建门调试精灵
//----------------------------------------------------
void ShedSceneProduct::createDoorDebugSprite(float x, float y, float width, float height) {
    auto debugSprite = DrawNode::create();
    Vec2 worldPos = tileMap->convertToWorldSpace(Vec2(x, y));
    
    Vec2 vertices[] = {
        Vec2(0, 0),
        Vec2(width, 0),
        Vec2(width, height),
        Vec2(0, height)
    };
    
    debugSprite->drawPolygon(vertices, 4, Color4F(1, 0, 0, 0.3f), 1, Color4F(1, 0, 0, 1));
    debugSprite->setPosition(worldPos);
    this->addChild(debugSprite, 999);
    
    CCLOG("Door debug sprite created at world position: (%.0f, %.0f), size: (%.0f, %.0f)",
        worldPos.x, worldPos.y, width, height);
}

