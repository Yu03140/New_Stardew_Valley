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
// 不再需要包含Scene文件夹下的头文件，使用工厂创建场景
#include "SceneFactory.h"

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
        
        // 更新角色移动逻辑和相机位置
        auto spriteMovePtr = sprite_move; // 创建局部变量用于lambda捕获
        sprite_move->schedule([this, spriteMovePtr](float dt) {
            spriteMovePtr->update(dt);
            this->updateCameraPosition(dt, spriteMovePtr);
        }, "update_key_person");
        
        // 更新场景逻辑
        this->schedule([this, spriteMovePtr](float dt) {
            // update函数只需要delta参数，sprite_move是成员变量，在update函数内部访问
            this->update(dt);
        }, "update_key_scene");
    }
    
    // 创建工具（需要sprite_move成功创建）
    if (sprite_move) {
        Size originalSize = sprite_move->getContentSize();
        float scale = sprite_move->getScale();
        Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);
        
        auto sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
        if (sprite_tool) {
            sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2,
                visibleSize.height / 2 + origin.y));
            this->addChild(sprite_tool, 1);
            sprite_tool->init_keyboardlistener();
            sprite_tool->init_mouselistener();
            sprite_tool->schedule([sprite_tool](float dt) {
                sprite_tool->update(dt);
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
                    sprite->init_mouselistener();
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
    
    schedule([this](float deltaTime) {
        this->update(deltaTime);
    }, "update_key");
}

//----------------------------------------------------
// FarmSceneProduct::onEnter()
// 功能：进入场景时调用
//----------------------------------------------------
void FarmSceneProduct::onEnter() {
    SceneBase::onEnter();
    is_infarm = 1;
    CCLOG("IN FARM");
    
    // 确保背包层正确显示
    auto backpacklayer = BackpackLayer::getInstance();
    if (backpacklayer) {
        if (!backpacklayer->getParent()) {
            this->addChild(backpacklayer, Backpacklayer);
            backpacklayer->setName("backpackLayer");
        }
        // 设置背包层位置（屏幕底部中央）
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        backpacklayer->setPosition(Vec2(0, 0));  // 相对于场景原点
        CCLOG("Backpack layer added and positioned");
    } else {
        CCLOG("Warning: BackpackLayer instance is null in onEnter()");
    }
}

//----------------------------------------------------
// FarmSceneProduct::onExit()
// 功能：退出场景时调用
//----------------------------------------------------
void FarmSceneProduct::onExit() {
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
    SceneBase::onMouseClick(event);
    
    auto mouse_event = dynamic_cast<EventMouse*>(event);
    if (!mouse_event) return;
    
    Vec2 mousePosition = mouse_event->getLocationInView();
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 windowOrigin = camera->getPosition() - Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 mouse_pos = mousePosition + windowOrigin;
    MOUSE_POS = mouse_pos;
    
    checkForButtonClick(mouse_pos);
    
    this->scheduleOnce([this](float dt) {
        MOUSE_POS = Vec2::ZERO;
    }, 1.5f, "reset_mouse_pos_key");
}

//----------------------------------------------------
// FarmSceneProduct::checkForButtonClick()
// 功能：检查按钮点击
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
    
    for (int i = 0; i < 3; i++) {
        auto object = objectGroup->getObject(Objectname[i]);
        if (object.empty()) {
            continue; // 对象不存在，跳过
        }
        
        float posX = object["x"].asFloat();
        float posY = object["y"].asFloat();
        float width = object["width"].asFloat() * MapSize;
        float height = object["height"].asFloat() * MapSize;
        
        auto sprite = Sprite::create();
        sprite->setPosition(Vec2(posX, posY));
        sprite->setAnchorPoint(Vec2(0, 0));
        sprite->setContentSize(Size(width, height));
        tileMap->addChild(sprite);
        Vec2 pos = sprite->convertToWorldSpace(Vec2(0, 0));
        
        if (mousePosition.x >= pos.x && mousePosition.x <= pos.x + width &&
            mousePosition.y >= pos.y && mousePosition.y <= pos.y + height) {
            if (backpackLayer) {
                backpackLayer->removeFromParent();
            }
            
            // 使用工厂创建场景
            auto factoryManager = SceneFactoryManager::getInstance();
            switch (i) {
            case 0:
                nextScene = factoryManager->createScene(SceneType::MINES_SCENE);
                break;
            case 1:
                nextScene = factoryManager->createScene(SceneType::HOME_SCENE);
                break;
            case 2:
                nextScene = factoryManager->createScene(SceneType::SHED_SCENE);
                break;
            }
            
            if (nextScene) {
                nextScene->retain();
                Director::getInstance()->pushScene(nextScene);
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
    
    // 使用SceneBase的clamp辅助函数
    float cameraX = playerPosition.x;
    if (cameraX < visibleSize.width - sceneSize.width / 2) cameraX = visibleSize.width - sceneSize.width / 2;
    if (cameraX > sceneSize.width / 2) cameraX = sceneSize.width / 2;
    
    float cameraY = playerPosition.y;
    if (cameraY < visibleSize.height - sceneSize.height / 2) cameraY = visibleSize.height - sceneSize.height / 2;
    if (cameraY > sceneSize.height / 2) cameraY = sceneSize.height / 2;
    
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

