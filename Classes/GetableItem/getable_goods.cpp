#include "getable_goods.h"
#include "Data/GameData.h"


cocos2d::Texture2D* getable_goods::transparent_texture = nullptr;

void getable_goods::set_info(std::string name, Size size)
{
    // 不再存字符串，而是从GameData中获取享元对象
    //sprite_name = name;
    _model = GameData::getInstance()->getItemModel(name);
    sprite_size = size;
}

getable_goods* getable_goods::create(const std::string& plist_name)
{
    //加载 plist
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //实例化
    getable_goods* sprite = new getable_goods();

    //创建透明纹理内存 (RGBA8888)
    int dataSize = DEFAULT_WIDTH * DEFAULT_HEIGHT * 4;  // ÿ 4 ֽڣRGBA ʽ
    unsigned char* transparentData = new unsigned char[dataSize];

    memset(transparentData, 0, dataSize);

    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, DEFAULT_WIDTH, DEFAULT_HEIGHT, cocos2d::Size(DEFAULT_HEIGHT, DEFAULT_HEIGHT));
    transparent_texture = transparentTexture;

    delete[] transparentData;

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
    // 从_model获得ID
    std::string name = _model->id;
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(name + ".png");
    //cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name + ".png");
    this->initWithSpriteFrame(frame);
    is_getable = 1;
}

void getable_goods::init_mouselistener()
{
    auto listener = cocos2d::EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(getable_goods::on_mouse_click, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void getable_goods::on_mouse_click(cocos2d::Event* event)
{
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
    // 关键修复：所有场景都需要检查is_in_control（玩家必须在控制范围内）
    // 但是非农场场景的is_in_control检查可以放宽（如果不在控制范围内，也可以点击，但需要工具）
    bool canCheckClick = false;
    if (!is_infarm) {
        // 非农场场景：如果is_in_control为1，直接可以点击；如果为0，需要检查工具
        // 这里先允许点击，然后在工具检查中判断
        canCheckClick = true;
    } else {
        // 农场场景：必须is_in_control为1才能点击
        canCheckClick = is_in_control;
    }
    
    if (is_getable && canCheckClick) {
        //λЧΧ
        if ((mouse_pos.x > min_x &&
            mouse_pos.x < max_x &&
            mouse_pos.y > min_y &&
            mouse_pos.y < max_y))
        {
            CCLOG("good click");
            //CCLOG("%s", sprite_name.c_str());
            ///CCLOG("%s", _model->getID().c_str());
            
            // 关键修复：所有场景都需要检查工具匹配（除了明确不需要工具的物品）
            // 安全获取BackpackLayer实例
            auto* safeBackpackLayer = BackpackLayer::getInstance();
            if (!safeBackpackLayer) {
                CCLOG("BackpackLayer is null, cannot check tool");
                return;
            }
            
            //std::string requiredTool = GOODS_MAP.at(sprite_name).at("tool");
            std::string requiredTool = _model->requiredTool;
            bool canInteract = false;
            
            // 如果不需要工具（空字符串），直接允许交互
            if (requiredTool == "") {
                canInteract = true;
                click_count += 1;
            } else {
                // 需要工具：检查选中的工具是否匹配
                std::string selectedItem = safeBackpackLayer->getSelectedItem();
                
                // 精确匹配：检查选中的工具是否以所需工具名称开头
                // 例如：requiredTool = "Hoe", selectedItem = "Hoe1" 或 "Hoe2" 都匹配
                // 使用 find() == 0 确保是前缀匹配，而不是包含匹配
                if (!selectedItem.empty() && selectedItem.find(requiredTool) == 0) {
                    canInteract = true;
                    // 根据工具等级计算点击次数
                    char last_char = selectedItem[selectedItem.size() - 1];
                    int level = last_char - '0';
                    if (level >= 1 && level <= 9) {
                        click_count += level;
                    } else {
                        click_count += 1; // 默认等级为1
                    }
                    CCLOG("Tool matched: required=%s, selected=%s, click_count=%d", requiredTool.c_str(), selectedItem.c_str(), click_count);
                } else {
                    CCLOG("Wrong tool: required=%s, selected=%s", requiredTool.c_str(), selectedItem.c_str());
                    canInteract = false;  // 明确设置为false
                }
            }
            
            if (canInteract) {
                CCLOG("!!!!click_count:%d", click_count);
                this->show_click_bar();
                this->update();
            }
        }
    }
}

void getable_goods::show_click_bar()
{

    //ʾ
    if (!click_bar) {
        // δ򴴽ʾ
        click_bar = progress_bar::create();
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
        this->getParent()->addChild(click_bar, 3);
    }
    else {
        click_bar->show_progress_bar(cocos2d::Vec2(this->getPositionX(), this->getPositionY() + this->getContentSize().height / 2 + 5));
    }

    //从享元获取最大点击数
    //float progressValue = (float)click_count / (float)GOODS_CLICK_MAP.at(sprite_name) * 100.0f;
    //float progressValue = (float)click_count / (float)_model->getMaxClicks() * 100.0f;
    float progressValue = (float)click_count / (float)_model->maxClicks * 100.0f;

    click_bar->update_progress_bar(progressValue);

    // ڵ 3 ؽ
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
    //更新为享元模式
    //if (click_count >= GOODS_CLICK_MAP.at(sprite_name))
    //{
    //    this->hide_click_bar();
    //    //뱳 
    //    // 安全获取BackpackLayer实例，避免访问已销毁的对象
    //    auto* safeBackpackLayer = BackpackLayer::getInstance();
    //    if (safeBackpackLayer) {
    //        safeBackpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
    //        if (sprite_name == "bigstone")
    //            safeBackpackLayer->addItem(GOODS_MAP.at(sprite_name).at("get"));
    //    } else {
    //        CCLOG("Warning: BackpackLayer instance is null in getable_goods::update()");
    //    }
    //    //ﾭ
    //    Player* player = Player::getInstance("me");
    //    player->playerproperty.addExperience(EXPERIENCE * GOODS_CLICK_MAP.at(sprite_name));
    //    if (sprite_name == "badGreenhouse") {
    //        this->setSpriteFrame("newGreenhouse.png");
    //        is_getable = 0;
    //    }
    //    else {
    //        this->setTexture(transparent_texture);//Ϊ͸
    //        click_count = 0;//
    //        is_getable = 0;
    //    }
    //}

    if (!_model) return;

        if (click_count >= _model->maxClicks)
    {
        this->hide_click_bar();
        auto* safeBackpackLayer = BackpackLayer::getInstance();
        if (safeBackpackLayer) {

            // 获取掉落物
            safeBackpackLayer->addItem(_model->dropItem);

            // 特殊逻辑：bigstone 掉两份
            if (_model->id == "bigstone")
                safeBackpackLayer->addItem(_model->dropItem);
        }

        Player* player = Player::getInstance("me");
        // 经验值逻辑
        player->playerproperty.addExperience(EXPERIENCE * _model->maxClicks);

        if (_model->id == "badGreenhouse") {
            this->setSpriteFrame("newGreenhouse.png");
            is_getable = 0;
        }
        else {
            this->setTexture(transparent_texture); // 变透明
            click_count = 0;
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

    // ͸ľ
    sprite->set_info(name, Size(width * MapSize, height * MapSize));
    sprite->setPosition(Vec2(posX, posY));        //λ
    sprite->setAnchorPoint(Vec2(0, 0));     //ê
    sprite->setContentSize(Size(width, height));  // ôС
    tileMap->addChild(sprite, 2);  // ӵƬͼ
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
// Ӿ鵽
void GoodsManager::add_goods(getable_goods* sprite) {
    goods.push_back(sprite);
}

void GoodsManager::get_getable_count()
{
    getable_count = 0;
    // ʾ
    for (auto it = goods.begin(); it != goods.end(); ++it) {
        getable_count = (*it)->get_is_getable();
    }
}

//һ飨ͨ
void GoodsManager::random_access()
{
    get_getable_count();
    //ǰɻȡƷ٣
    if (!goods.empty() && getable_count <= goods.size() * PERCENT)
    {
        auto it = goods.begin();  // ȡĿʼ

        std::advance(it, rand() % goods.size());  // תĳλ
        if ((*it)->get_is_getable() == 0) {
            (*it)->setImag();
        }

    }
}

void GoodsManager::start_scheduler() {

    // ʹöʱÿ 60 һ randomAccess
    this->schedule([this](float delta) {
        this->random_access();
        }, 6.0f, "RandomAccessSchedulerKey");
}

void GoodsManager::stop_scheduler() {
    // ֹͣʱ
    this->unschedule("RandomAccessSchedulerKey");
}
