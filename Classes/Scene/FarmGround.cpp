#include "FarmGround.h"
USING_NS_CC;
FarmScene* FarmScene::instance = nullptr;

//----------------------------------------------------
// FarmScene::init()
// 功能：初始化场景
// 说明：主场景，包含了农场的所有功能
//----------------------------------------------------
bool FarmScene::init() {

    if (!Scene::init()) {
        return false;
    }
    init_mouselistener();
    //----------------------------------------------------
    // 功能：加载瓦片地图
    // 说明：放于屏幕中央
    //----------------------------------------------------
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    tileMap = TMXTiledMap::create("Myfarm.tmx");
    if (tileMap) {
        tileMap->setAnchorPoint(Vec2(0.5, 0.5));
        float Map_posX = visibleSize.width / 2;
        float Map_posY = visibleSize.height / 2;
        tileMap->setPosition(Vec2(Map_posX, Map_posY));
        this->addChild(tileMap, 0);
        tileMap->setScale(MapSize);
        SceneWidth = tileMap->getContentSize().width * MapSize;
        SceneHeight = tileMap->getContentSize().height * MapSize;

        CCLOG("Loaded the tile map successfully: (%f,%f)", SceneWidth, SceneHeight);
    }
    else {
        CCLOG("Failed to load the tile map");
    }

    //----------------------------------------------------
    // 功能：添加建筑物精灵
    // 说明：便于后续设置透明度
    //----------------------------------------------------
    // 添加房子
    house = Sprite::create("houses.png"); 
    house->setPosition(Vec2(600, 600)); 
    house->setScale(MapSize);
    this->addChild(house);
    // 添加棚屋
    shed = Sprite::create("Big Shed.png"); 
    shed->setPosition(Vec2(0, 1100));
    shed->setScale(MapSize);
    this->addChild(shed);

    //----------------------------------------------------
    // 功能：人物初始化
    // 说明：创建人物对象，初始化人物属性
    //----------------------------------------------------
    Player* player = Player::getInstance("me");

    //----------------------------------------------------
    // 功能：添加背包图层
    // 说明：添加背包图层到当前场景，初始化背包
    //----------------------------------------------------
    backpackLayer = BackpackLayer::getInstance();
    if (backpackLayer) {
        this->addChild(backpackLayer, Backpacklayer);
        backpackLayer->setName("backpackLayer");
    }
    else
        CCLOG("Failed to load the backpack layer");

    //----------------------------------------------------
    // 功能：添加移动主角
    // 说明：添加主角，主角位于地图中央
    //----------------------------------------------------
    auto sprite_move = moveable_sprite_key_walk::create("Jas_Winter.plist", "Jas_Winter");
    if (sprite_move)
    {
        sprite_move->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(sprite_move, Playerlayer);
        sprite_move->init_keyboardlistener();
        // 更新人物移动逻辑和相机位置
        sprite_move->schedule([this, sprite_move](float dt) {
            sprite_move->update(dt);                         
            updateCameraPosition(dt, sprite_move);        
            }, "update_key_person");

        schedule([this, sprite_move](float dt) {
            this->update(dt, sprite_move);  
            }, "update_key_scene");
        CCLOG("diao yong cheng gong la");

    }
    Size originalSize = sprite_move->getContentSize();
    float scale = sprite_move->getScale();
    Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);

    //----------------------------------------------------
    // 功能：添加工具
    // 说明：添加随人物移动的工具
    //----------------------------------------------------
    auto sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
    if (sprite_tool)
    {
        sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2, visibleSize.height / 2 + origin.y));
        this->addChild(sprite_tool, 1);
        sprite_tool->init_keyboardlistener();
        sprite_tool->init_mouselistener();
        sprite_tool->schedule([sprite_tool](float dt) {
            sprite_tool->update(dt);
            }, "update_key_tool");
    }

    //----------------------------------------------------
    // 功能：树模块
    // 说明：在地图上添加树格子，用于生成树
    // 图层：Playerlayer
    //----------------------------------------------------
    // 获取对象层（每个作物格子的位置）
    auto objectGroup_tree = tileMap->getObjectGroup("forest");
    if (!objectGroup_tree) {
        CCLOG("Failed to get object group 'tree'");
        return false;
    }

    // 树格子管理
    GoodsManager* tree_manager = GoodsManager::create();
    auto objects_tree = objectGroup_tree->getObjects();

    for (const auto& object : objects_tree) {
        // 通过 object 中的数据判断是否是名称为 'tree' 的对象
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        //处理所有名称为树的对象
        if (objectName == "trees") {
            auto sprite = getable_goods::create("goods.plist");
            sprite->add_in(dict, sprite, "tree", tileMap);
            //加入树格子管理器
            tree_manager->add_goods(sprite);
        }
    }
    this->addChild(tree_manager);
    tree_manager->schedule([tree_manager](float delta) {
        tree_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");

    //----------------------------------------------------
    // 功能：草模块
    // 说明：在地图上添加草格子，用于生成草
    // 图层：Playerlayer
    //----------------------------------------------------
    // 获取对象层（每个作物格子的位置）
    auto objectGroup_grass = tileMap->getObjectGroup("grass");
    if (!objectGroup_grass) {
        CCLOG("Failed to get object group 'grass'");
        return false;
    }

    // 草格子管理
    GoodsManager* grass_manager = GoodsManager::create();
    auto objects_grass = objectGroup_grass->getObjects();

    for (const auto& object : objects_grass) {
        // 通过 object 中的数据判断是否是名称为 'grass' 的对象
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        //处理所有名称为草的对象
        if (objectName == "grass") {
            auto sprite = getable_goods::create("goods.plist");
            sprite->add_in(dict, sprite, "grass", tileMap);
            //加入草格子管理器
            grass_manager->add_goods(sprite);
        }
    }
    this->addChild(grass_manager);
    grass_manager->schedule([grass_manager](float delta) {
        grass_manager->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");


	//----------------------------------------------------
	// 功能：修复温室
	// 说明：在地图上修复破败的建筑温室
    //----------------------------------------------------
    auto objectGroup_gh = tileMap->getObjectGroup("warmhouse");
    if (!objectGroup_gh) {
        CCLOG("Failed to get object group 'warmhouse'");
        return false;
    }
    auto objects_gh = objectGroup_gh->getObjects();
    for (const auto& object : objects_gh) {
        // 通过 object 中的数据判断是否是名称为 'warmhouse' 的对象
        auto dict = object.asValueMap();
        std::string objectName = dict["name"].asString();
        if (objectName == "warmhouse") {
            auto sprite = getable_goods::create("goods.plist");
            sprite->add_in(dict, sprite, "badGreenhouse", tileMap);
        }
    }

    //----------------------------------------------------
    // 功能：作物模块
    // 说明：通过获取对象层，在地图上添加作物格子，用于种植作物
    //----------------------------------------------------
    auto objectGroup = tileMap->getObjectGroup("crops_layer"); 
    if (!objectGroup) {
        CCLOG("Failed to get object group 'crops_layer'");
        return false;
    }
    Vec2 map_pos = tileMap->getPosition();
    // 作物格子管理（共36格）
    crops.resize(36);
    for (int i = 0; i < 36; ++i) {
        // 作物名 
        crops[i].name = "";     
        // 获取第 i+1 个格子
        auto object = objectGroup->getObject("crop" + std::to_string(i + 1)); 
        // 获取对象层中每个格子的坐标
        float posX = object["x"].asFloat(); 
        float posY = object["y"].asFloat();
        float width = object["width"].asFloat();
        float height = object["height"].asFloat();
        // 创建透明纹理的精灵
        auto sprite = crop::create("crop_m.plist", width, height);  
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

    //----------------------------------------------------
    // 功能：任务栏和商店模块
    // 说明：任务栏...
    //----------------------------------------------------
    taskBarLayer = TaskBarLayer::create();
    this->addChild(taskBarLayer, Backpacklayer);
	shop_board_layer = Shop_Board_Layer::create();
	this->addChild(shop_board_layer);

    board = Board::createBoard("normal", 0, 0);
    board->setScale(6);
    board->setPosition(0, 0);
    this->addChild(board, Backpacklayer);
    

	//----------------------------------------------------
	// 功能：时间系统
	// 说明：添加时间系统到当前场景
	//----------------------------------------------------
    timeSystem = TimeSystem::getInstance();
    Node* parentNode = this;
    parentNode->addChild(timeSystem);
    // 每帧更新时检查时间
    schedule([this](float deltaTime) {
        timeSystem->checkForHoliday();
        }, "time_check_key");

    //----------------------------------------------------
    // 功能：初始化NPC
    // 说明：初始化NPC并添加到场景中
    //----------------------------------------------------
    // NPC1的初始化
    npc1 = NPC::create(cocos2d::Vec2(1050, 1050), "Bouncer", 50, {
        "Hello, traveler!",
        "My name is Bouncer.",
        "Could you please give me a favor?",
        "Check the taskbar please"
        });
    npc1->setScale(4);
    this->addChild(npc1);
    // NPC2的初始化
    npc2 = NPC::create(cocos2d::Vec2(500, 500), "May", 80, {
    "Hello, traveler!",
    "How can I help you today?",
    "I hope you're having a good day!",
    "Bye~"
        });
    npc2->setScale(4); 
    this->addChild(npc2);
    //设置定时更新函数
    this->schedule([this](float deltaTime) {
    //每帧调用更新函数
        this->update(deltaTime);
        }, "update_key");

    //----------------------------------------------------
    // 功能：钓鱼模块
    // 说明：通过获取对象层，判断可钓鱼的位置
    //----------------------------------------------------
    //获取对象层
    auto barrierobjectGroup = tileMap->getObjectGroup("barriers");
    if (!barrierobjectGroup) {
        CCLOG("Failed to get object group 'barriers'");
        return false;
    }
    //获取对象层中水池的坐标
    fish.resize(1);
    fish[0].name = ""; 
    auto object = barrierobjectGroup->getObject("barrier9");
    float posX = object["x"].asFloat();
    float posY = object["y"].asFloat();
    float width = object["width"].asFloat();
    float height = object["height"].asFloat();
    //创建透明纹理的精灵
    auto sprite1 = fish::create("crop_m.plist", width, height); 
    //设置位置
    sprite1->setPosition(Vec2(posX, posY));       
    sprite1->setAnchorPoint(Vec2(0, 0)); 
    sprite1->setContentSize(Size(width, height));
    tileMap->addChild(sprite1, 2); 
    sprite1->init_mouselistener();
    fish[0].sprite = sprite1;

    return true;
}

//----------------------------------------------------
// FarmScene::update()
// 功能：检测人物移动进而修改透明度
// 说明：根据人物是否在房屋区域内，调整房子的透明度
//----------------------------------------------------
void FarmScene::update(float dt, moveable_sprite_key_walk* sprite_move) {

    // 获取房子中心位置
    Vec2 housePos = house->getPosition();  
    // 房子的尺寸
    Size houseSize = house->getContentSize() * MapSize;  
    // 获取人物的中心位置
    Vec2 playerPos = sprite_move->getPosition();
    // 判断人物是否在房屋的矩形区域内
    bool isPlayerInsideHouse = playerPos.x > housePos.x - houseSize.width / 2 &&
        playerPos.x < housePos.x + houseSize.width / 2 &&
        playerPos.y > housePos.y - houseSize.height / 2 &&
        playerPos.y < housePos.y + houseSize.height / 2;
    // 获取房子中心位置
    Vec2 shedPos = shed->getPosition(); 
    // 房子的尺寸
    Size shedSize = shed->getContentSize() * MapSize;  
    // 判断人物是否在棚屋的矩形区域内
    bool isPlayerInsideshed = playerPos.x > shedPos.x - shedSize.width / 2 &&
        playerPos.x < shedPos.x + shedSize.width / 2 &&
        playerPos.y > shedPos.y - shedSize.height / 2 &&
        playerPos.y < shedPos.y + shedSize.height / 2;
    //透明度修改
    house->setOpacity(isPlayerInsideHouse ? 128 : 255);
    shed->setOpacity(isPlayerInsideshed ? 128 : 255);
}

//----------------------------------------------------
// FarmScene::getInstance()
// 功能：获取全局唯一单例
//----------------------------------------------------
FarmScene* FarmScene::getInstance() {
    if (instance == nullptr) {
        instance = FarmScene::create();
    }
    return instance;
}

//----------------------------------------------------
// FarmScene::createScene()
// 功能：创建并返回一个 FarmScene 场景的实例
//----------------------------------------------------
Scene* FarmScene::createScene() {
    return getInstance();
}

//----------------------------------------------------
// FarmScene::addItem()
// 功能：在背包中添加物品
//----------------------------------------------------
void FarmScene::addItem(const std::string& itemName) {
    for (int i = 0; i < crops.size(); ++i) {
        if (crops[i].name == "") {
            // 找到空格子，放入物品
            crops[i].name = itemName;
            updateItemTexture(i);
            return;
        }
    }
}

//----------------------------------------------------
// FarmScene::updateItemTexture()
// 功能：更新物品显示纹理
//----------------------------------------------------
void FarmScene::updateItemTexture(int slotIndex) {
    // 检查槽位索引范围是否合法
    if (slotIndex < 0 || slotIndex >= crops.size()) {
        CCLOG("Invalid slot index: %d", slotIndex);
        return;
    }
    auto& slot = crops[slotIndex];
    // 检查槽位的精灵是否为空
    if (!slot.sprite) {
        CCLOG("Slot sprite is null for slot index: %d", slotIndex);
        return;
    }
    // 如果槽位没有物品名称，直接返回
    if (slot.name.empty()) {
        CCLOG("Slot %d is empty.", slotIndex);
        return;
    }
    // 拼接图片路径
    std::string texturePath = slot.name + ".png"; 
    // 从纹理缓存加载纹理
    auto texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
    if (texture) {
        // 更新精灵的纹理
        slot.sprite->setTexture(texture);
        slot.sprite->setScale(3.0f);
        CCLOG("Updated texture for slot %d: %s", slotIndex, texturePath.c_str());
    }
    else {
        CCLOG("Failed to load texture: %s", texturePath.c_str());
    }
}

//----------------------------------------------------
// FarmScene::updateItemTexture()
// 功能：清除物品显示纹理
//----------------------------------------------------
void FarmScene::clearItemTexture(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= crops.size())
        return;
    auto& slot = crops[slotIndex];
    // 清空纹理
    slot.sprite->setTexture(nullptr);  
    // 清空子节点
    slot.sprite->removeAllChildren();  
}

//限制值的范围，使其保持在指定的最小值和最大值之间
template <typename T>
T clamp(T value, T low, T high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

//----------------------------------------------------
// FarmScene::updateCameraPosition()
// 功能：更新摄像机跟踪位置
//----------------------------------------------------
void FarmScene::updateCameraPosition(float dt, Node* player)
{
    auto playerPosition = player->getPosition();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 镜头位置要保持在地图边界内
    float cameraX = clamp(playerPosition.x, visibleSize.width - SceneWidth / 2, SceneWidth / 2);
    float cameraY = clamp(playerPosition.y, visibleSize.height - SceneHeight / 2, SceneHeight / 2);
    // 获取默认摄像头
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    // 设置摄像头位置
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
// FarmScene::init_mouselistener()
// 功能：初始化鼠标监听器
//----------------------------------------------------
void FarmScene::init_mouselistener()
{
    // 创建鼠标监听器
    auto listener = cocos2d::EventListenerMouse::create();

    // 鼠标回调
    listener->onMouseDown = CC_CALLBACK_1(FarmScene::on_mouse_click, this);

    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

//----------------------------------------------------
// FarmScene::on_mouse_click()
// 功能：鼠标按下时的回调
//----------------------------------------------------
void FarmScene::on_mouse_click(cocos2d::Event* event)
{
    //获取鼠标在窗口中的位置,转换到地图坐标
    auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
    Vec2 mousePosition = mouse_event->getLocationInView();
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 windowOrigin = camera->getPosition() - Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 mouse_pos = mousePosition + windowOrigin;
    MOUSE_POS = mouse_pos;
    CCLOG("Mouse Position(global): (%f, %f)", MOUSE_POS.x, MOUSE_POS.y);
    checkForButtonClick(mouse_pos);
    // 0.1秒后将 MOUSE_POS 置为 (0, 0)，并且不影响其他程序运行
    this->scheduleOnce([this](float dt) {
        MOUSE_POS = Vec2::ZERO;
        CCLOG("Mouse Position reset to: (%f, %f)", MOUSE_POS.x, MOUSE_POS.y);
        }, 1.5f, "reset_mouse_pos_key");
}

//----------------------------------------------------
// FarmScene::checkForButtonClick()
// 功能：检测是否点击了Button并切换场景
//----------------------------------------------------
void FarmScene::checkForButtonClick(Vec2 mousePosition)
{
    // 获取 Button 对象层
    auto objectGroup = tileMap->getObjectGroup("Button");
    if (!objectGroup) {
        CCLOG("Failed to get object group 'Button'");
        return ;
    }
    // 获取 Door 对象的坐标和尺寸
    std::string Objectname[3] = { "Mines_Door","Home_Door","Shed_Door" };
    Scene* nextScene = nullptr;
    for(int i=0;i<3;i++){
    auto object = objectGroup->getObject(Objectname[i]);
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
    CCLOG("POS: %f, %f", pos.x, pos.y);
    // 判断点击位置是否在 Door 区域内
    if (mousePosition.x >= pos.x && mousePosition.x <= pos.x + width &&
        mousePosition.y >= pos.y && mousePosition.y <= pos.y + height) {
        if (backpackLayer) {
            // 从当前场景中移除背包层，但不销毁它的内存
            backpackLayer->removeFromParent();
            CCLOG("remove backpackLayer successfully!");
        }
        CCLOG("Door clicked! Switching to MinesScene...");
        switch (i) {
        case 0:
            // 切换到 MinesScene
            nextScene = MinesScene::createScene();
            break;
        case 1:
            // 切换到 HomeScene
            nextScene = HomeScene::createScene();
            break;
        case 2:
			// 切换到 ShedScene
			nextScene = ShedScene::createScene();
			break;
        }
        // 如果我们成功获取到下一个场景，就推入栈中
        if (nextScene) {
            // 保留场景，避免被销毁
            nextScene->retain();  
            Director::getInstance()->pushScene(nextScene);
        }
        return ;
        }
    }
}

//----------------------------------------------------
// FarmScene::update()
// 功能：更新玩家与NPC交互的状态
//----------------------------------------------------
void FarmScene::update(float delta) {
	// 检查玩家与NPC的交互
    checkNPCInteraction();
     // 检查玩家与NPC的距离，并更新按钮的显示状态
    if (character_pos.distance(npc1->getPosition()) < 50) {
        // 在范围内时显示按钮
        npc1->showChatButton(true); 
    }
    else {
        // 不在范围内时隐藏按钮
        npc1->showChatButton(false); 
    }
    if (character_pos.distance(npc2->getPosition()) < 50) {
        // 在范围内时显示按钮
        npc2->showChatButton(true);  
    }
    else {
        // 不在范围内时隐藏按钮
        npc2->showChatButton(false); 
    }
}

//----------------------------------------------------
// FarmScene::checkNPCInteraction()
// 功能：检查玩家和NPC之间的距离是否小于100单位
// 说明：100是触发交互的范围阈值，即玩家与NPC的距离小于100时，才有可能与NPC互动
//----------------------------------------------------
void FarmScene::checkNPCInteraction() {
    if (character_pos.distance(npc1->getPosition()) < 50) {
        // 如果NPC当前不在聊天状态（即NPC没有正在与玩家对话），则显示互动提示。
        if (!npc1->isChattingStatus()) {
            // 输出日志，提示玩家按下“Chat”键与NPC互动。
            CCLOG("Press 'Chat' to interact with NPC.");
        }
        if (!npc2->isChattingStatus()) {
            // 输出日志，提示玩家按下“Chat”键与NPC互动。
            CCLOG("Press 'Chat' to interact with NPC.");
        }
    }
}

//----------------------------------------------------
// FarmScene::onEnter()
// 功能：当场景进入时调用的初始化函数
//       主要用于确保背包层存在并添加到当前场景中
//----------------------------------------------------
void FarmScene::onEnter() {
    Scene::onEnter();
    is_infarm = 1;
    CCLOG("IN FARM");
    // 如果背包层不存在于当前场景，重新添加
    if (backpackLayer && !this->getChildByName("backpackLayer")) {
        this->addChild(backpackLayer, Backpacklayer);
        backpackLayer->setName("backpackLayer");
        CCLOG("readd backpacklayer");
    }
}

void FarmScene::onExit()
{
    Scene::onExit();
    is_infarm = 0;
    CCLOG("LEAVE FARM");

}

