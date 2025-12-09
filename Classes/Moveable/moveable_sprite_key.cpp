#include "moveable_sprite_key.h"

// 静态成员变量定义
std::string moveable_sprite_key::sprite_name = "";
std::string moveable_sprite_key_walk::sprite_name_walk = "";
std::string moveable_sprite_key_tool::sprite_name_tool = "";
cocos2d::Texture2D* moveable_sprite_key_tool::transparent_texture = nullptr;
cocos2d::Texture2D* moveable_sprite_key::transparent_texture = nullptr;

std::unordered_set<std::string> TOOLS_MAP =
{ "Axe1", "Rod1", "Hoe1", "Pick1", "Can1" ,"Axe2", "Rod2", "Hoe2", "Pick2", "Can2" };


//创建一个moveable_sprite_key的实例
moveable_sprite_key* moveable_sprite_key::create(const std::string& plist_name, float width, float height)
{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //创建实例
    moveable_sprite_key* sprite = new moveable_sprite_key();

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
        sprite->initWithTexture(transparentTexture);
        sprite->autorelease();
        sprite->init_keyboardlistener();
        CCLOG("Creation moveable_sprite_key successfully!");
        return sprite;
    }
    CCLOG("Creation moveable_sprite_key unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

//键盘监听器
void moveable_sprite_key::init_keyboardlistener()
{
    // 创建键盘监听器
    auto listener = cocos2d::EventListenerKeyboard::create();

    // 按键按下时的回调
    listener->onKeyPressed = CC_CALLBACK_2(moveable_sprite_key::onKeyPressed, this);

    // 按键释放时的回调
    listener->onKeyReleased = CC_CALLBACK_2(moveable_sprite_key::onKeyReleased, this);

    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

//按下键盘时，将对应方向参数修改为true
void moveable_sprite_key::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        movement[0] = true; // 上
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
        movement[1] = true; // 下
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
        movement[2] = true; // 左
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
        movement[3] = true; // 右
    }
}

//松开键盘后，将对应方向参数修改会false
void moveable_sprite_key::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        movement[0] = false;
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
        movement[1] = false;
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
        movement[2] = false;
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
        movement[3] = false;
    }

    // 松开键盘时，停止所有动作
    this->stopAllActions();
    isAnimating = false;

}

//更新位置
void moveable_sprite_key::update(float deltaTime)
{
    //保存是否到达边界的判断结果,分别为上下左右
    bool is_hit_edge[4] = { false,false, false, false };

    //获取精灵的位置
    sprite_pos = this->getPosition();

    //获取窗口的大小信息
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    if (sprite_pos.y + this->getContentSize().height * MapSize >= SceneHeight / 2 + visibleSize.height / 2) {
        is_hit_edge[0] = true;
        CCLOG("Sprite hit the top edge");
    }
    else  if (sprite_pos.y - this->getContentSize().height * MapSize <= visibleSize.height / 2 - SceneHeight / 2) {
        is_hit_edge[1] = true;
        CCLOG("Sprite hit the bottom edge");
    }
    if (sprite_pos.x - this->getContentSize().width * MapSize <= visibleSize.width / 2 - SceneWidth / 2) {
        is_hit_edge[2] = true;
        CCLOG("Sprite hit the left edge");
    }
    else if (sprite_pos.x + this->getContentSize().width * MapSize >= SceneWidth / 2 + visibleSize.width / 2) {
        is_hit_edge[3] = true;
        CCLOG("Sprite hit the right edge");
    }

    for (int i = 0; i < 4; i++) {
        if (movement[i] && !is_hit_edge[i] && is_passable)
            move_act(i);
    }

    //获取精灵的位置
    sprite_pos = this->getPosition();
}

//生成移动指令
void moveable_sprite_key::move_act(int direction)
{
    //各方向对应图片后缀
    std::string dic[4] = { "-back","-front","-left","-right" };
    this->setSpriteFrame(sprite_name + dic[direction] + ".png");
    //创建移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

// 确保在 onEnter 中重新添加监听器
void moveable_sprite_key::onEnter() {
    Sprite::onEnter();  // 保证基类的 onEnter 被调用
    init_keyboardlistener();  // 重新初始化监听器
}

// 确保在 onExit 中移除监听器
void moveable_sprite_key::onExit() {
    _eventDispatcher->removeEventListenersForTarget(this);  // 移除监听器
    Sprite::onExit();  // 保证基类的 onExit 被调用
}


//创建moveable_sprite_key_walk实例
moveable_sprite_key_walk* moveable_sprite_key_walk::create(const std::string& plist_name, const std::string& sprite_framename)

{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //同步精灵的图集名称
    sprite_name_walk = sprite_framename;

    //创建实例
    std::string default_framename = sprite_name_walk + "-front.png";
    moveable_sprite_key_walk* sprite = new moveable_sprite_key_walk();

    // 获取指定精灵帧
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(default_framename);

    //判断是否能成功创建
    if (frame)
    {
        sprite->initWithSpriteFrame(frame);
        sprite->autorelease();
        sprite->setScale(6.0f);  // 将精灵放大 6 倍
        sprite->init_keyboardlistener();
        CCLOG("Creation moveable_sprite_key_walk successfully!");
        return sprite;
    }
    CCLOG("Creation moveable_sprite_key_walk unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

//生成带有移动动画的移动指令
void moveable_sprite_key_walk::move_act(int direction)
{
    //各方向对应图片后缀
    std::string dic[4] = { "-back","-front","-left","-right" };

    //创建移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    //创建动画
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + ".png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "2.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "3.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "4.png"));
    // 创建动画对象
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f); // 每帧间隔 0.1s
    auto animate = cocos2d::Animate::create(animation);
    auto repeat = cocos2d::RepeatForever::create(animate);

    this->runAction(move_action);
    // 如果还没有在播放动画，才开始播放
    if (!isAnimating) {
        // 执行移动和动画
        this->runAction(repeat);
        isAnimating = true;  // 标记动画正在播放
    }

    character_pos = this->getPosition();
    //CCLOG("charactor position: (%f, %f)", character_pos.x, character_pos.y);
}

cocos2d::Vec2 moveable_sprite_key_walk::get_pos()
{
    auto pos = this->getPosition();
    CCLOG("pos at: (%.2f, %.2f)", pos.x, pos.y);
    return pos;
}

moveable_sprite_key_tool* moveable_sprite_key_tool::create(const std::string& plist_name)

{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //创建实例
    moveable_sprite_key_tool* sprite = new moveable_sprite_key_tool();

    // 创建透明的内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = TOOL_HEIGHT * TOOL_WIDTH * 4;  // 每个像素 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值都为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, TOOL_WIDTH, TOOL_HEIGHT, cocos2d::Size(TOOL_WIDTH, TOOL_HEIGHT));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;

    //判断是否能成功创建
    if (transparentTexture)
    {
        sprite->initWithTexture(transparentTexture);
        sprite->autorelease();
        sprite->setScale(3.0f);  // 将精灵放大 3 倍
        sprite->init_keyboardlistener();
        CCLOG("Creation moveable_sprite_key_tool successfully!");
        return sprite;
    }
    CCLOG("Creation moveable_sprite_key_tool unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void moveable_sprite_key_tool::update(float deltaTime) {

    // 先调用父类的 update
    moveable_sprite_key::update(deltaTime);

    //如果手上现在的工具与背包选中的不一致，则需更新
    if (backpackLayer->getSelectedItem() != sprite_name_tool) {
        sprite_name_tool = backpackLayer->getSelectedItem();
        //如果该物品为工具，则需要随方向调整
        if (sprite_name_tool != "") {
            if (TOOLS_MAP.count(backpackLayer->getSelectedItem())) {
                this->setSpriteFrame(sprite_name_tool + direc + ".png");
            }
            else {
                this->setSpriteFrame(sprite_name_tool + ".png");
            }
        }
        else
            this->setTexture(transparent_texture);
    }
}

//生成移动指令
void moveable_sprite_key_tool::move_act(int direction)
{
    std::string dic[4] = { "-back","-front","-left","-right" };
    if (sprite_name_tool != "") {
        if (TOOLS_MAP.count(backpackLayer->getSelectedItem())) {
            //各方向对应图片后缀
            this->setSpriteFrame(sprite_name_tool + dic[direction] + ".png");
        }
    }
    direc = dic[direction];

    //创建移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

// 初始化鼠标监听器
void moveable_sprite_key_tool::init_mouselistener()
{
    // 创建鼠标监听器
    auto listener = cocos2d::EventListenerMouse::create();

    // 鼠标按下时的回调
    listener->onMouseDown = CC_CALLBACK_1(moveable_sprite_key_tool::on_mouse_click, this);

    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 鼠标按下时的回调
void moveable_sprite_key_tool::on_mouse_click(cocos2d::Event* event)
{
    
    auto tool_pos = this->getPosition();
    auto tool_size = this->getContentSize();
    Vec2 mouse_pos;
    if (is_infarm)
    {
        mouse_pos = MOUSE_POS;
    }
    else {
        auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
        mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
    }

    if (mouse_pos.x > character_pos.x - CONTROL_RANGE &&
        mouse_pos.x < character_pos.x + CONTROL_RANGE &&
        mouse_pos.y > character_pos.y - CONTROL_RANGE &&
        mouse_pos.y < character_pos.y + CONTROL_RANGE)
    {
        is_in_control = 1;
        CCLOG("IN CONTROL");
        if (TOOLS_MAP.count(sprite_name_tool)) {
            CCLOG("tool click!");
            // 切换纹理
            this->setSpriteFrame(sprite_name_tool + direc + "-clicked.png");

            // 在 0.2 秒后恢复原图
            this->scheduleOnce([this](float dt) {
                if (sprite_name_tool != "")
                    this->setSpriteFrame(sprite_name_tool + direc + ".png");
                }, 0.2f, "reset_texture_key");
        }
    }
    else
        is_in_control = 0;

}
