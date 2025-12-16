#include "animals.h"
#include "Data/GameData.h"

cocos2d::Texture2D* animals::transparent_texture = nullptr;
int animals::count = 0;

animals::animals()
{
    //produce = new Sprite();
    produce = Sprite::create();
    produce->retain();
    ID = ++count;
}

// 保存基本信息
void animals::set_info(std::string name, cocos2d::Vec2 pos, cocos2d::Size size)
{
    //animals_name = name;
    _model = GameData::getInstance()->getAnimalModel(name);
    produce_size = size;
    produce_pos = pos;
    //produce_day = ANIMAL_MAP.at(animals_name);

    // 初始化喂食进度追踪组件
    feedTracker.setup(
        [this]() {
            // 条件：喂食次数是产出周期的倍数
            return _model && feedTracker.getCount() > 0 && feedTracker.getCount() % _model->produceCycle == 0;
        },
        [this]() {
            // 动作：生成产出物
            if (produce && _model) {
                produce->setSpriteFrame(_model->id + "-produce.png");
                is_produce = 1;
                CCLOG("[Animals] Produce created: %s", (_model->id + "-produce.png").c_str());
            }
        }
    );
    feedTracker.reset();

    // 初始化每日更新组件
    dailyUpdater.setup(
        [this]() { return timeSystem->getDay(); },  // 获取当前天数
        [this]() {  // 每日执行的逻辑
            if (_model) {
                if (feed_today == 0) {  // 说明今天喂过了
                    // 使用进度追踪组件累计喂食次数
                    feedTracker.increment();
                    CCLOG("[Animals] Feed count: %d", feedTracker.getCount());
                    this->create_produce();
                }
            }
        },
        [this]() {  // 每日重置逻辑
            feed_today = 1;
        }
    );
    // 初始化收获组件
    harvester.setupDynamic(
        [this]() { return _model ? _model->produceItem : ""; },  // 动态获取产品名称
        EXPERIENCE,                                                // 经验值
        [this]() {                                                 // 收获后清理
            produce->setTexture(transparent_texture);
            is_produce = 0;
        }
    );
    if (transparent_texture == nullptr) {
        int dataSize = size.width * size.height * 4;
        unsigned char* transparentData = new unsigned char[dataSize];
        memset(transparentData, 0, dataSize);
        transparent_texture = new cocos2d::Texture2D();
        transparent_texture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, size.width, size.height, size);
        delete[] transparentData;
    }
}

// 创建实例
animals* animals::create(const std::string& plist_name)
{

    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);
    animals* animals_sprite = new animals();
    
    if (animals_sprite && animals_sprite->init())
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

// 图片设置
void animals::set_imag()
{
    if (produce && transparent_texture && _model) {
        //this->setSpriteFrame(animals_name + "-front.png");
        this->setSpriteFrame(_model->id + "-front.png");
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
    if (!mouse_event) return;
    
    // 只响应右键点击
    if (mouse_event->getMouseButton() != cocos2d::EventMouse::MouseButton::BUTTON_RIGHT) {
        return;
    }
    
    auto mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
    auto animals_pos = this->getPosition();
    auto animals_size = this->getContentSize();
    
    // 非农场场景不需要is_in_control检查，农场场景需要
    bool canCheckClick = false;
    if (!is_infarm) {
        // 非农场场景：直接可以点击，不需要is_in_control
        canCheckClick = true;
    } else {
        // 农场场景：需要is_in_control
        canCheckClick = is_in_control;
    }
    
    if (canCheckClick) {
        if (mouse_pos.x > animals_pos.x - 5 &&
            mouse_pos.x < animals_pos.x + animals_size.width + 5 &&
            mouse_pos.y > animals_pos.y - 5 &&
            mouse_pos.y < animals_pos.y + animals_size.height + 5)
        {
            CCLOG("animals clicked");
            if (backpackLayer && backpackLayer->getSelectedItem() == FOOD) {
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

// 喂食
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
    if (!_model) return;

    // 使用进度追踪组件检查并触发产出
    feedTracker.checkAndTrigger();
}

// 收获功能
void animals::harvest()
{
    // 使用收获组件执行收获逻辑
    harvester.harvest();
}


// 游荡
void animals::randmove(cocos2d::TMXTiledMap* tileMap)
{
    // 这里的随机比较复杂，可以考虑更改
    unsigned int timestamp = static_cast<unsigned int>(time(0)) * 1000 + static_cast<unsigned int>(clock()) / (CLOCKS_PER_SEC / 1000);

    // 获取进程ID（不同进程的产物）
    unsigned int pid = static_cast<unsigned int>(getpid());

    // 获取线程ID（支持多线程）
    unsigned int tid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    // 将时间戳、进程ID和线程ID组合成一个更随机的种子
    unsigned int seed = timestamp ^ pid ^ tid;

    srand(seed + ID);
    dic = (rand() % rand() + ID) % 4;
    movement[dic] = 1;

    // 定时调度 move_act，每秒调用一次
    this->schedule([this, tileMap](float) {
        move_act(tileMap);
        }, 0.1f, "move_act_key");  

    // 2秒后执行回调，停止一次
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

    // 获取精灵位置
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
            // 执行移动动作
            std::string dirStr[4] = { "-back","-front","-left","-right" };
            this->setSpriteFrame(_model->id + dirStr[i] + ".png");
            //this->setSpriteFrame(animals_name + dic[i] + ".png");
            auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[i], move_vecy[i]));
            this->runAction(move_action);
        }
        else if (movement[i] && is_hit_edge[i]) {
            movement[i] = 0;
            movement[(i / 2) * 2 + 1 - i % 2] = 1;
        }
    }

}

// 开启随机移动调度器
void animals::scheduleRandomMove(cocos2d::TMXTiledMap* tileMap) {

    this->schedule([this, tileMap](float dt) {
        randmove(tileMap);
        }, 5.0f, "random_move_key");

}

// 新一天更新
void animals::update_day(float deltaTime)
{
    // 使用每日更新组件
    dailyUpdater.update(deltaTime);
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
// 添加精灵到队列
void AnimalsManager::add_animals(animals* sprite) {
    animals_list.push_back(sprite);
}

void AnimalsManager::schedule_animals()
{
    // 安排动物自身的更新调度
    for (auto it = animals_list.begin(); it != animals_list.end(); ++it) {
        auto animal = *it;                      
        animal->schedule([animal](float dt) {   
            animal->update(dt); 
            }, "update_animal");
    }
}