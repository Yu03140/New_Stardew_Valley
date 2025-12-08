// HomeSceneFactory.cpp
#include "HomeSceneFactory.h"
#include "Charactor/RecipeLayer.h"

USING_NS_CC;

#define MapSize 4

//----------------------------------------------------
// HomeSceneFactory::createScene()
// 功能：创建HomeScene（使用默认参数）
//----------------------------------------------------
Scene* HomeSceneFactory::createScene() {
    SceneCreateParams params("HomeScene.tmx", 4.0f);
    return createSceneWithParams(params);
}

//----------------------------------------------------
// HomeSceneFactory::createSceneWithParams()
// 功能：使用参数创建HomeScene
//----------------------------------------------------
Scene* HomeSceneFactory::createSceneWithParams(const SceneCreateParams& params) {
    if (!validateParams(params)) {
        CCLOG("Error: Invalid parameters for HomeScene");
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
    
    // 初始化HomeScene特有组件（玩家、UI等）
    initSceneSpecific(scene, params);
    
    return scene;
}

//----------------------------------------------------
// HomeSceneFactory::createSceneBase()
// 功能：创建HomeSceneProduct实例
//----------------------------------------------------
SceneBase* HomeSceneFactory::createSceneBase() {
    return HomeSceneProduct::create();
}

//----------------------------------------------------
// HomeSceneFactory::initSceneSpecific()
// 功能：初始化HomeScene特有组件
//----------------------------------------------------
void HomeSceneFactory::initSceneSpecific(SceneBase* scene, const SceneCreateParams& params) {
    auto homeScene = dynamic_cast<HomeSceneProduct*>(scene);
    if (!homeScene) {
        CCLOG("Error: Failed to cast to HomeSceneProduct");
        return;
    }
    
    // 检查地图是否加载成功
    if (!homeScene->getTileMap()) {
        CCLOG("Error: Tile map not loaded, cannot initialize HomeScene specific components");
        return;
    }
    
    // 初始化HomeScene特有的组件
    homeScene->initHomeSpecificComponents();
}

//----------------------------------------------------
// HomeSceneProduct::init()
// 功能：初始化HomeScene
// 注意：地图加载和背包层设置由工厂方法统一处理，这里只初始化HomeScene特有的内容
//----------------------------------------------------
bool HomeSceneProduct::init() {
    if (!SceneBase::init()) {
        return false;
    }
    
    // 设置默认地图文件（如果还没有设置）
    if (tileMapFile.empty()) {
        tileMapFile = "HomeScene.tmx";
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
// HomeSceneProduct::initHomeSpecificComponents()
// 功能：初始化HomeScene特有的组件（玩家、UI等）
//----------------------------------------------------
void HomeSceneProduct::initHomeSpecificComponents() {
    // 检查地图是否加载成功
    if (!tileMap) {
        CCLOG("Error: Tile map not loaded, cannot initialize HomeScene specific components");
        return;
    }
    
    // 初始化玩家
    setupPlayer();
    
    // 初始化UI
    setupUI();
}

//----------------------------------------------------
// HomeSceneProduct::setupPlayer()
// 功能：设置玩家
//----------------------------------------------------
void HomeSceneProduct::setupPlayer() {
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
// HomeSceneProduct::setupUI()
// 功能：设置UI
//----------------------------------------------------
void HomeSceneProduct::setupUI() {
    // HomeScene的UI设置
}

//----------------------------------------------------
// HomeSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void HomeSceneProduct::onEnter() {
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
// HomeSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void HomeSceneProduct::onExit() {
    SceneBase::onExit();
}

//----------------------------------------------------
// HomeSceneProduct::update()
// 功能：每帧更新
//----------------------------------------------------
void HomeSceneProduct::update(float delta) {
    SceneBase::update(delta);
}

//----------------------------------------------------
// HomeSceneProduct::onMouseClick()
// 功能：鼠标点击事件处理
//----------------------------------------------------
void HomeSceneProduct::onMouseClick(Event* event) {
    SceneBase::onMouseClick(event);
    changeScene(event);
}

//----------------------------------------------------
// HomeSceneProduct::changeScene()
// 功能：切换场景
//----------------------------------------------------
void HomeSceneProduct::changeScene(Event* event) {
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
    auto kitchenObject = objectGroup->getObject("Kitchen");
    
    // 检查门点击
    if (!doorObject.empty()) {
        auto sprite1 = Sprite::create();
        sprite1->setPosition(Vec2(doorObject["x"].asFloat(), doorObject["y"].asFloat()));
        sprite1->setAnchorPoint(Vec2(0, 0));
        sprite1->setContentSize(Size(doorObject["width"].asFloat() * MapSize, doorObject["height"].asFloat() * MapSize));
        tileMap->addChild(sprite1);
        Vec2 pos1 = sprite1->convertToWorldSpace(Vec2(0, 0));
        Rect doorRect = Rect(pos1.x, pos1.y, doorObject["width"].asFloat() * MapSize, doorObject["height"].asFloat() * MapSize);
        
        if (doorRect.containsPoint(clickLocation)) {
            CCLOG("Door clicked! Switching scenes...");
            Director::getInstance()->popScene();
            return;
        }
    }
    
    // 检查厨房点击
    if (!kitchenObject.empty()) {
        auto sprite2 = Sprite::create();
        sprite2->setPosition(Vec2(kitchenObject["x"].asFloat(), kitchenObject["y"].asFloat()));
        sprite2->setAnchorPoint(Vec2(0, 0));
        sprite2->setContentSize(Size(kitchenObject["width"].asFloat() * MapSize, kitchenObject["height"].asFloat() * MapSize));
        tileMap->addChild(sprite2);
        Vec2 pos2 = sprite2->convertToWorldSpace(Vec2(0, 0));
        Rect kitchenRect = Rect(pos2.x, pos2.y, kitchenObject["width"].asFloat() * MapSize, kitchenObject["height"].asFloat() * MapSize);
        
        if (kitchenRect.containsPoint(clickLocation)) {
            auto recipeLayer = RecipeLayer::create();
            this->addChild(recipeLayer, Taskbarlayer);
        }
    }
}

