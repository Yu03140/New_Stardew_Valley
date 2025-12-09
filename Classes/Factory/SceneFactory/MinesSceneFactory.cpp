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
// 注意：矿洞场景需要工具精灵来显示和检查工具（镐），参考农场场景的实现
//----------------------------------------------------
void MinesSceneProduct::setupPlayer() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 创建可移动角色
    sprite_move = moveable_sprite_key_walk::create("Jas_Winter.plist", "Jas_Winter");
    if (sprite_move) {
        sprite_move->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(sprite_move, Playerlayer);
        sprite_move->init_keyboardlistener();
        
        this->schedule([this](float dt) {
            if (sprite_move) {
                sprite_move->update(dt);
            }
        }, "update_key_person");
    }
    
    // 创建工具精灵（需要sprite_move成功创建）
    // 关键修复：矿洞场景也需要工具精灵来显示和检查工具（镐），就像农场场景一样
    if (sprite_move) {
        Size originalSize = sprite_move->getContentSize();
        float scale = sprite_move->getScale();
        Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);
        
        sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
        if (sprite_tool) {
            sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2,
                visibleSize.height / 2 + origin.y));
            this->addChild(sprite_tool, 1);
            sprite_tool->init_keyboardlistener();
            sprite_tool->init_mouselistener();
            // 创建局部变量用于 lambda 捕获，避免捕获 this 指针
            auto spriteToolPtr = sprite_tool;
            sprite_tool->schedule([spriteToolPtr](float dt) {
                spriteToolPtr->update(dt);
            }, "update_key_tool");
        } else {
            CCLOG("Warning: Failed to create sprite_tool in MinesScene");
        }
    } else {
        CCLOG("Error: Failed to create sprite_move, cannot create tool");
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
    
    // 关键修复1：重新初始化鼠标和键盘监听器（场景切换后需要重新注册）
    initMouseListener();
    initKeyboardListener();
    
    // 设置非农场场景标志
    is_infarm = 0;
    CCLOG("IN MINES (not farm)");
    
    // 添加背包层并更新全局变量
    auto backpacklayer = BackpackLayer::getInstance();
    backpackLayer = backpacklayer; // 更新场景成员变量
    if (backpacklayer) {
        // 如果背包层已经有父节点，先移除它（可能是从其他场景带来的）
        if (backpacklayer->getParent()) {
            backpacklayer->removeFromParent();
        }
        
        // 添加到当前场景
        this->addChild(backpacklayer, Backpacklayer);
        backpacklayer->setName("backpackLayer");
        
        // 关键修复2：更新全局变量backpackLayer（工具精灵使用全局变量）
        ::backpackLayer = backpacklayer;  // 更新全局变量
        
        backpacklayer->setPosition(Vec2(0, 0));
        CCLOG("Backpack layer added and positioned in MinesScene, global variable updated");
    } else {
        CCLOG("Warning: BackpackLayer instance is null in onEnter()");
    }
    
    // 关键修复3：确保工具精灵能正确获取backpackLayer
    if (sprite_tool) {
        // 工具精灵使用全局变量backpackLayer，已经在上面更新了
        CCLOG("Tool sprite should now have access to backpackLayer");
    }
    
    // 关键修复4：重新初始化玩家精灵的键盘监听器（场景切换后需要重新注册）
    // 关键修复：先移除旧的监听器，避免重复注册
    if (sprite_move) {
        // 先移除旧的监听器，避免重复注册
        sprite_move->getEventDispatcher()->removeEventListenersForTarget(sprite_move);
        
        // 使用scheduleOnce延迟一帧，确保精灵已经完全添加到场景树中
        this->scheduleOnce([this](float dt) {
            if (sprite_move && sprite_move->getParent()) {
                sprite_move->init_keyboardlistener();
                CCLOG("Player sprite keyboard listener reinitialized in MinesScene onEnter");
            }
        }, 0.0f, "reinit_player_keyboard");
    }
    
    // 关键修复5：重新初始化工具精灵的鼠标监听器
    if (sprite_tool) {
        // 使用scheduleOnce延迟一帧，确保精灵的onEnter()已经执行
        this->scheduleOnce([this](float dt) {
            if (sprite_tool && sprite_tool->getParent()) {
                sprite_tool->init_mouselistener();
                CCLOG("Tool sprite mouse listener reinitialized in MinesScene onEnter");
            }
        }, 0.0f, "reinit_tool_mouse");
    }
}

//----------------------------------------------------
// MinesSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void MinesSceneProduct::onExit() {
    // 清理所有调度器，防止在场景销毁后仍然调用回调导致悬空指针
    unschedule("reinit_player_keyboard");
    unschedule("reinit_tool_mouse");
    
    // 清理 sprite_move 上的调度器
    if (sprite_move) {
        sprite_move->unschedule("update_key_person");
    }
    
    // 清理 sprite_tool 上的调度器（关键修复：防止场景销毁后仍然调用 update）
    if (sprite_tool) {
        sprite_tool->unschedule("update_key_tool");
    }
    
    // 注意：scheduleUpdate() 注册的更新会在 Scene::onExit() 中自动清理
    // 不需要手动调用 unscheduleUpdate()
    
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
    auto mouse_event = dynamic_cast<EventMouse*>(event);
    if (!mouse_event) return;
    
    // 只处理左键点击（用于移动），右键点击用于交互
    if (mouse_event->getMouseButton() != EventMouse::MouseButton::BUTTON_LEFT) {
        return;
    }
    
    SceneBase::onMouseClick(event);
    
    Vec2 mousePosition = mouse_event->getLocationInView();
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 windowOrigin = camera->getPosition() - Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 mouse_pos = mousePosition + windowOrigin;
    MOUSE_POS = mouse_pos;
    
    // 关键修复：检查点击位置是否在工具控制范围内，如果是，设置is_in_control
    // 注意：工具精灵的鼠标监听器（优先级2）会在场景监听器（优先级1）之后处理
    // 所以这里先设置is_in_control，工具精灵的监听器会再次检查和设置
    // 这样确保在精灵的监听器处理之前，is_in_control已经被正确设置
    if (sprite_move) {
        Vec2 playerPos = sprite_move->getPosition();
        if (mouse_pos.x > playerPos.x - CONTROL_RANGE &&
            mouse_pos.x < playerPos.x + CONTROL_RANGE &&
            mouse_pos.y > playerPos.y - CONTROL_RANGE &&
            mouse_pos.y < playerPos.y + CONTROL_RANGE) {
            is_in_control = 1;
            CCLOG("Mouse click in control range (MinesScene), is_in_control set to 1");
        } else {
            is_in_control = 0;
        }
    }
    
    // 检查场景切换（点击门）
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

