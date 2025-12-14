// FarmSceneFactory.cpp
#include "FarmSceneFactory.h"
#include "GetableItem/Crop.h"
#include "GetableItem/Animals.h"
#include "NPC/NPC.h"
#include "GetableItem/Fish.h"
#include "Shop/Shop_board_Layer.h"
#include "Board/Board.h"
#include "TaskBar/TaskBarLayer.h"
#include "TimeSystem/TimeSystem.h"
#include "GetableItem/getable_goods.h"
#include "Player/PlayerAccount.h"
//外观模式
#include "Engine/GameEngine.h"
// 不再需要包含Scene文件夹下的头文件，使用工厂创建场景
#include "SceneFactory.h"
//【观察者模式】
//引入中介者
#include "Getableitem/InteractionManager.h"

USING_NS_CC;

#define MapSize 4
#define TOOL_WIDTH 40
#define TOOL_HEIGHT 60

//----------------------------------------------------
// FarmSceneFactory::createScene()
// 功能：创建FarmScene（使用默认参数）
//----------------------------------------------------
Scene* FarmSceneFactory::createScene() {
    SceneCreateParams params("Myfarm.tmx", 4.0f);
    return createSceneWithParams(params);
}

//----------------------------------------------------
// FarmSceneFactory::createSceneWithParams()
// 功能：使用参数创建FarmScene
//----------------------------------------------------
Scene* FarmSceneFactory::createSceneWithParams(const SceneCreateParams& params) {
    if (!validateParams(params)) {
        CCLOG("Error: Invalid parameters for FarmScene");
        return nullptr;
    }
    
    // 创建场景实例（CREATE_FUNC会自动调用init()）
    auto scene = createSceneBase();
    if (!scene) {
        CCLOG("Error: Failed to create FarmSceneProduct");
        return nullptr;
    }
    
    // 设置场景参数（init()中会使用默认值，这里设置正确的参数）
    scene->setSceneParams(params.tileMapFile, params.mapScale);
    
    // 初始化通用组件（加载地图和设置背包层）
    initSceneCommon(scene, params);
    setupCommonComponents(scene, params);
    
    // 初始化FarmScene特有组件（房屋、棚屋、玩家、UI等）
    initSceneSpecific(scene, params);
    
    return scene;
}

//----------------------------------------------------
// FarmSceneFactory::createSceneBase()
// 功能：创建FarmSceneProduct实例
//----------------------------------------------------
SceneBase* FarmSceneFactory::createSceneBase() {
    return FarmSceneProduct::create();
}

//----------------------------------------------------
// FarmSceneFactory::initSceneSpecific()
// 功能：初始化FarmScene特有组件
//----------------------------------------------------
void FarmSceneFactory::initSceneSpecific(SceneBase* scene, const SceneCreateParams& params) {
    auto farmScene = dynamic_cast<FarmSceneProduct*>(scene);
    if (!farmScene) {
        CCLOG("Error: Failed to cast to FarmSceneProduct");
        return;
    }
    
    // 检查地图是否加载成功
    if (!farmScene->getTileMap()) {
        CCLOG("Error: Tile map not loaded, cannot initialize FarmScene specific components");
        return;
    }
    
    // 初始化FarmScene特有的组件
    farmScene->initFarmSpecificComponents();
}

//----------------------------------------------------
// FarmSceneProduct::init()
// 功能：初始化FarmScene
// 注意：地图加载和背包层设置由工厂方法统一处理，这里只初始化FarmScene特有的内容
//----------------------------------------------------
bool FarmSceneProduct::init() {
    if (!SceneBase::init()) {
        return false;
    }
    
    // 设置默认地图文件（如果还没有设置）
    if (tileMapFile.empty()) {
        tileMapFile = "Myfarm.tmx";
    }
    if (mapScale <= 0.0f) {
        mapScale = 4.0f;
    }
    
    // 注意：地图加载和背包层设置由工厂方法的initSceneCommon()统一处理
    // 这里不加载地图，等待工厂方法设置参数后再加载
    
    // 注意：鼠标和键盘监听器已在SceneBase::init()中统一初始化
    // 不需要在子类中重复调用initMouseListener()和initKeyboardListener()
    // 玩家的键盘监听器在setupPlayer()中通过sprite_move->init_keyboardlistener()初始化
    
    // 调度更新
    scheduleUpdate();
    
    return true;
}

//----------------------------------------------------
// FarmSceneProduct::initFarmSpecificComponents()
// 功能：初始化FarmScene特有的组件（房屋、棚屋、玩家、UI等）
//----------------------------------------------------
void FarmSceneProduct::initFarmSpecificComponents() {
    // 检查地图是否加载成功
    if (!tileMap) {
        CCLOG("Error: Tile map not loaded, cannot initialize FarmScene specific components");
        return;
    }
    
    // 添加房屋和棚屋
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 添加房屋
    house = Sprite::create("houses.png");
    if (house) {
        house->setPosition(Vec2(600, 600));
        house->setScale(MapSize);
        this->addChild(house);
    }
    
    // 添加棚屋
    shed = Sprite::create("Big Shed.png");
    if (shed) {
        shed->setPosition(Vec2(0, 1100));
        shed->setScale(MapSize);
        this->addChild(shed);
    }
    
    // 初始化玩家
    setupPlayer();
    
    // 初始化UI
    setupUI();
}

//----------------------------------------------------
// FarmSceneProduct::setupPlayer()
// 功能：设置玩家
//----------------------------------------------------
void FarmSceneProduct::setupPlayer() {
    Player* player = Player::getInstance("me");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 创建可移动角色
    sprite_move = moveable_sprite_key_walk::create("Jas_Winter.plist", "Jas_Winter");
    if (sprite_move) {
        sprite_move->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(sprite_move, Playerlayer);
        // 注意：moveable_sprite_key_walk::create() 已经调用了 init_keyboardlistener()
        // 但是当精灵被添加到场景后，onEnter() 会被调用，会重新初始化监听器
        // 所以这里不需要再次调用
        
        // 更新角色移动逻辑
        // 注意：相机位置更新在 update() 方法中处理，不需要单独的调度器
        sprite_move->schedule([spriteMovePtr = sprite_move](float dt) {
            spriteMovePtr->update(dt);
        }, "update_key_person");
    }
    
    // 创建工具（需要sprite_move成功创建）
    if (sprite_move) {
        Size originalSize = sprite_move->getContentSize();
        float scale = sprite_move->getScale();
        Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);
        
        sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
        if (sprite_tool) {
            sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2,
                visibleSize.height / 2 + origin.y));
            this->addChild(sprite_tool, 1);
            // 【观察者模式】将场景成员变量赋给全局变量
            ::sprite_tool = sprite_tool;
            sprite_tool->init_keyboardlistener();
            // sprite_tool->init_mouselistener();
            // 创建局部变量用于 lambda 捕获，避免捕获 this 指针
            auto spriteToolPtr = sprite_tool;
            sprite_tool->schedule([spriteToolPtr](float dt) {
                spriteToolPtr->update(dt);
            }, "update_key_tool");
        } else {
            CCLOG("Warning: Failed to create sprite_tool");
        }
    } else {
        CCLOG("Error: Failed to create sprite_move, cannot create tool");
    }
    
    // 检查tileMap是否存在
    if (!tileMap) {
        CCLOG("Error: tileMap is null in setupPlayer");
        return;
    }
    
    // 初始化树木
    auto objectGroup_tree = tileMap->getObjectGroup("forest");
    if (objectGroup_tree) {
        GoodsManager* tree_manager = GoodsManager::create();
        auto objects_tree = objectGroup_tree->getObjects();
        
        for (const auto& object : objects_tree) {
            auto dict = object.asValueMap();
            std::string objectName = dict["name"].asString();
            if (objectName == "trees") {
                auto sprite = getable_goods::create("goods.plist");
                sprite->add_in(dict, sprite, "tree", tileMap);
                tree_manager->add_goods(sprite);
            }
        }
        this->addChild(tree_manager);
        tree_manager->schedule([tree_manager](float delta) {
            tree_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
    }
    
    // 初始化草
    auto objectGroup_grass = tileMap->getObjectGroup("grass");
    if (objectGroup_grass) {
        GoodsManager* grass_manager = GoodsManager::create();
        auto objects_grass = objectGroup_grass->getObjects();
        
        for (const auto& object : objects_grass) {
            auto dict = object.asValueMap();
            std::string objectName = dict["name"].asString();
            if (objectName == "grass") {
                auto sprite = getable_goods::create("goods.plist");
                sprite->add_in(dict, sprite, "grass", tileMap);
                grass_manager->add_goods(sprite);
            }
        }
        this->addChild(grass_manager);
        grass_manager->schedule([grass_manager](float delta) {
            grass_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
    }
    
    // 修改温室
    auto objectGroup_gh = tileMap->getObjectGroup("warmhouse");
    if (objectGroup_gh) {
        auto objects_gh = objectGroup_gh->getObjects();
        for (const auto& object : objects_gh) {
            auto dict = object.asValueMap();
            std::string objectName = dict["name"].asString();
            if (objectName == "warmhouse") {
                auto sprite = getable_goods::create("goods.plist");
                sprite->add_in(dict, sprite, "badGreenhouse", tileMap);
            }
        }
    }
    
    // 初始化作物
    auto objectGroup = tileMap->getObjectGroup("crops_layer");
    if (objectGroup) {
        crops.resize(36);
        for (int i = 0; i < 36; ++i) {
            crops[i].name = "";
            auto object = objectGroup->getObject("crop" + std::to_string(i + 1));
            if (!object.empty()) {
                float posX = object["x"].asFloat();
                float posY = object["y"].asFloat();
                float width = object["width"].asFloat();
                float height = object["height"].asFloat();
                
                auto sprite = crop::create("crop_m.plist", width, height);
                if (sprite) {
                    sprite->setPosition(Vec2(posX, posY));
                    sprite->setAnchorPoint(Vec2(0, 0));
                    sprite->setContentSize(Size(width, height));
                    tileMap->addChild(sprite, 2);
                    //sprite->init_mouselistener();
                    crops[i].sprite = sprite;
                    
                    sprite->schedule([sprite](float dt) {
                        sprite->update_day(dt);
                    }, "update_crop");
                }
            }
        }
    }
    
    // 初始化鱼
    auto barrierobjectGroup = tileMap->getObjectGroup("barriers");
    if (barrierobjectGroup) {
        auto object = barrierobjectGroup->getObject("barrier9");
        if (!object.empty()) {
            fish.resize(1);
            fish[0].name = "";
            float posX = object["x"].asFloat();
            float posY = object["y"].asFloat();
            float width = object["width"].asFloat();
            float height = object["height"].asFloat();
            
            auto sprite1 = fish::create("crop_m.plist", width, height);
            if (sprite1) {
                sprite1->setPosition(Vec2(posX, posY));
                sprite1->setAnchorPoint(Vec2(0, 0));
                sprite1->setContentSize(Size(width, height));
                tileMap->addChild(sprite1, 2);
                sprite1->init_mouselistener();
                fish[0].sprite = sprite1;
            }
        }
    }
}

//----------------------------------------------------
// FarmSceneProduct::setupUI()
// 功能：设置UI
//----------------------------------------------------
void FarmSceneProduct::setupUI() {
    taskBarLayer = TaskBarLayer::create();
    this->addChild(taskBarLayer, Backpacklayer);
    
    shop_board_layer = Shop_Board_Layer::create();
    this->addChild(shop_board_layer);
    
    board = Board::createBoard("normal", 0, 0);
    board->setScale(6);
    board->setPosition(0, 0);
    this->addChild(board, Backpacklayer);
    
    // 时间系统
    timeSystem = TimeSystem::getInstance();
    if (timeSystem) {
        // 如果时间系统已经有父节点，先移除它（因为TimeSystem可能是单例）
        if (timeSystem->getParent()) {
            timeSystem->removeFromParent();
        }
        Node* parentNode = this;
        parentNode->addChild(timeSystem);
    }
    //schedule([this](float deltaTime) {
    //    timeSystem->checkForHoliday();
    //}, "time_check_key");
    
    // 初始化NPC
    npc1 = NPC::create(cocos2d::Vec2(1050, 1050), "Bouncer", 50, {
        "Hello, traveler!",
        "My name is Bouncer.",
        "Could you please give me a favor?",
        "Check the taskbar please"
    });
    npc1->setScale(4);
    this->addChild(npc1);
    
    npc2 = NPC::create(cocos2d::Vec2(500, 500), "May", 80, {
        "Hello, traveler!",
        "How can I help you today?",
        "I hope you're having a good day!",
        "Bye~"
    });
    npc2->setScale(4);
    this->addChild(npc2);
    
    // 注意：不需要再次调用 schedule update，因为 init() 中已经调用了 scheduleUpdate()
    // scheduleUpdate() 会自动调用 update() 方法
}

//----------------------------------------------------
// FarmSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void FarmSceneProduct::onEnter() {
    SceneBase::onEnter();
    is_infarm = 1;
    CCLOG("IN FARM");
    
    // 关键修复1：重新初始化鼠标和键盘监听器（场景切换后需要重新注册）
    initMouseListener();
    initKeyboardListener();
    // --- 【观察者模式】启动 InteractionManage ---
    InteractionManager::getInstance()->startListening(this->getEventDispatcher());
    CCLOG("[FarmScene] InteractionManager started listening.");
    
    // 关键修复2：确保背包层正确显示并更新全局变量
    auto backpacklayer = BackpackLayer::getInstance();
    if (backpacklayer) {
        // 如果背包层已经有父节点，先移除它（可能是从其他场景带来的）
        if (backpacklayer->getParent()) {
            backpacklayer->removeFromParent();
        }
        
        // 添加到当前场景
        this->addChild(backpacklayer, Backpacklayer);
        backpacklayer->setName("backpackLayer");
        
        // 更新场景的成员变量
        backpackLayer = backpacklayer;
        
        // 关键修复3：更新全局变量backpackLayer（工具精灵使用全局变量）
        // 注意：Global.h中已经声明了extern BackpackLayer* backpackLayer;
        // Global.cpp中定义了全局变量，这里直接使用全局命名空间的变量
        ::backpackLayer = backpacklayer;  // 更新全局变量
        
        // 设置背包层位置（屏幕底部中央）
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        backpacklayer->setPosition(Vec2(0, 0));  // 相对于场景原点
        CCLOG("Backpack layer added and positioned in FarmScene, global variable updated");
    } else {
        CCLOG("Warning: BackpackLayer instance is null in onEnter()");
    }
    
    // 关键修复4：确保工具精灵能正确获取backpackLayer
    if (sprite_tool) {
        // 工具精灵使用全局变量backpackLayer，已经在上面更新了
        CCLOG("Tool sprite should now have access to backpackLayer");
    }
    
    // 注意：玩家精灵和工具精灵的监听器重新初始化移到onEnterTransitionDidFinish()中
    // 因为此时所有子节点都已经完全进入场景，确保监听器能正确注册
}

//----------------------------------------------------
// FarmSceneProduct::onEnterTransitionDidFinish()
// 功能：场景进入过渡完成时调用（此时所有子节点都已完全进入场景）
//----------------------------------------------------
void FarmSceneProduct::onEnterTransitionDidFinish() {
    SceneBase::onEnterTransitionDidFinish();
    
    // 关键修复：使用scheduleOnce延迟一帧重新初始化监听器
    // 这确保精灵的onEnter()和onExit()调用完成后再初始化
    // 因为在使用popScene()时，精灵可能会先调用onExit()，然后调用onEnter()
    this->scheduleOnce([this](float dt) {
        // 关键修复：在场景完全进入后重新初始化玩家精灵的键盘监听器
        // 这确保当使用popScene()返回农场场景时，键盘监听器能正确重新注册
        if (sprite_move) {
            if (!sprite_move->getParent()) {
                CCLOG("ERROR: sprite_move has no parent in delayed reinit!");
                return;
            }
            
            // 检查精灵是否正在运行
            if (!sprite_move->isRunning()) {
                CCLOG("WARNING: sprite_move is not running, waiting for onEnter...");
                // 如果精灵还没有运行，再延迟一点
                this->scheduleOnce([this](float dt2) {
                    if (sprite_move && sprite_move->isRunning()) {
                        sprite_move->getEventDispatcher()->removeEventListenersForTarget(sprite_move);
                        sprite_move->init_keyboardlistener();
                        CCLOG("Player sprite keyboard listener reinitialized in FarmScene (second delay)");
                    }
                }, 0.1f, "reinit_player_keyboard_second");
                return;
            }
            
            // 先移除旧的监听器，避免重复注册
            sprite_move->getEventDispatcher()->removeEventListenersForTarget(sprite_move);
            
            // 此时精灵已经完全添加到场景树中，可以直接重新初始化监听器
            sprite_move->init_keyboardlistener();
            CCLOG("Player sprite keyboard listener reinitialized in FarmScene onEnterTransitionDidFinish (delayed), sprite running: %d", sprite_move->isRunning());
        } else {
            CCLOG("ERROR: sprite_move is null in delayed reinit!");
        }
        
        //// 关键修复：重新初始化工具精灵的鼠标监听器
        //if (sprite_tool && sprite_tool->getParent() && sprite_tool->isRunning()) {
        //    // 先移除旧的监听器，避免重复注册
        //    sprite_tool->getEventDispatcher()->removeEventListenersForTarget(sprite_tool);
        //    
        //    // 重新初始化鼠标监听器
        //    sprite_tool->init_mouselistener();
        //    CCLOG("Tool sprite mouse listener reinitialized in FarmScene onEnterTransitionDidFinish (delayed)");
        //}

    }, 0.1f, "reinit_listeners_after_transition");
}

//----------------------------------------------------
// FarmSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void FarmSceneProduct::onExit() {
    // 清理所有调度器，防止在场景销毁后仍然调用回调导致悬空指针
    unschedule("reset_mouse_pos_key");
    unschedule("reinit_listeners_after_transition");  // 清理延迟初始化调度器
    
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

    // --- 【观察者模式】停止 InteractionManage ---
    InteractionManager::getInstance()->stopListening(this->getEventDispatcher());
    CCLOG("[FarmScene] InteractionManager stopped listening.");
    
    SceneBase::onExit();
    is_infarm = 0;
    CCLOG("LEAVE FARM");
}

//----------------------------------------------------
// FarmSceneProduct::update()
// 功能：每帧更新
//----------------------------------------------------
void FarmSceneProduct::update(float delta) {
    SceneBase::update(delta);
    
    // 更新相机位置（跟随玩家）
    if (sprite_move) {
        updateCameraPosition(delta, sprite_move);
    }
    
    if (sprite_move && house && shed) {
        // 更新房屋和棚屋透明度
        Vec2 housePos = house->getPosition();
        Size houseSize = house->getContentSize() * MapSize;
        Vec2 playerPos = sprite_move->getPosition();
        bool isPlayerInsideHouse = playerPos.x > housePos.x - houseSize.width / 2 &&
            playerPos.x < housePos.x + houseSize.width / 2 &&
            playerPos.y > housePos.y - houseSize.height / 2 &&
            playerPos.y < housePos.y + houseSize.height / 2;
        
        Vec2 shedPos = shed->getPosition();
        Size shedSize = shed->getContentSize() * MapSize;
        bool isPlayerInsideshed = playerPos.x > shedPos.x - shedSize.width / 2 &&
            playerPos.x < shedPos.x + shedSize.width / 2 &&
            playerPos.y > shedPos.y - shedSize.height / 2 &&
            playerPos.y < shedPos.y + shedSize.height / 2;
        
        house->setOpacity(isPlayerInsideHouse ? 128 : 255);
        shed->setOpacity(isPlayerInsideshed ? 128 : 255);
    }
    
    // 更新NPC交互
    checkNPCInteraction();
    
    if (npc1 && character_pos.distance(npc1->getPosition()) < 50) {
        npc1->showChatButton(true);
    } else if (npc1) {
        npc1->showChatButton(false);
    }
    
    if (npc2 && character_pos.distance(npc2->getPosition()) < 50) {
        npc2->showChatButton(true);
    } else if (npc2) {
        npc2->showChatButton(false);
    }
}

//----------------------------------------------------
// FarmSceneProduct::onMouseClick()
// 功能：鼠标点击事件处理
//----------------------------------------------------
void FarmSceneProduct::onMouseClick(Event* event) {
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
    
    // 关键修复1：检查点击位置是否在工具控制范围内，如果是，设置is_in_control
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
            CCLOG("Mouse click in control range, is_in_control set to 1");

            // 【观察者模式：如果点在控制范围内，立即播放工具动画】
            // 确保 ::sprite_tool 已在 Global.h/cpp 中声明和 FarmSceneProduct::setupPlayer 中赋值
            if (::sprite_tool && !::sprite_tool->get_sprite_name_tool().empty()) {
                // 假设 moveable_sprite_key_tool::get_selected_item_name() 存在
                // 或者我们直接调用 playClickAnimation()，让它自己检查是否有选中工具。
                ::sprite_tool->playClickAnimation();
                CCLOG("Tool animation played due to control range click.");
            }
            checkForButtonClick(mouse_pos);

        } else {
            is_in_control = 0;
        }

    }
    
    // 检查是否点击在背包层上（背包层在屏幕底部，大约占屏幕高度的1/4）
    bool clickedOnBackpack = false;
    if (backpackLayer) {
        Vec2 backpackPos = backpackLayer->getPosition();
        Size backpackSize = backpackLayer->getContentSize();
        // 检查点击位置是否在背包层范围内
        if (mouse_pos.x >= backpackPos.x && mouse_pos.x <= backpackPos.x + backpackSize.width &&
            mouse_pos.y >= backpackPos.y && mouse_pos.y <= backpackPos.y + backpackSize.height) {
            clickedOnBackpack = true;
        }
    }
    
    // 如果没有点击在背包层上，让人物移动到目标位置
    if (!clickedOnBackpack && sprite_move) {
        // 检查目标位置是否在地图范围内
        if (isPositionInMap(mouse_pos)) {
            sprite_move->moveToPosition(mouse_pos);
            CCLOG("Player moving to: (%f, %f)", mouse_pos.x, mouse_pos.y);
        }
    }
    
    this->scheduleOnce([this](float dt) {
        MOUSE_POS = Vec2::ZERO;
    }, 1.5f, "reset_mouse_pos_key");
}

//----------------------------------------------------
// FarmSceneProduct::checkForButtonClick()
// 功能：检查按钮点击
// 注意：参考矿洞场景的实现方式，使用视图坐标进行检测
//----------------------------------------------------
void FarmSceneProduct::checkForButtonClick(Vec2 mousePosition) {
    if (!tileMap) {
        return;
    }
    auto objectGroup = tileMap->getObjectGroup("Button");
    if (!objectGroup) {
        return;
    }
    
    std::string Objectname[3] = { "Mines_Door", "Home_Door", "Shed_Door" };
    Scene* nextScene = nullptr;
    CCLOG("[scene]mouse in control");
    for (int i = 0; i < 3; i++) {
        auto object = objectGroup->getObject(Objectname[i]);
        if (object.empty()) {
            continue; // 对象不存在，跳过
        }
        
        float posX = object["x"].asFloat();
        float posY = object["y"].asFloat();
        float width = object["width"].asFloat() * MapSize;
        float height = object["height"].asFloat() * MapSize;
        
        // 参考矿洞场景的实现方式：创建一个临时精灵来转换坐标
        auto sprite = Sprite::create();
        sprite->setPosition(Vec2(posX, posY));
        sprite->setAnchorPoint(Vec2(0, 0));
        sprite->setContentSize(Size(width, height));
        tileMap->addChild(sprite);
        Vec2 pos = sprite->convertToWorldSpace(Vec2(0, 0));
        sprite->removeFromParent(); // 立即移除临时精灵
        
        // mousePosition 已经是世界坐标（在onMouseClick中已转换）
        // pos 也是世界坐标（通过convertToWorldSpace转换）
        // 所以可以直接比较
        Rect doorRect = Rect(pos.x, pos.y, width, height);
        
        if (doorRect.containsPoint(mousePosition)) {
            CCLOG("Button %s clicked! Switching scenes...", Objectname[i].c_str());
            
            if (backpackLayer) {
                backpackLayer->removeFromParent();
            }
            
            // 使用工厂创建场景
            auto factoryManager = SceneFactoryManager::getInstance();
            switch (i) {
            case 0:
                //nextScene = factoryManager->createScene(SceneType::MINES_SCENE);
                GameEngine::getInstance()->changeScene(SceneType::MINES_SCENE);
                break;
            case 1:
                //nextScene = factoryManager->createScene(SceneType::HOME_SCENE);
                GameEngine::getInstance()->changeScene(SceneType::HOME_SCENE);
                break;
            case 2:
                //nextScene = factoryManager->createScene(SceneType::SHED_SCENE);
                GameEngine::getInstance()->changeScene(SceneType::SHED_SCENE);
                break;
            }
            
            return;
        }
    }
}

//----------------------------------------------------
// FarmSceneProduct::updateCameraPosition()
// 功能：更新相机位置
//----------------------------------------------------
void FarmSceneProduct::updateCameraPosition(float dt, Node* player) {
    auto playerPosition = player->getPosition();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 关键修复：考虑地图的实际位置和锚点
    // 地图的锚点是(0.5, 0.5)，位置在(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y)
    // 地图的左下角在：(visibleSize.width/2 + origin.x - sceneSize.width/2, visibleSize.height/2 + origin.y - sceneSize.height/2)
    // 地图的右上角在：(visibleSize.width/2 + origin.x + sceneSize.width/2, visibleSize.height/2 + origin.y + sceneSize.height/2)
    
    // 计算地图在世界坐标系中的边界
    float mapCenterX = visibleSize.width / 2 + origin.x;
    float mapCenterY = visibleSize.height / 2 + origin.y;
    float mapLeft = mapCenterX - sceneSize.width / 2;
    float mapRight = mapCenterX + sceneSize.width / 2;
    float mapBottom = mapCenterY - sceneSize.height / 2;
    float mapTop = mapCenterY + sceneSize.height / 2;
    
    // 相机中心的最小值：不能小于地图左边界 + 屏幕宽度的一半
    float minCameraX = mapLeft + visibleSize.width / 2;
    // 相机中心的最大值：不能大于地图右边界 - 屏幕宽度的一半
    float maxCameraX = mapRight - visibleSize.width / 2;
    
    // 如果地图比屏幕小，将相机固定在地图中心
    if (maxCameraX < minCameraX) {
        minCameraX = maxCameraX = mapCenterX;
    }
    
    // 限制相机X坐标在有效范围内
    float cameraX = playerPosition.x;
    if (cameraX < minCameraX) cameraX = minCameraX;
    if (cameraX > maxCameraX) cameraX = maxCameraX;
    
    // Y轴同理
    float minCameraY = mapBottom + visibleSize.height / 2;
    float maxCameraY = mapTop - visibleSize.height / 2;
    
    if (maxCameraY < minCameraY) {
        minCameraY = maxCameraY = mapCenterY;
    }
    
    float cameraY = playerPosition.y;
    if (cameraY < minCameraY) cameraY = minCameraY;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
    
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    if (camera) {
        camera->setPosition3D(Vec3(cameraX, cameraY, camera->getPosition3D().z));
        float Posx = cameraX - visibleSize.width / 2;
        float Posy = cameraY - visibleSize.height / 2;
        if (backpackLayer)
            backpackLayer->setPosition(Vec2(Posx, Posy));
        if (board)
            board->setPosition(Vec2(Posx, Posy));
        if (taskBarLayer)
            taskBarLayer->setPosition(Vec2(Posx, Posy));
    }
}

//----------------------------------------------------
// FarmSceneProduct::addItem()
// 功能：添加物品
//----------------------------------------------------
void FarmSceneProduct::addItem(const std::string& itemName) {
    for (int i = 0; i < crops.size(); ++i) {
        if (crops[i].name == "") {
            crops[i].name = itemName;
            updateItemTexture(i);
            return;
        }
    }
}

//----------------------------------------------------
// FarmSceneProduct::updateItemTexture()
// 功能：更新物品纹理
//----------------------------------------------------
void FarmSceneProduct::updateItemTexture(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= crops.size()) {
        return;
    }
    auto& slot = crops[slotIndex];
    if (!slot.sprite || slot.name.empty()) {
        return;
    }
    
    std::string texturePath = slot.name + ".png";
    auto texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
    if (texture) {
        slot.sprite->setTexture(texture);
        slot.sprite->setScale(3.0f);
    }
}

//----------------------------------------------------
// FarmSceneProduct::clearItemTexture()
// 功能：清除物品纹理
//----------------------------------------------------
void FarmSceneProduct::clearItemTexture(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= crops.size())
        return;
    auto& slot = crops[slotIndex];
    slot.sprite->setTexture(nullptr);
    slot.sprite->removeAllChildren();
}

//----------------------------------------------------
// FarmSceneProduct::checkNPCInteraction()
// 功能：检查NPC交互
//----------------------------------------------------
void FarmSceneProduct::checkNPCInteraction() {
    if (npc1 && character_pos.distance(npc1->getPosition()) < 50) {
        if (!npc1->isChattingStatus()) {
            CCLOG("Press 'Chat' to interact with NPC.");
        }
    }
    if (npc2 && character_pos.distance(npc2->getPosition()) < 50) {
        if (!npc2->isChattingStatus()) {
            CCLOG("Press 'Chat' to interact with NPC.");
        }
    }
}

//----------------------------------------------------
// FarmSceneProduct::init_mouselistener()
// 功能：初始化鼠标监听器（已废弃，使用SceneBase::initMouseListener）
//----------------------------------------------------
void FarmSceneProduct::init_mouselistener() {
    // 不再需要，使用SceneBase::initMouseListener()，它会调用onMouseClick()
    // 这个方法保留是为了兼容性，但不会被调用
}

//----------------------------------------------------
// FarmSceneProduct::on_mouse_click()
// 功能：鼠标点击回调（已废弃，使用onMouseClick）
//----------------------------------------------------
void FarmSceneProduct::on_mouse_click(Event* event) {
    onMouseClick(event);
}

//----------------------------------------------------
// FarmSceneProduct::replaceHouseImage()
// 功能：替换房屋图片
//----------------------------------------------------
void FarmSceneProduct::replaceHouseImage() {
    // 实现替换房屋图片的逻辑
}

//----------------------------------------------------
// FarmSceneProduct::checkForwarmhouseClick()
// 功能：检查温室点击
//----------------------------------------------------
bool FarmSceneProduct::checkForwarmhouseClick(Vec2 mousePosition) {
    // 实现检查温室点击的逻辑
    return false;
}

