#include "crop.h"
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
        crop_sprite->init_mouselistener();
        CCLOG("Creation cope successfully!");
        return crop_sprite;
    }
    CCLOG("Creation cope unsuccessfully!");
    CC_SAFE_DELETE(crop_sprite);
    return nullptr;
}

// 初始化鼠标监听器
void crop::init_mouselistener()
{
    // 创建鼠标监听器
    auto listener = cocos2d::EventListenerMouse::create();

    // 鼠标按下时的回调
    listener->onMouseDown = CC_CALLBACK_1(crop::on_mouse_click, this);
    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
// 鼠标按下时的回调
void crop::on_mouse_click(cocos2d::Event* event)
{
    /*------------------------------------------------------renew-------------------------------------------------------------*/
    //获取作物位置
    Vec2 crop_pos = this->convertToWorldSpace(Vec2(0, 0));
    // 计算点击的有效范围
    float min_x = crop_pos.x;
    float max_x = crop_pos.x + crop_size.width * MapSize;
    float min_y = crop_pos.y;
    float max_y = crop_pos.y + crop_size.height * MapSize;
    if (is_in_control) {
        if ((MOUSE_POS.x > min_x &&
            MOUSE_POS.x < max_x &&
            MOUSE_POS.y > min_y &&
            MOUSE_POS.y < max_y))
        {
            CCLOG("click crop:%d", develop_level);
            switch (develop_level)
            {
            case -1: //作物枯萎，点击铲除
                CCLOG("clear the dead crop");
                this->clear();
                break;
            case 5: //作物成熟，点击收获
                CCLOG("harvest the crop");
                this->harvest();
            case 0://此时为空地
                if (CROP_MAP.count(backpackLayer->getSelectedItem())) //手上拿的物品是植物种子
                {
                    CCLOG("plant a crop");
                    this->planting(backpackLayer->getSelectedItem());
                }
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                if (backpackLayer->getSelectedItem().find("Can") != std::string::npos) //手上的工具为水壶，执行浇水
                {
                    CCLOG("water this crop");
                    this->water(backpackLayer->getSelectedItem());
                }
                else if (backpackLayer->getSelectedItem().find("fertilizer") != std::string::npos)//手上的工具为肥料，执行施肥
                {
                    CCLOG("fertilize this crop");
                    this->fertilize(backpackLayer->getSelectedItem());
                }
                else
                {
                    CCLOG("%s couldn't do anything to the crop", backpackLayer->getSelectedItem().c_str());
                }
                break;
            default:
                CCLOG("ERROR develop_level!!!!");
                break;
            }

        }
    }
}

//浇水,根据工具等级，工具等级越高，浇一次水的有效次数越多
void crop::water(std::string name)
{
    if (watered_today)//如果今天还有浇水次数
    {
        char last_char = name[name.size() - 1];
        int level = last_char - '0';
        watered_today = std::min(watered_today - level, 0);
        this->setSpriteFrame("water.png");

        // 在 0.2 秒后恢复原图
        this->scheduleOnce([this](float dt) {
            this->setSpriteFrame(crop_name + "-" + std::to_string(this->develop_level) + ".png");
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
    if (CROP_MAP.at(name).at("season") == timeSystem->getSeason()) //此时是种植这个作物的季节
    {
        crop_name = name;
        develop_level = 1;
        develop_day = CROP_MAP.at(name).at("develop_day");
        std::string framename = this->crop_name + "-1.png";//显示第一阶段种植图片，表示种植成功
        this->setSpriteFrame(framename);
        this->setScale(MAG_TIME_CROP);

        //背包里种子-1
        backpackLayer->removeItem(name);
        CCLOG("plant successfully");
    }
    else
        CCLOG("the crop couldn't be planted in this season");
}

//施肥,生长进度++，根据工具等级，工具等级越高，生长进度增加越多
void crop::fertilize(std::string name)
{
    char last_char = name[name.size() - 1];
    int level = last_char - '0';
    develop_level = std::min(develop_level + level, 5);
    std::string framename = this->crop_name + "-" + std::to_string(this->develop_level) + ".png";
    this->setSpriteFrame(framename);

    //背包里肥料-1
    backpackLayer->removeItem(name);
    CCLOG("fertilize successfully");
}

//丰收，每次收获增加EXPERIENCE经验值
void crop::harvest()
{
    //把生成物加入背包 
    backpackLayer->addItem(HARVEST_MAP.at(crop_name));
    //人物经验增加10
    Player* player = Player::getInstance("me");
    player->playerproperty.addExperience(EXPERIENCE);
    //清空土地
    this->clear();
}

//清除
void crop::clear()
{
    water_count = 0;
    unwater_count = 0;
    develop_day = 0;
    develop_level = 0;
    crop_name = "";
    watered_today = 2 + timeSystem->getweather();
    this->initWithTexture(transparent_texture);
}

//更新
void crop::update_day(float deltaTime)
{
    if (timeSystem->getDay() != now_day)//今天结束了
    {

        if (develop_level > 0) {
            if (watered_today)//说明今天浇水次数没有达到要求
            {
                CCLOG("unwater today");
                unwater_count++;
                if (unwater_count == DIE_DRY) {
                    develop_level = -1;
                    this->setSpriteFrame("-1.png");
                    CCLOG("the crop die");
                }
            }
            else//今天浇水达到要求
            {
                CCLOG("water successfully today:%d", water_count);
                water_count++;
                //查看是否需要更新成长状态
                if (water_count > develop_level * develop_day && water_count <= MAX_LEVEL * develop_day) {
                    this->develop_level++;
                    //更新显示的成长状态
                    std::string framename = this->crop_name + "-" + std::to_string(this->develop_level) + ".png";
                    this->setSpriteFrame(framename);
                }
            }
        }
        now_day = timeSystem->getDay();
        watered_today = WATER_PRED + timeSystem->getweather();
    }
}