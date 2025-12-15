#include "crop.h"
#include "InteractionManager.h"
#include "StrategyContext.h"
#include "Data/GameData.h"

//定义常量
#define MAG_TIME_CROP 1.5f
// 定义静态成员变量并初始化
cocos2d::Texture2D* crop::transparent_texture = nullptr;
cocos2d::Size crop::crop_size = cocos2d::Size(0, 0);

// 创建实例
crop* crop::create(const std::string& plist_name, float width, float height)
{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);
    crop_size.width = width;
    crop_size.height = height;

    //创建实例
    crop* crop_sprite = new crop();

    // 创建透明的内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = width * height * 4;  // 每个像素 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值都为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, width, height, cocos2d::Size(width, height));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;


    //判断是否能成功创建
    if (transparentTexture)
    {
        crop_sprite->initWithTexture(transparentTexture);
        crop_sprite->autorelease();
        //crop_sprite->init_mouselistener();
        CCLOG("Creation cope successfully!");
        return crop_sprite;
    }
    CCLOG("Creation cope unsuccessfully!");
    CC_SAFE_DELETE(crop_sprite);
    return nullptr;
}

//// 初始化鼠标监听器
//void crop::init_mouselistener()
//{
//    // 创建鼠标监听器
//    auto listener = cocos2d::EventListenerMouse::create();
//
//    // 鼠标按下时的回调
//    listener->onMouseDown = CC_CALLBACK_1(crop::on_mouse_click, this);
//    // 获取事件分发器，添加监听器
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//}
//// 鼠标按下时的回调
//void crop::on_mouse_click(cocos2d::Event* event)
//{
//    /*------------------------------------------------------renew-------------------------------------------------------------*/
//    //获取作物位置
//    Vec2 crop_pos = this->convertToWorldSpace(Vec2(0, 0));
//    // 计算点击的有效范围
//    float min_x = crop_pos.x;
//    float max_x = crop_pos.x + crop_size.width * MapSize;
//    float min_y = crop_pos.y;
//    float max_y = crop_pos.y + crop_size.height * MapSize;
//    if (is_in_control) {
//        if ((MOUSE_POS.x > min_x &&
//            MOUSE_POS.x < max_x &&
//            MOUSE_POS.y > min_y &&
//            MOUSE_POS.y < max_y))
//        {
//            CCLOG("click crop:%d", develop_level);
//            switch (develop_level)
//            {
//            case -1: //作物枯萎，点击铲除
//                CCLOG("clear the dead crop");
//                this->clear();
//                break;
//            case 5: //作物成熟，点击收获
//                CCLOG("harvest the crop");
//                this->harvest();
//            case 0://此时为空地
//                if (CROP_MAP.count(backpackLayer->getSelectedItem())) //手上拿的物品是植物种子
//                {
//                    CCLOG("plant a crop");
//                    this->planting(backpackLayer->getSelectedItem());
//                }
//                break;
//            case 1:
//            case 2:
//            case 3:
//            case 4:
//                if (backpackLayer->getSelectedItem().find("Can") != std::string::npos) //手上的工具为水壶，执行浇水
//                {
//                    CCLOG("water this crop");
//                    this->water(backpackLayer->getSelectedItem());
//                }
//                else if (backpackLayer->getSelectedItem().find("fertilizer") != std::string::npos)//手上的工具为肥料，执行施肥
//                {
//                    CCLOG("fertilize this crop");
//                    this->fertilize(backpackLayer->getSelectedItem());
//                }
//                else
//                {
//                    CCLOG("%s couldn't do anything to the crop", backpackLayer->getSelectedItem().c_str());
//                }
//                break;
//            default:
//                CCLOG("ERROR develop_level!!!!");
//                break;
//            }
//
//        }
//    }
//}

//浇水,根据工具等级，工具等级越高，浇一次水的有效次数越多
void crop::water(std::string name)
{
    //种了东西才浇水
    if (!_model) return;
    if (watered_today)//如果今天还有浇水次数
    {
        char last_char = name[name.size() - 1];
        int level = last_char - '0';
        watered_today = std::min(watered_today - level, 0);
        this->setSpriteFrame("water.png");

        // 在 0.2 秒后恢复原图
        this->scheduleOnce([this](float dt) {
            //this->setSpriteFrame(crop_name + "-" + std::to_string(this->develop_level) + ".png");
            this->setSpriteFrame(_model->id + "-" + std::to_string(this->develop_level) + ".png");
            }, 0.2f, "reset_texture_crop");

        //背包水-1
        backpackLayer->removeItem(name);
        CCLOG("water successfully");
    }
    else
        CCLOG("couldn't water today");
}

//种植
void crop::planting(std::string name) {

	//从GameData获取作物模型
    CropModel* targetModel = GameData::getInstance()->getCropModel(name);
    if (!targetModel) {
        CCLOG("Error: Unknown crop seed: %s", name.c_str());
        return;
    }

    // 检查季节
    if (targetModel->season == timeSystem->getSeason())
    {
        _model = targetModel; // 绑定享元
        // crop_name = name; // 删除
        develop_level = 1;
        // develop_day = ... // 删除

        // 初始化浇水进度追踪组件
        waterTracker.setup(
            [this]() {
                // 条件：浇水次数 > 生长阶段 * 每阶段天数
                return _model && waterTracker.getCount() > develop_level * _model->developDays && develop_level < MAX_LEVEL;
            },
            [this]() {
                // 动作：升级
                develop_level++;
                std::string framename = _model->id + "-" + std::to_string(develop_level) + ".png";
                this->setSpriteFrame(framename);
                CCLOG("[Crop] Level up to %d", develop_level);
            }
        );
        waterTracker.reset();

        // 初始化每日更新组件
        dailyUpdater.setup(
            [this]() { return timeSystem->getDay(); },  // 获取当前天数
            [this]() {  // 每日执行的逻辑
                if (_model && develop_level > 0) {
                    if (watered_today) {
                        CCLOG("unwater today");
                        unwater_count++;
                        if (unwater_count == DIE_DRY) {
                            develop_level = -1;
                            this->setSpriteFrame("-1.png");
                            CCLOG("the crop die");
                        }
                    }
                    else {
                        // 使用进度追踪组件累计浇水次数
                        waterTracker.increment();
                        CCLOG("water successfully today, total count:%d", waterTracker.getCount());
                        // 检查并触发升级
                        waterTracker.checkAndTrigger();
                    }
                }
            },
            [this]() {  // 每日重置逻辑
                watered_today = WATER_PRED + timeSystem->getweather();
            }
        );

        std::string framename = _model->id + "-1.png";
        this->setSpriteFrame(framename);
        this->setScale(MAG_TIME_CROP);

        backpackLayer->removeItem(name);
        CCLOG("plant successfully: %s", _model->id.c_str());
    }
    else
        CCLOG("the crop couldn't be planted in this season");

    //if (CROP_MAP.at(name).at("season") == timeSystem->getSeason()) //此时是种植这个作物的季节
    //{
    //    crop_name = name;
    //    develop_level = 1;
    //    develop_day = CROP_MAP.at(name).at("develop_day");
    //    std::string framename = this->crop_name + "-1.png";//显示第一阶段种植图片，表示种植成功
    //    this->setSpriteFrame(framename);
    //    this->setScale(MAG_TIME_CROP);

    //    //背包里种子-1
    //    backpackLayer->removeItem(name);
    //    CCLOG("plant successfully");
    //}
    //else
    //    CCLOG("the crop couldn't be planted in this season");
}

//施肥,生长进度++，根据工具等级，工具等级越高，生长进度增加越多
void crop::fertilize(std::string name)
{
    if (!_model) return;

    char last_char = name[name.size() - 1];
    int level = last_char - '0';
    develop_level = std::min(develop_level + level, 5);
    //std::string framename = this->crop_name + "-" + std::to_string(this->develop_level) + ".png";
    std::string framename = _model->id + "-" + std::to_string(this->develop_level) + ".png";
    this->setSpriteFrame(framename);

    //背包里肥料-1
    backpackLayer->removeItem(name);
    CCLOG("fertilize successfully");
}

//丰收，每次收获增加EXPERIENCE经验值
void crop::harvest()
{
    // 使用收获组件执行收获逻辑
    harvester.harvest();
}

//清除
void crop::clear()
{
    waterTracker.reset();
    unwater_count = 0;
    //develop_day = 0;
    develop_level = 0;
    //crop_name = "";
    _model = nullptr;
    watered_today = 2 + timeSystem->getweather();
    this->initWithTexture(transparent_texture);

	this->setScale(1.0f); //重置缩放
}

//更新
void crop::update_day(float deltaTime)
{
    // 使用每日更新组件
    dailyUpdater.update(deltaTime);
}

//【观察者模式】
// 自动注册
void crop::onEnter() {
    CCLOG("[Crop] onEnter called for crop %p", this);
    Sprite::onEnter();
    CCLOG("[Crop] Registering crop %p with InteractionManager", this);
    InteractionManager::getInstance()->registerObject(this);
    CCLOG("[Crop] Crop %p registration completed", this);
}

// 自动注销
void crop::onExit() {
    CCLOG("[Crop] onExit called for crop %p", this);
    CCLOG("[Crop] Unregistering crop %p from InteractionManager", this);
    InteractionManager::getInstance()->unregisterObject(this);
    Sprite::onExit();
    CCLOG("[Crop] Crop %p unregistration completed", this);
}

// 替代原来的位置计算
cocos2d::Rect crop::getBoundingBoxWorld() {
    // 获取世界坐标原点
    Vec2 worldPos = this->convertToWorldSpace(Vec2::ZERO);
    // 使用 MapSize 进行放大 (保持你原有的逻辑)
    cocos2d::Rect bbox = cocos2d::Rect(
        worldPos.x,
        worldPos.y,
        crop_size.width * MapSize,
        crop_size.height * MapSize
    );
    CCLOG("[Crop] getBoundingBoxWorld for crop %p: (%.2f, %.2f, %.2f, %.2f)", 
          this, bbox.origin.x, bbox.origin.y, bbox.size.width, bbox.size.height);
    return bbox;
}

//【原有逻辑】
// 核心业务逻辑 (完全去除了坐标判断和背包查找)
//bool crop::onInteract(const InteractContext& ctx) {
//    CCLOG("[Crop] onInteract called for crop %p", this);
//    CCLOG("[Crop] Context - isInControl: %s, toolName: '%s', toolLevel: %d, develop_level: %d", 
//          ctx.isInControl ? "true" : "false", ctx.toolName.c_str(), ctx.toolLevel, develop_level);
//    
//    // 1. 必须在控制范围内
//    if (!ctx.isInControl) {
//        CCLOG("[Crop] Interaction rejected - not in control range");
//        return false;
//    }
//
//    // 辅助 lambda：重新拼凑工具全名 (例如 "Can" + 1 -> "Can1")
//    // 你的旧函数依赖这个全名来解析等级和扣除背包物品
//    auto getFullToolName = [&]() -> std::string {
//        return ctx.toolName + std::to_string(ctx.toolLevel);
//        };
//
//    switch (develop_level) {
//    case -1: // 枯萎
//        CCLOG("[Crop] Handling withered crop - clearing");
//        //CCLOG("clear the dead crop");
//        this->clear(); // 直接调用现有函数
//        break;
//
//    case 5: // 收获
//        CCLOG("[Crop] Handling mature crop - harvesting");
//        //CCLOG("harvest the crop");
//        this->harvest(); // 直接调用现有函数
//        break;
//
//    case 0: // 空地：种植
//        CCLOG("[Crop] Handling empty soil - checking for planting");
//        // 种子比较特殊，它不在 ctx.toolName (因为那只存工具)，
//        // 所以我们这里还是需要访问背包获取当前拿的物品全名
//        if (backpackLayer && !backpackLayer->getSelectedItem().empty()) {
//            std::string currentItem = backpackLayer->getSelectedItem();
//            CCLOG("[Crop] Current selected item: '%s'", currentItem.c_str());
//            // 检查是否是种子
//            if (CROP_MAP.count(currentItem)) {
//                CCLOG("[Crop] Item is a valid seed - planting");
//                //CCLOG("plant a crop");
//                this->planting(currentItem); // 直接调用现有函数
//            } else {
//                CCLOG("[Crop] Item is not a valid seed");
//            }
//        } else {
//            CCLOG("[Crop] No item selected or backpack is null");
//        }
//        break;
//
//    case 1: case 2: case 3: case 4: // 成长阶段
//        CCLOG("[Crop] Handling growing crop - checking tool type");
//        if (ctx.toolName == "Can") {
//            // 这里的判断移到了 water 函数内部，但我们在外部做一个简单的工具类型检查
//            CCLOG("[Crop] Tool is watering can - attempting to water");
//            //CCLOG("water this crop");
//            // 传入拼凑好的名字 "Can1"
//            this->water(getFullToolName());
//        }
//        else if (ctx.toolName == "fertilizer") {
//            CCLOG("[Crop] Tool is fertilizer - attempting to fertilize");
//            //CCLOG("fertilize this crop");
//            // 传入拼凑好的名字 "fertilizer1"
//            this->fertilize(getFullToolName());
//        }
//        else {
//            // 如果是其他工具（比如锄头点到了作物），这里可以加个日志或者什么都不做
//            CCLOG("[Crop] Tool '%s' cannot be used on growing crops", ctx.toolName.c_str());
//            //CCLOG("%s couldn't do anything to the crop", ctx.toolName.c_str());
//        }
//        break;
//
//    default:
//        CCLOG("[Crop] ERROR: Invalid develop_level %d", develop_level);
//        //CCLOG("ERROR develop_level!!!!");
//        break;
//    }
//
//    CCLOG("[Crop] onInteract completed successfully for crop %p", this);
//    return true;
//}
//【策略模式】
bool crop::onInteract(const InteractContext& ctx) {
    if (!ctx.isInControl) return false;

    // 特殊处理种植 (因为种植不需要点击已有的 crop，而是点击空地，且判定逻辑依赖具体的种子表)
    // 如果是空地 (level 0) 且手持种子，这里保留原有逻辑或写一个 PlantingStrategy
    //if (develop_level == 0) {
    //    if (backpackLayer && CROP_MAP.count(backpackLayer->getSelectedItem())) {
    //        this->planting(backpackLayer->getSelectedItem());
    //        return true;
    //    }
    //}
    if (develop_level == 0) {
        if (backpackLayer) {
            std::string item = backpackLayer->getSelectedItem();
            // 检查 item 是否是种子
            if (GameData::getInstance()->getCropModel(item) != nullptr) {
                this->planting(item);
                return true;
            }
        }
    }

    // 其他所有逻辑委托给策略管理器
    return InteractionStrategyContext::getInstance()->handleInteraction(ctx, this);
}