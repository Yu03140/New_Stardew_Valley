#include "getable_goods.h"


cocos2d::Texture2D* getable_goods::transparent_texture = nullptr;

void getable_goods::set_info(std::string name, Size size)
{
    sprite_name = name;
    sprite_size = size;
}

getable_goods* getable_goods::create(const std::string& plist_name)
{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //创建实例
    getable_goods* sprite = new getable_goods();

    // 创建透明的内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = DEFAULT_WIDTH * DEFAULT_HEIGHT * 4;  // 每个像素 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值都为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, DEFAULT_WIDTH, DEFAULT_HEIGHT, cocos2d::Size(DEFAULT_HEIGHT, DEFAULT_HEIGHT));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;

    //判断是否能成功创建
    if (transparentTexture)
    {
        sprite->initWithTexture(transparentTexture);
        sprite->autorelease();
        sprite->init_mouselistener();
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
    // 获取指定精灵帧
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name + ".png");
    this->initWithSpriteFrame(frame);
    is_getable = 1;
}

// 初始化鼠标监听器
void getable_goods::init_mouselistener()
{
    // 创建鼠标监听器
    auto listener = cocos2d::EventListenerMouse::create();

    // 鼠标回调
    listener->onMouseDown = CC_CALLBACK_1(getable_goods::on_mouse_click, this);

    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 鼠标按下时的回调
void getable_goods::on_mouse_click(cocos2d::Event* event)
{
    //获取物品位置
    Vec2 goods_pos = this->convertToWorldSpace(Vec2(0, 0));
    Vec2 mouse_pos;
    float min_x;
    float max_x;
    float min_y;
    float max_y;

    if (is_infarm)
    {
        mouse_pos = MOUSE_POS;
        min_x = goods_pos.x;
        max_x = goods_pos.x + sprite_size.width;
        min_y = goods_pos.y;
        max_y = goods_pos.y + sprite_size.height;
    }
    else {
        auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
        mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
        goods_pos = this->getPosition();
        min_x = goods_pos.x;
        max_x = goods_pos.x + sprite_size.width / MapSize;
        min_y = goods_pos.y;
        max_y = goods_pos.y + sprite_size.height / MapSize;
    }
    if (is_getable && is_in_control) {
        //鼠标点击位置在有效范围
        if ((mouse_pos.x > min_x &&
            mouse_pos.x < max_x &&
            mouse_pos.y > min_y &&
            mouse_pos.y < max_y))
        {
            CCLOG("good click");
            CCLOG("%s", sprite_name.c_str());
            if (GOODS_MAP.at(sprite_name).at("tool") == "" || (backpackLayer->getSelectedItem().find(GOODS_MAP.at(sprite_name).at("tool"))) != std::string::npos)//手上那个工具与物品匹配
            {
                char last_char = backpackLayer->getSelectedItem()[backpackLayer->getSelectedItem().size() - 1];
                int level = last_char - '0';
                click_count += level;
                CCLOG("!!!!click_count:%d", click_count);
                this->show_click_bar();
                this->update();
            }
            else
                CCLOG("wrong tool");
        }
    }
}

void getable_goods::show_click_bar()
{

    //显示进度条
    if (!click_bar) {
        // 如果进度条尚未创建，则创建并显示它
        click_bar = progress_bar::create();
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
        this->getParent()->addChild(click_bar, 3);
    }
    else {
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
    }

    // 计算当前的进度百分比
    float progressValue = (float)click_count / (float)GOODS_CLICK_MAP.at(sprite_name) * 100.0f;

    click_bar->update_progress_bar(progressValue);

    // 在点击后 3 秒隐藏进度条
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
    //判断是否能获得
    if (click_count >= GOODS_CLICK_MAP.at(sprite_name))
    {
        this->hide_click_bar();
        //把生成物加入背包 
        backpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
		if (sprite_name == "bigstone")
            backpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
        //人物经验增加
        Player* player = Player::getInstance("me");
        player->playerproperty.addExperience(EXPERIENCE * GOODS_CLICK_MAP.at(sprite_name));
        if (sprite_name == "badGreenhouse") {
            this->setSpriteFrame("newGreenhouse.png");
            is_getable = 0;
        }
        else {
            this->setTexture(transparent_texture);//设为透明
            click_count = 0;//清空
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

    // 创建透明纹理的精灵
    sprite->set_info(name, Size(width * MapSize, height * MapSize));
    sprite->setPosition(Vec2(posX, posY));        // 设置位置
    sprite->setAnchorPoint(Vec2(0, 0));     // 设置锚点
    sprite->setContentSize(Size(width, height));  // 设置大小
    tileMap->addChild(sprite, 2);  // 添加到瓦片地图
    sprite->init_mouselistener();
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
// 添加精灵到容器
void GoodsManager::add_goods(getable_goods* sprite) {
    goods.push_back(sprite);
}

void GoodsManager::get_getable_count()
{
    getable_count = 0;
    // 迭代器遍历访问精灵
    for (auto it = goods.begin(); it != goods.end(); ++it) {
        getable_count = (*it)->get_is_getable();
    }
}

// 随机访问一个精灵（通过迭代器）
void GoodsManager::random_access()
{
    get_getable_count();
    //如果当前可获取的物品数量过少，则随机重新生成
    if (!goods.empty() && getable_count <= goods.size() * PERCENT)
    {
        auto it = goods.begin();  // 获取容器的开始迭代器

        std::advance(it, rand() % goods.size());  // 随机跳转到某个位置
        if ((*it)->get_is_getable() == 0) {
            (*it)->setImag();
        }

    }
}

void GoodsManager::start_scheduler() {

    // 使用定时器，每 60 秒调用一次 randomAccess
    this->schedule([this](float delta) {
        this->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
}

void GoodsManager::stop_scheduler() {
    // 停止定时器
    this->unschedule("RandomAccessSchedulerKey");
}
