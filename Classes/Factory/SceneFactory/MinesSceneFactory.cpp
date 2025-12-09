// MinesSceneFactory.cpp
#include "MinesSceneFactory.h"
#include "GetableItem/getable_goods.h"

USING_NS_CC;

#define MapSize 4

//----------------------------------------------------
// MinesSceneFactory::createScene()
// 功能：创建MinesScene（使用默认参数）
//----------------------------------------------------
Scene* MinesSceneFactory::createScene() {
    SceneCreateParams params("Mines.tmx", 4.0f);
    return createSceneWithParams(params);
}

//----------------------------------------------------
// MinesSceneFactory::createSceneWithParams()
// 功能：使用参数创建MinesScene
//----------------------------------------------------
Scene* MinesSceneFactory::createSceneWithParams(const SceneCreateParams& params) {
    if (!validateParams(params)) {
        CCLOG("Error: Invalid parameters for MinesScene");
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
    
    // 初始化MinesScene特有组件（石头、玩家、UI等）
    initSceneSpecific(scene, params);
    
    return scene;
}

//----------------------------------------------------
// MinesSceneFactory::createSceneBase()
// 功能：创建MinesSceneProduct实例
//----------------------------------------------------
SceneBase* MinesSceneFactory::createSceneBase() {
    return MinesSceneProduct::create();
}

//----------------------------------------------------
// MinesSceneFactory::initSceneSpecific()
// 功能：初始化MinesScene特有组件
//----------------------------------------------------
void MinesSceneFactory::initSceneSpecific(SceneBase* scene, const SceneCreateParams& params) {
    auto minesScene = dynamic_cast<MinesSceneProduct*>(scene);
    if (!minesScene) {
        CCLOG("Error: Failed to cast to MinesSceneProduct");
        return;
    }
    
    // 检查地图是否加载成功
    if (!minesScene->getTileMap()) {
        CCLOG("Error: Tile map not loaded, cannot initialize MinesScene specific components");
        return;
    }
    
    // 初始化MinesScene特有的组件
    minesScene->initMinesSpecificComponents();
}

//----------------------------------------------------
// MinesSceneProduct::init()
// 功能：初始化MinesScene
// 注意：地图加载和背包层设置由工厂方法统一处理，这里只初始化MinesScene特有的内容
//----------------------------------------------------
bool MinesSceneProduct::init() {
    if (!SceneBase::init()) {
        return false;
    }
    
    // 设置默认地图文件（如果还没有设置）
    if (tileMapFile.empty()) {
        tileMapFile = "Mines.tmx";
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
// MinesSceneProduct::initMinesSpecificComponents()
// 功能：初始化MinesScene特有的组件（石头、玩家、UI等）
//----------------------------------------------------
void MinesSceneProduct::initMinesSpecificComponents() {
    // 检查地图是否加载成功
    if (!tileMap) {
        CCLOG("Error: Tile map not loaded, cannot initialize MinesScene specific components");
        return;
    }
    
    // 初始化石头
    initStones();
    
    // 初始化玩家
    setupPlayer();
    
    // 初始化UI
    setupUI();
}

//----------------------------------------------------
// MinesSceneProduct::initStones()
// 功能：初始化石头
//----------------------------------------------------
void MinesSceneProduct::initStones() {
    auto objectGroup_stone = tileMap->getObjectGroup("stone");
    if (!objectGroup_stone) {
        CCLOG("Failed to get object group 'stone'");
        return;
    }
    
    GoodsManager* stone_manager = GoodsManager::create();
    GoodsManager* mine_manager = GoodsManager::create();
    GoodsManager* bigstone_manager = GoodsManager::create();
    auto objects = objectGroup_stone->getObjects();
    
    for (const auto& object : objects) {
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        auto sprite = getable_goods::create("goods.plist");
        
        if (objectName == "stone") {
            sprite->add_in(dict, sprite, "stones", tileMap);
            stone_manager->add_goods(sprite);
        }
        else if (objectName == "mine") {
            sprite->add_in(dict, sprite, "mine", tileMap);
            mine_manager->add_goods(sprite);
        }
        else if (objectName == "big_stone") {
            sprite->add_in(dict, sprite, "bigstone", tileMap);
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

//----------------------------------------------------
// MinesSceneProduct::setupPlayer()
// 功能：设置玩家
//----------------------------------------------------
void MinesSceneProduct::setupPlayer() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 创建可移动角色
    auto sprite_move = moveable_sprite_key_walk::create("Jas_Winter.plist", "Jas_Winter");
    if (sprite_move) {
        sprite_move->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(sprite_move, Playerlayer);
        sprite_move->init_keyboardlistener();
        
        this->schedule([sprite_move](float dt) {
            sprite_move->update(dt);
        }, "update_key_person");
    }
    
    // 创建工具
    Size originalSize = sprite_move->getContentSize();
    float scale = sprite_move->getScale();
    Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);
    
    auto sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
    if (sprite_tool) {
        sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2,
            visibleSize.height / 2 + origin.y));
        this->addChild(sprite_tool, Playerlayer);
        sprite_tool->init_keyboardlistener();
        sprite_tool->init_mouselistener();
        
        this->schedule([sprite_tool](float dt) {
            sprite_tool->update(dt);
        }, "update_key_tool");
    }
}

//----------------------------------------------------
// MinesSceneProduct::setupUI()
// 功能：设置UI
//----------------------------------------------------
void MinesSceneProduct::setupUI() {
    // MinesScene的UI设置
}

//----------------------------------------------------
// MinesSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void MinesSceneProduct::onEnter() {
    SceneBase::onEnter();
    
    // 添加背包层
    auto backpacklayer = BackpackLayer::getInstance();
    if (!backpacklayer->getParent()) {
        this->addChild(backpacklayer, Backpacklayer);
        backpacklayer->setPosition(Vec2(0, 0));
        backpacklayer->setName("backpacklayer");
    }
}

//----------------------------------------------------
// MinesSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void MinesSceneProduct::onExit() {
    SceneBase::onExit();
}

//----------------------------------------------------
// MinesSceneProduct::update()
// 功能：每帧更新
//----------------------------------------------------
void MinesSceneProduct::update(float delta) {
    SceneBase::update(delta);
}

//----------------------------------------------------
// MinesSceneProduct::onMouseClick()
// 功能：鼠标点击事件处理
//----------------------------------------------------
void MinesSceneProduct::onMouseClick(Event* event) {
    SceneBase::onMouseClick(event);
    changeScene(event);
}

//----------------------------------------------------
// MinesSceneProduct::changeScene()
// 功能：切换场景
//----------------------------------------------------
void MinesSceneProduct::changeScene(Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent) {
        return;
    }
    
    Vec2 clickLocation = mouseEvent->getLocationInView();
    auto objectGroup = tileMap->getObjectGroup("Button");
    if (!objectGroup) {
        CCLOG("Button object layer not found.");
        return;
    }
    
    auto doorObject = objectGroup->getObject("Door");
    if (doorObject.empty()) {
        CCLOG("Door object not found.");
        return;
    }
    
    float x = doorObject["x"].asFloat();
    float y = doorObject["y"].asFloat();
    float width = doorObject["width"].asFloat() * MapSize;
    float height = doorObject["height"].asFloat() * MapSize;
    
    auto sprite = Sprite::create();
    sprite->setPosition(Vec2(x, y));
    sprite->setAnchorPoint(Vec2(0, 0));
    sprite->setContentSize(Size(width, height));
    tileMap->addChild(sprite);
    Vec2 pos = sprite->convertToWorldSpace(Vec2(0, 0));
    
    Rect doorRect = Rect(pos.x, pos.y, width, height);
    
    if (doorRect.containsPoint(clickLocation)) {
        CCLOG("Door clicked! Switching scenes...");
        Director::getInstance()->popScene();
    }
}

