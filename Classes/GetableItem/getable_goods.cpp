#include "getable_goods.h"
#include "InteractionManager.h"

cocos2d::Texture2D* getable_goods::transparent_texture = nullptr;

void getable_goods::set_info(std::string name, Size size)
{
    sprite_name = name;
    sprite_size = size;
}

getable_goods* getable_goods::create(const std::string& plist_name)
{
    // 加载 plist 文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    // 实例化对象
    getable_goods* sprite = new getable_goods();

    // 创建透明纹理数据，设为全透明 (RGBA8888 格式)
    int dataSize = DEFAULT_WIDTH * DEFAULT_HEIGHT * 4;  // 每像素 4 字节，RGBA 格式
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个值设为 0)
    memset(transparentData, 0, dataSize);

    // 创建纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, DEFAULT_WIDTH, DEFAULT_HEIGHT, cocos2d::Size(DEFAULT_HEIGHT, DEFAULT_HEIGHT));
    transparent_texture = transparentTexture;

    // 释放数据内存
    delete[] transparentData;

    // 判断是否创建成功
    if (transparentTexture)
    {
        sprite->initWithTexture(transparentTexture);
        sprite->autorelease();
        //sprite->init_mouselistener();
        CCLOG("Creation goods successfully!");
        return sprite;
    }
    CCLOG("Creation goods unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void getable_goods::setImag()
{
    CCLOG("getable_goods::setImag");
    // 获取指定帧并设置
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name + ".png");
    this->initWithSpriteFrame(frame);
    is_getable = 1;
}

//// 初始化鼠标监听器
//void getable_goods::init_mouselistener()
//{
//    // 创建鼠标事件监听器
//    auto listener = cocos2d::EventListenerMouse::create();
//
//    // 设置鼠标按下回调
//    listener->onMouseDown = CC_CALLBACK_1(getable_goods::on_mouse_click, this);
//
//    // 注册事件监听器
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//}
//
//// 鼠标按下时的回调
//void getable_goods::on_mouse_click(cocos2d::Event* event)
//{
//    // 获取物品位置
//    Vec2 goods_pos = this->convertToWorldSpace(Vec2(0, 0));
//    Vec2 mouse_pos;
//    float min_x;
//    float max_x;
//    float min_y;
//    float max_y;
//
//    if (is_infarm)
//    {
//        mouse_pos = MOUSE_POS;
//        min_x = goods_pos.x;
//        max_x = goods_pos.x + sprite_size.width;
//        min_y = goods_pos.y;
//        max_y = goods_pos.y + sprite_size.height;
//    }
//    else {
//        auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
//        mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
//        goods_pos = this->getPosition();
//        min_x = goods_pos.x;
//        max_x = goods_pos.x + sprite_size.width / MapSize;
//        min_y = goods_pos.y;
//        max_y = goods_pos.y + sprite_size.height / MapSize;
//    }
//    // 关键修复：所有场景都需要检查is_in_control（玩家必须在控制范围内）
//    // 但是非农场场景的is_in_control检查可以放宽（如果不在控制范围内，也可以点击，但需要工具）
//    bool canCheckClick = false;
//    if (!is_infarm) {
//        // 非农场场景：如果is_in_control为1，直接可以点击；如果为0，需要检查工具
//        // 这里先允许点击，然后在工具检查中判断
//        canCheckClick = true;
//    }
//    else {
//        // 农场场景：必须is_in_control为1才能点击
//        canCheckClick = is_in_control;
//    }
//
//    if (is_getable && canCheckClick) {
//        // 判断点击是否在范围内
//        if ((mouse_pos.x > min_x &&
//            mouse_pos.x < max_x &&
//            mouse_pos.y > min_y &&
//            mouse_pos.y < max_y))
//        {
//            CCLOG("good click");
//            CCLOG("%s", sprite_name.c_str());
//
//            // 关键修复：所有场景都需要检查工具匹配（除了明确不需要工具的物品）
//            // 安全获取BackpackLayer实例
//            auto* safeBackpackLayer = BackpackLayer::getInstance();
//            if (!safeBackpackLayer) {
//                CCLOG("BackpackLayer is null, cannot check tool");
//                return;
//            }
//
//            std::string requiredTool = GOODS_MAP.at(sprite_name).at("tool");
//            bool canInteract = false;
//
//            // 如果不需要工具（空字符串），直接允许交互
//            if (requiredTool == "") {
//                canInteract = true;
//                click_count += 1;
//            }
//            else {
//                // 需要工具：检查选中的工具是否匹配
//                std::string selectedItem = safeBackpackLayer->getSelectedItem();
//
//                // 精确匹配：检查选中的工具是否以所需工具名称开头
//                // 例如：requiredTool = "Hoe", selectedItem = "Hoe1" 或 "Hoe2" 都匹配
//                // 使用 find() == 0 确保是前缀匹配，而不是包含匹配
//                if (!selectedItem.empty() && selectedItem.find(requiredTool) == 0) {
//                    canInteract = true;
//                    // 根据工具等级计算点击次数
//                    char last_char = selectedItem[selectedItem.size() - 1];
//                    int level = last_char - '0';
//                    if (level >= 1 && level <= 9) {
//                        click_count += level;
//                    }
//                    else {
//                        click_count += 1; // 默认等级为1
//                    }
//                    CCLOG("Tool matched: required=%s, selected=%s, click_count=%d", requiredTool.c_str(), selectedItem.c_str(), click_count);
//                }
//                else {
//                    CCLOG("Wrong tool: required=%s, selected=%s", requiredTool.c_str(), selectedItem.c_str());
//                    canInteract = false;  // 明确设置为false
//                }
//            }
//
//            if (canInteract) {
//                CCLOG("!!!!click_count:%d", click_count);
//                this->show_click_bar();
//                this->update();
//            }
//        }
//    }
//}

void getable_goods::show_click_bar()
{
    // 显示进度条
    if (!click_bar) {
        // 未创建则创建并显示
        click_bar = progress_bar::create();
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
        this->getParent()->addChild(click_bar, 3);
    }
    else {
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
    }

    // 计算当前进度百分比
    float progressValue = (float)click_count / (float)GOODS_CLICK_MAP.at(sprite_name) * 100.0f;

    click_bar->update_progress_bar(progressValue);

    // 3秒后隐藏进度条
    this->scheduleOnce([=](float delta) {
        hide_click_bar();
        }, 3.0f, "hide_progress_key_3s");
}

void getable_goods::hide_click_bar()
{
    if (click_bar) {
        click_bar->hide_progress_bar();
    }
}
void getable_goods::update()
{
    // 判断是否采集完成
    if (click_count >= GOODS_CLICK_MAP.at(sprite_name))
    {
        this->hide_click_bar();
        // 添加到背包
        // 安全获取BackpackLayer实例，避免访问已销毁的对象
        auto* safeBackpackLayer = BackpackLayer::getInstance();
        if (safeBackpackLayer) {
            safeBackpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
            if (sprite_name == "bigstone")
                safeBackpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
        }
        else {
            CCLOG("Warning: BackpackLayer instance is null in getable_goods::update()");
        }
        // 增加经验值
        Player* player = Player::getInstance("me");
        player->playerproperty.addExperience(EXPERIENCE * GOODS_CLICK_MAP.at(sprite_name));
        if (sprite_name == "badGreenhouse") {
            this->setSpriteFrame("newGreenhouse.png");
            is_getable = 0;
        }
        else {
            this->setTexture(transparent_texture); // 设为透明纹理 (移除显示)
            click_count = 0; // 重置计数
            is_getable = 0;
        }
    }
}

void getable_goods::add_in(ValueMap dict, getable_goods* sprite, std::string name, cocos2d::TMXTiledMap* tileMap)
{
    float posX = dict["x"].asFloat();
    float posY = dict["y"].asFloat();
    float width = dict["width"].asFloat();
    float height = dict["height"].asFloat();

    // 设置精灵属性
    sprite->set_info(name, Size(width * MapSize, height * MapSize));
    sprite->setPosition(Vec2(posX, posY));        // 位置
    sprite->setAnchorPoint(Vec2(0, 0));     // 锚点
    sprite->setContentSize(Size(width, height));  // 设置大小
    tileMap->addChild(sprite, 2);  // 添加到瓦片地图
    // sprite->init_mouselistener();
    sprite->setImag();
}

GoodsManager* GoodsManager::create()
{
    GoodsManager* ret = new (std::nothrow) GoodsManager();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}
// 添加精灵到列表
void GoodsManager::add_goods(getable_goods* sprite) {
    goods.push_back(sprite);
}

void GoodsManager::get_getable_count()
{
    getable_count = 0;
    // 统计当前可采集物品数量
    for (auto it = goods.begin(); it != goods.end(); ++it) {
        getable_count = (*it)->get_is_getable();
    }
}

// 随机生成逻辑
void GoodsManager::random_access()
{
    get_getable_count();
    // 检查当前可采集物品数量是否低于设定百分比
    if (!goods.empty() && getable_count <= goods.size() * PERCENT)
    {
        auto it = goods.begin();  // 获取迭代器起始

        std::advance(it, rand() % goods.size());  // 随机跳转到某位置
        if ((*it)->get_is_getable() == 0) {
            (*it)->setImag();
        }

    }
}

void GoodsManager::start_scheduler() {

    // 开启定时器，每 6 秒执行一次 randomAccess
    this->schedule([this](float delta) {
        this->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
}

void GoodsManager::stop_scheduler() {
    // 停止定时器
    this->unschedule("RandomAccessSchedulerKey");
}


// 【观察者模式】
// 自动注册
void getable_goods::onEnter() {
    CCLOG("[GetableGoods] onEnter called for goods %p (%s)", this, sprite_name.c_str());
    Sprite::onEnter();
    InteractionManager::getInstance()->registerObject(this);
    CCLOG("[GetableGoods] Goods %p registered", this);
}

// 自动注销
void getable_goods::onExit() {
    CCLOG("[GetableGoods] onExit called for goods %p (%s)", this, sprite_name.c_str());
    InteractionManager::getInstance()->unregisterObject(this);
    Sprite::onExit();
    CCLOG("[GetableGoods] Goods %p unregistered", this);
}

// 1. 获取对象的包围盒（必须返回世界坐标系下的 Rect）
cocos2d::Rect getable_goods::getBoundingBoxWorld() {
    // 获取世界坐标原点
    Vec2 worldPos = this->convertToWorldSpace(Vec2::ZERO);

    // 你的原有逻辑是：sprite_size 存储了 (width * MapSize, height * MapSize)
    // 但在 add_in 中，sprite_size 设置为 width * MapSize 和 height * MapSize 
    // 让我们使用 size_size 作为包围盒的尺寸。

    cocos2d::Rect bbox = cocos2d::Rect(
        worldPos.x,
        worldPos.y,
        sprite_size.width,
        sprite_size.height
    );
    CCLOG("[GetableGoods] getBoundingBoxWorld for %s %p: (%.2f, %.2f, %.2f, %.2f)",
        sprite_name.c_str(), this, bbox.origin.x, bbox.origin.y, bbox.size.width, bbox.size.height);
    return bbox;
}


// 3. 核心响应方法
bool getable_goods::onInteract(const InteractContext& ctx) {
    CCLOG("[GetableGoods] onInteract called for %s %p", sprite_name.c_str(), this);
    CCLOG("[GetableGoods] Context - isInControl: %s, toolName: '%s', toolLevel: %d",
        ctx.isInControl ? "true" : "false", ctx.toolName.c_str(), ctx.toolLevel);

    // 1. 检查是否可获取 (is_getable)
    if (!this->isInteractable()) {
        CCLOG("[GetableGoods] Interaction rejected - not currently getable");
        return false;
    }

    // 2. 检查是否在控制范围内 (除非不需要工具)
    std::string requiredTool = GOODS_MAP.at(sprite_name).at("tool");

    // 如果需要工具，必须在控制范围内
    if (!requiredTool.empty() && !ctx.isInControl) {
        CCLOG("[GetableGoods] Interaction rejected - tool required but not in control range");
        return false;
    }

    // 3. 检查工具匹配
    bool canInteract = false;

    if (requiredTool.empty()) { // 不需要工具 (例如：badGreenhouse)
        canInteract = true;
        click_count += 1; // 增加点击计数
    }
    else {
        // 需要工具：检查选中的工具名称是否匹配
        if (ctx.toolName == requiredTool) {
            canInteract = true;
            // 根据工具等级增加点击次数
            click_count += ctx.toolLevel;
            CCLOG("[GetableGoods] Tool matched: required=%s, selected=%s, click_count=%d",
                requiredTool.c_str(), ctx.toolName.c_str(), click_count);
        }
        else {
            CCLOG("[GetableGoods] Wrong tool: required=%s, selected=%s",
                requiredTool.c_str(), ctx.toolName.c_str());
            // 工具不匹配，返回 false，事件继续传播 (例如：用斧头点石头，让移动逻辑接管)
            return false;
        }
    }

    // 4. 执行交互逻辑
    if (canInteract) {
        CCLOG("!!!!click_count:%d", click_count);
        this->show_click_bar();
        this->update();
        return true; // 事件已处理
    }

    return false; // 不应该到达这里，除非逻辑有误
}