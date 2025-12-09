#include "animals.h"

cocos2d::Texture2D* animals::transparent_texture = nullptr;
int animals::count = 0;

animals::animals()
{
    produce = new Sprite();
    ID = ++count;
}

// 保存基本信息
void animals::set_info(std::string name, cocos2d::Vec2 pos, cocos2d::Size size)
{
    // 设置基本信息
    animals_name = name;
    produce_size = size;
    produce_pos = pos;
    produce_day = ANIMAL_MAP.at(animals_name);
    // 创建透明的内存块
    int dataSize = size.width * size.height * 4; 
    unsigned char* transparentData = new unsigned char[dataSize];
    memset(transparentData, 0, dataSize);
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, size.width, size.height, size);
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;
}

// 创建实例
animals* animals::create(const std::string& plist_name)
{

    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);
    animals* animals_sprite = new animals();
    
    if (animals_sprite)
    {
        CCLOG("Creation animal successfully!");
        animals_sprite->autorelease();
        animals_sprite->init_mouselistener();
        return animals_sprite;
    }
    CCLOG("Creation animal unsuccessfully!");
    CC_SAFE_DELETE(animals_sprite);
    return nullptr;
}

// 图片生成
void animals::set_imag()
{
    if (produce && transparent_texture) {
        this->setSpriteFrame(animals_name + "-front.png");
        produce->initWithTexture(transparent_texture);
        produce->autorelease();
        produce->setPosition(produce_pos);
    }
}

// 初始化鼠标监听器
void animals::init_mouselistener()
{
    auto listener = cocos2d::EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(animals::on_mouse_click, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 鼠标按下时的回调
void animals::on_mouse_click(cocos2d::Event* event)
{
    auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
    auto mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
    auto animals_pos = this->getPosition();
    auto animals_size = this->getContentSize();
    if (is_in_control) {
        if (mouse_pos.x > animals_pos.x - 5 &&
            mouse_pos.x < animals_pos.x + animals_size.width + 5 &&
            mouse_pos.y > animals_pos.y - 5 &&
            mouse_pos.y < animals_pos.y + animals_size.height + 5)
        {
            CCLOG("animals clicked");
            if (backpackLayer->getSelectedItem() == FOOD) {
                this->feed();
            }
        }
        else if (mouse_pos.x > produce_pos.x - produce_size.width / 2 &&
            mouse_pos.x < produce_pos.x + produce_size.width / 2 &&
            mouse_pos.y > produce_pos.y - produce_size.height / 2 &&
            mouse_pos.y < produce_pos.y + produce_size.height / 2)
        {
            CCLOG("produce clicked");
            if (is_produce) {
                this->harvest();
            }
        }
        else {
            CCLOG("misclicked");
        }
    }
}

// 喂养
void animals::feed()
{
    if (feed_today) {
        feed_today--;
        backpackLayer->removeItem(FOOD);
        CCLOG("feed successfully");
    }
    else
        CCLOG("couldn't feed today");
}

// 生成附属品
void animals::create_produce()
{
    if (feed_count % produce_day == 0 && feed_count)
    {
        produce->setSpriteFrame(animals_name + "-produce.png");//显示生成物
        is_produce = 1;
        CCLOG("create produce");
    }
}

// 收获功能
void animals::harvest()
{
    //把生成物加入背包,并获得经验值
    backpackLayer->addItem(PRODUCE_MAP.at(animals_name));
    Player* player = Player::getInstance("me");
    player->playerproperty.addExperience(EXPERIENCE);
    produce->setTexture(transparent_texture);
    is_produce = 0;
}


// 游荡
void animals::randmove(cocos2d::TMXTiledMap* tileMap)
{
    unsigned int timestamp = static_cast<unsigned int>(time(0)) * 1000 + static_cast<unsigned int>(clock()) / (CLOCKS_PER_SEC / 1000);

    // 获取进程ID（进程间的差异）
    unsigned int pid = static_cast<unsigned int>(getpid());

    // 获取线程ID（如果有多线程）
    unsigned int tid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    // 结合时间戳、进程ID、线程ID来生成一个复杂的种子
    unsigned int seed = timestamp ^ pid ^ tid;

    srand(seed + ID);
    dic = (rand() % rand() + ID) % 4;
    movement[dic] = 1;

    // 定时调用 move_act，每秒调用一次
    this->schedule([this, tileMap](float) {
        move_act(tileMap);
        }, 0.1f, "move_act_key");  

    // 2秒后执行回调函数一次
    this->scheduleOnce([this](float dt) {
        movement[dic] = 0;
        }, 2.0f, "one_time_schedule");

}

// 移动动作
void animals::move_act(cocos2d::TMXTiledMap* tileMap)
{
    for (int i = 0; i < 4; i++) {
        is_hit_edge[i] = false;
    }

    //获取精灵的位置
    auto sprite_pos = this->getPosition();
    cocos2d::Size spriteSize = this->getContentSize();
    cocos2d::Size mapSize = tileMap->getMapSize();
    cocos2d::Size tileSize = tileMap->getTileSize();
    mapSize.width *= tileSize.width;
    mapSize.height *= tileSize.height;
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // 判断精灵是否超出边界
    if (sprite_pos.y + spriteSize.height / 2 >= mapSize.height - EDGE1) {
        is_hit_edge[0] = true;
        //CCLOG("Sprite hit the top edge");
    }
    else  if (sprite_pos.y - spriteSize.height - EDGE0 <= 0) {
        is_hit_edge[1] = true;
        // CCLOG("Sprite hit the bottom edge");
    }
    if (sprite_pos.x - spriteSize.width - EDGE0 <= 0) {
        is_hit_edge[2] = true;
        //CCLOG("Sprite hit the left edge");
    }
    else if (sprite_pos.x + spriteSize.width / 2 >= mapSize.width - EDGE1) {
        is_hit_edge[3] = true;
        //CCLOG("Sprite hit the right edge");
    }
    for (int i = 0; i < 4; i++) {
        if (movement[i] && !is_hit_edge[i]) {
            //创建移动动作
            std::string dic[4] = { "-back","-front","-left","-right" };
            this->setSpriteFrame(animals_name + dic[i] + ".png");
            auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[i], move_vecy[i]));
            this->runAction(move_action);
        }
        else if (movement[i] && is_hit_edge[i]) {
            movement[i] = 0;
            movement[(i / 2) * 2 + 1 - i % 2] = 1;
        }
    }

}

// 不定时游荡
void animals::scheduleRandomMove(cocos2d::TMXTiledMap* tileMap) {

    // 每5秒随机移动一次
    this->schedule([this, tileMap](float dt) {
        randmove(tileMap);
        }, 5.0f, "random_move_key");

}

// 新一天的更新
void animals::update_day(float deltaTime)
{
    if (timeSystem->getDay() != now_day)//今天结束了
    {
        if (animals_name != "") {
            if (feed_today == 0)//说明今天喂养次数达到要求
            {
                feed_count++;
                //查看是否需要更新成长状态
                this->create_produce();
            }
        }
        now_day = timeSystem->getDay();
        feed_today = 1;
    }
}

AnimalsManager* AnimalsManager::create()
{
    AnimalsManager* ret = new (std::nothrow) AnimalsManager();
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
void AnimalsManager::add_animals(animals* sprite) {
    animals_list.push_back(sprite);
}

void AnimalsManager::schedule_animals()
{
    // 迭代器遍历访问精灵
    for (auto it = animals_list.begin(); it != animals_list.end(); ++it) {
        auto animal = *it;                      // 获取指针，指向精灵
        animal->schedule([animal](float dt) {   // 捕获指针 animal
            animal->update(dt); 
            }, "update_animal");
    }
}