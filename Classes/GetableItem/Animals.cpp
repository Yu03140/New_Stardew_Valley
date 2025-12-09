#include "animals.h"

cocos2d::Texture2D* animals::transparent_texture = nullptr;
int animals::count = 0;

animals::animals()
{
    produce = new Sprite();
    ID = ++count;
}

// ���������Ϣ
void animals::set_info(std::string name, cocos2d::Vec2 pos, cocos2d::Size size)
{
    // ���û�����Ϣ
    animals_name = name;
    produce_size = size;
    produce_pos = pos;
    produce_day = ANIMAL_MAP.at(animals_name);
    // ����͸�����ڴ��
    int dataSize = size.width * size.height * 4; 
    unsigned char* transparentData = new unsigned char[dataSize];
    memset(transparentData, 0, dataSize);
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, size.width, size.height, size);
    transparent_texture = transparentTexture;

    // �ͷ��ڴ�
    delete[] transparentData;
}

// ����ʵ��
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

// ͼƬ����
void animals::set_imag()
{
    if (produce && transparent_texture) {
        this->setSpriteFrame(animals_name + "-front.png");
        produce->initWithTexture(transparent_texture);
        produce->autorelease();
        produce->setPosition(produce_pos);
    }
}

// ��ʼ����������
void animals::init_mouselistener()
{
    auto listener = cocos2d::EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(animals::on_mouse_click, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// ��갴��ʱ�Ļص�
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

// ι��
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

// ���ɸ���Ʒ
void animals::create_produce()
{
    if (feed_count % produce_day == 0 && feed_count)
    {
        produce->setSpriteFrame(animals_name + "-produce.png");//��ʾ������
        is_produce = 1;
        CCLOG("create produce");
    }
}

// �ջ���
void animals::harvest()
{
    //����������뱳��,����þ���ֵ
    backpackLayer->addItem(PRODUCE_MAP.at(animals_name));
    Player* player = Player::getInstance("me");
    player->playerproperty.addExperience(EXPERIENCE);
    produce->setTexture(transparent_texture);
    is_produce = 0;
}


// �ε�
void animals::randmove(cocos2d::TMXTiledMap* tileMap)
{
    unsigned int timestamp = static_cast<unsigned int>(time(0)) * 1000 + static_cast<unsigned int>(clock()) / (CLOCKS_PER_SEC / 1000);

    // ��ȡ����ID�����̼�Ĳ��죩
    unsigned int pid = static_cast<unsigned int>(getpid());

    // ��ȡ�߳�ID������ж��̣߳�
    unsigned int tid = static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    // ���ʱ���������ID���߳�ID������һ�����ӵ�����
    unsigned int seed = timestamp ^ pid ^ tid;

    srand(seed + ID);
    dic = (rand() % rand() + ID) % 4;
    movement[dic] = 1;

    // ��ʱ���� move_act��ÿ�����һ��
    this->schedule([this, tileMap](float) {
        move_act(tileMap);
        }, 0.1f, "move_act_key");  

    // 2���ִ�лص�����һ��
    this->scheduleOnce([this](float dt) {
        movement[dic] = 0;
        }, 2.0f, "one_time_schedule");

}

// �ƶ�����
void animals::move_act(cocos2d::TMXTiledMap* tileMap)
{
    for (int i = 0; i < 4; i++) {
        is_hit_edge[i] = false;
    }

    //��ȡ�����λ��
    auto sprite_pos = this->getPosition();
    cocos2d::Size spriteSize = this->getContentSize();
    cocos2d::Size mapSize = tileMap->getMapSize();
    cocos2d::Size tileSize = tileMap->getTileSize();
    mapSize.width *= tileSize.width;
    mapSize.height *= tileSize.height;
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // �жϾ����Ƿ񳬳��߽�
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
            //�����ƶ�����
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

// ����ʱ�ε�
void animals::scheduleRandomMove(cocos2d::TMXTiledMap* tileMap) {

    // ÿ5������ƶ�һ��
    this->schedule([this, tileMap](float dt) {
        randmove(tileMap);
        }, 5.0f, "random_move_key");

}

// ��һ��ĸ���
void animals::update_day(float deltaTime)
{
    if (timeSystem->getDay() != now_day)//���������
    {
        if (animals_name != "") {
            if (feed_today == 0)//˵������ι�������ﵽҪ��
            {
                feed_count++;
                //�鿴�Ƿ���Ҫ���³ɳ�״̬
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
// ���Ӿ��鵽����
void AnimalsManager::add_animals(animals* sprite) {
    animals_list.push_back(sprite);
}

void AnimalsManager::schedule_animals()
{
    // �������������ʾ���
    for (auto it = animals_list.begin(); it != animals_list.end(); ++it) {
        auto animal = *it;                      // ��ȡָ�룬ָ����
        animal->schedule([animal](float dt) {   // ����ָ�� animal
            animal->update(dt); 
            }, "update_animal");
    }
}