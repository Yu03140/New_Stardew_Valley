#include "moveable_sprite_key.h"

// 静态成员变量初始化
std::string moveable_sprite_key::sprite_name = "";
std::string moveable_sprite_key_walk::sprite_name_walk = "";
std::string moveable_sprite_key_tool::sprite_name_tool = "";
cocos2d::Texture2D* moveable_sprite_key_tool::transparent_texture = nullptr;
cocos2d::Texture2D* moveable_sprite_key::transparent_texture = nullptr;

std::unordered_set<std::string> TOOLS_MAP =
{ "Axe1", "Rod1", "Hoe1", "Pick1", "Can1" ,"Axe2", "Rod2", "Hoe2", "Pick2", "Can2" };


// 创建一个moveable_sprite_key的实例
moveable_sprite_key* moveable_sprite_key::create(const std::string& plist_name, float width, float height)
{
    // 加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    // 创建实例
    moveable_sprite_key* sprite = new moveable_sprite_key();

    // 创建透明纹理内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = width * height * 4;  // 每像素占 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值均为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, width, height, cocos2d::Size(width, height));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;
    // 判断是否能成功创建
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

// 键盘监听器
void moveable_sprite_key::init_keyboardlistener()
{
    // 先移除旧的监听器，避免重复注册
    _eventDispatcher->removeEventListenersForTarget(this);
    // 创建键盘监听器
    auto listener = cocos2d::EventListenerKeyboard::create();

    // 键盘按下时的回调
    listener->onKeyPressed = CC_CALLBACK_2(moveable_sprite_key::onKeyPressed, this);

    // 键盘释放时的回调
    listener->onKeyReleased = CC_CALLBACK_2(moveable_sprite_key::onKeyReleased, this);

    // 获取事件分发器并添加监听器
    // 关键修复：使用固定优先级1，确保玩家精灵的键盘监听器优先于场景的监听器（优先级-1）
    // 重要：需要保留监听器，防止被自动释放
    listener->retain();
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    CCLOG("moveable_sprite_key::init_keyboardlistener() - new listener registered with priority 1, listener retained");

}

// 按下键盘时，将对应的运动修改为true
void moveable_sprite_key::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    CCLOG("moveable_sprite_key::onKeyPressed() called with keyCode: %d", static_cast<int>(keyCode));
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

// 松开按键后，将对应的运动修改回false
void moveable_sprite_key::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    CCLOG("moveable_sprite_key::onKeyReleased() called with keyCode: %d", static_cast<int>(keyCode));
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        movement[0] = false;
        CCLOG("UP arrow released, movement[0] = false");
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

    // 松开按键时，停止所有动作
    this->stopAllActions();
    isAnimating = false;

}

// 更新位置
void moveable_sprite_key::update(float deltaTime)
{
    // 检测是否到达边界进行判断结果,分别为上下左右
    bool is_hit_edge[4] = { false,false, false, false };

    // 获取精灵位置
    sprite_pos = this->getPosition();

    // 获取窗口的大小信息
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    // 计算地图边界（地图中心在屏幕中心，锚点为0.5,0.5）
    float mapCenterX = visibleSize.width / 2 + origin.x;
    float mapCenterY = visibleSize.height / 2 + origin.y;
    float mapMinX = mapCenterX - SceneWidth / 2;
    float mapMaxX = mapCenterX + SceneWidth / 2;
    float mapMinY = mapCenterY - SceneHeight / 2;
    float mapMaxY = mapCenterY + SceneHeight / 2;
    
    // 获取精灵的尺寸（考虑缩放）
    float spriteHalfWidth = this->getContentSize().width * MapSize / 2;
    float spriteHalfHeight = this->getContentSize().height * MapSize / 2;
    
    // 检查边界（考虑精灵的尺寸）
    if (sprite_pos.y + spriteHalfHeight >= mapMaxY) {
        is_hit_edge[0] = true; // 上边界
        CCLOG("Sprite hit the top edge");
    }
    else if (sprite_pos.y - spriteHalfHeight <= mapMinY) {
        is_hit_edge[1] = true; // 下边界
        CCLOG("Sprite hit the bottom edge");
    }
    if (sprite_pos.x - spriteHalfWidth <= mapMinX) {
        is_hit_edge[2] = true; // 左边界
        CCLOG("Sprite hit the left edge");
    }
    else if (sprite_pos.x + spriteHalfWidth >= mapMaxX) {
        is_hit_edge[3] = true; // 右边界
        CCLOG("Sprite hit the right edge");
    }

    for (int i = 0; i < 4; i++) {
        if (movement[i] && !is_hit_edge[i] && is_passable)
            move_act(i);
    }

    // 更新精灵位置
    sprite_pos = this->getPosition();
}

// 具体移动指令
void moveable_sprite_key::move_act(int direction)
{
    // 构建对应图片后缀
    std::string dic[4] = { "-back","-front","-left","-right" };
    this->setSpriteFrame(sprite_name + dic[direction] + ".png");
    // 执行移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

// 确保在 onEnter 中添加监听器
void moveable_sprite_key::onEnter() {
    Sprite::onEnter();  // 确保调用 onEnter 方法
    
    // 先移除旧的监听器，避免重复注册
    //_eventDispatcher->removeEventListenersForTarget(this);
    
    // 重新初始化监听器
    init_keyboardlistener();
    CCLOG("moveable_sprite_key::onEnter() - keyboard listener initialized");
}

// 确保在 onExit 中移除监听器
void moveable_sprite_key::onExit() {
    _eventDispatcher->removeEventListenersForTarget(this);  // 移除监听器
    Sprite::onExit();  // 确保调用 onExit 方法
}


// 创建moveable_sprite_key_walk实例
moveable_sprite_key_walk* moveable_sprite_key_walk::create(const std::string& plist_name, const std::string& sprite_framename)

{
    // 加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    // 同步保存精灵名字
    sprite_name_walk = sprite_framename;

    // 创建实例
    std::string default_framename = sprite_name_walk + "-front.png";
    moveable_sprite_key_walk* sprite = new moveable_sprite_key_walk();

    // 获取指定精灵帧
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(default_framename);

    // 判断是否能成功创建
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

// 根据产生鼠标移动调整的移动指令
void moveable_sprite_key_walk::move_act(int direction)
{
    // 构建对应图片后缀
    std::string dic[4] = { "-back","-front","-left","-right" };

    // 执行移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    // 添加帧动画
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + ".png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "2.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "3.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "4.png"));
    // 创建帧动画序列
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f); // 每帧持续 0.1s
    auto animate = cocos2d::Animate::create(animation);
    auto repeat = cocos2d::RepeatForever::create(animate);

    this->runAction(move_action);
    // 如果用户没有在播放动画才开始播放
    if (!isAnimating) {
        // 执行移动和动画
        this->runAction(repeat);
        isAnimating = true;  // 标记动画正在播放
    }

    character_pos = this->getPosition();
    //CCLOG("charactor position: (%f, %f)", character_pos.x, character_pos.y);
}

// 移动到目标位置
void moveable_sprite_key_walk::moveToPosition(const cocos2d::Vec2& targetPosition) {
    this->targetPosition = targetPosition;
    this->hasTarget = true;
    CCLOG("Set target position: (%f, %f)", targetPosition.x, targetPosition.y);
}

// 重写update方法，添加自动移动到目标位置的逻辑
void moveable_sprite_key_walk::update(float deltaTime) {
    // 先调用基类的update
    moveable_sprite_key::update(deltaTime);
    
    // 更新character_pos全局变量（确保所有场景都能正确更新） 
    character_pos = this->getPosition();
    
    // 如果有目标位置，自动移动到目标
    if (hasTarget) {
        Vec2 currentPos = this->getPosition();
        Vec2 diff = targetPosition - currentPos;
        float distance = diff.length();
        
        // 如果距离小于阈值，认为已到达目标
        const float ARRIVAL_THRESHOLD = 10.0f;
        if (distance < ARRIVAL_THRESHOLD) {
            hasTarget = false;
            // 停止所有移动
            for (int i = 0; i < 4; i++) {
                movement[i] = false;
            }
            this->stopAllActions();
            isAnimating = false;
            CCLOG("Reached target position");
            return;
        }
        
        // 计算移动方向（优先考虑主要方向）
        float absX = std::abs(diff.x);
        float absY = std::abs(diff.y);
        
        // 清除所有移动标志
        for (int i = 0; i < 4; i++) {
            movement[i] = false;
        }
        
        // 根据距离目标的方向设置移动标志
        // 方向：0=上, 1=下, 2=左, 3=右
        if (absY > absX) {
            // 垂直方向优先
            if (diff.y > 0) {
                movement[0] = true; // 上
            } else {
                movement[1] = true; // 下
            }
        } else {
            // 水平方向优先
            if (diff.x < 0) {
                movement[2] = true; // 左
            } else {
                movement[3] = true; // 右
            }
        }
    }
}

cocos2d::Vec2 moveable_sprite_key_walk::get_pos()
{
    auto pos = this->getPosition();
    CCLOG("pos at: (%.2f, %.2f)", pos.x, pos.y);
    return pos;
}

moveable_sprite_key_tool* moveable_sprite_key_tool::create(const std::string& plist_name)
{
    // 加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    // 创建实例
    moveable_sprite_key_tool* sprite = new moveable_sprite_key_tool();

    // 创建透明纹理内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = TOOL_HEIGHT * TOOL_WIDTH * 4;  // 每像素占 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值均为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, TOOL_WIDTH, TOOL_HEIGHT, cocos2d::Size(TOOL_WIDTH, TOOL_HEIGHT));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;

    // 判断是否能成功创建
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

    // 如果当前手中的工具与背包选中的不一致，更新纹理
    // 关键修复：检查 backpackLayer 是否有效，防止访问已销毁的对象
    if (!backpackLayer) {
        return;
    }

    std::string selectedItem = backpackLayer->getSelectedItem();
    if (selectedItem != sprite_name_tool) {
        sprite_name_tool = selectedItem;
        // 如果该物品为工具，需要变方向纹理
        if (sprite_name_tool != "") {
            if (TOOLS_MAP.count(selectedItem)) {
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

// 具体移动指令
void moveable_sprite_key_tool::move_act(int direction)
{
    std::string dic[4] = { "-back","-front","-left","-right" };
    if (sprite_name_tool != "") {
        if (TOOLS_MAP.count(backpackLayer->getSelectedItem())) {
            // 构建对应图片后缀
            this->setSpriteFrame(sprite_name_tool + dic[direction] + ".png");
        }
    }
    direc = dic[direction];

    // 执行移动动作
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

//// 初始化鼠标监听器
//void moveable_sprite_key_tool::init_mouselistener()
//{
//    // 先移除旧的鼠标监听器，避免重复注册
//    _eventDispatcher->removeEventListenersForTarget(this);
//
//    // 创建鼠标监听器
//    auto listener = cocos2d::EventListenerMouse::create();
//
//    // 鼡标按下时的回调
//    listener->onMouseDown = CC_CALLBACK_1(moveable_sprite_key_tool::on_mouse_click, this);
//
//    // 获取事件分发器并添加监听器
//    // 关键修复：使用固定优先级2，确保工具精灵的监听器在场景监听器（优先级1）之后但在精灵监听器之前处理
//    _eventDispatcher->addEventListenerWithFixedPriority(listener, 2);
//}
//
//// 鼡标按下时的回调
//void moveable_sprite_key_tool::on_mouse_click(cocos2d::Event* event)
//{
//    
//    auto tool_pos = this->getPosition();
//    auto tool_size = this->getContentSize();
//    Vec2 mouse_pos;
//    if (is_infarm)
//    {
//        mouse_pos = MOUSE_POS;
//    }
//    else {
//        auto mouse_event = dynamic_cast<cocos2d::EventMouse*>(event);
//        mouse_pos = this->getParent()->convertToNodeSpace(mouse_event->getLocationInView());
//    }
//
//    if (mouse_pos.x > character_pos.x - CONTROL_RANGE &&
//        mouse_pos.x < character_pos.x + CONTROL_RANGE &&
//        mouse_pos.y > character_pos.y - CONTROL_RANGE &&
//        mouse_pos.y < character_pos.y + CONTROL_RANGE)
//    {
//        is_in_control = 1;
//        CCLOG("Tool: IN CONTROL, mouse_pos=(%f, %f), character_pos=(%f, %f)", 
//              mouse_pos.x, mouse_pos.y, character_pos.x, character_pos.y);
//        if (TOOLS_MAP.count(sprite_name_tool)) {
//            CCLOG("tool click!");
//            // 切换纹理
//            this->setSpriteFrame(sprite_name_tool + direc + "-clicked.png");
//
//            // 在 0.2 秒后恢复原图
//            this->scheduleOnce([this](float dt) {
//                if (sprite_name_tool != "")
//                    this->setSpriteFrame(sprite_name_tool + direc + ".png");
//                }, 0.2f, "reset_texture_key");
//        }
//    }
//    else {
//        is_in_control = 0;
//        CCLOG("Tool: NOT IN CONTROL, mouse_pos=(%f, %f), character_pos=(%f, %f)", 
//              mouse_pos.x, mouse_pos.y, character_pos.x, character_pos.y);
//    }
//}

// 【观察者模式：播放点击动画】
void moveable_sprite_key_tool::playClickAnimation()
{
    // 不再进行坐标或 is_in_control 检查，因为 InteractionManager 已经完成了验证。
    // 我们只需要检查当前是否有工具被选中。
    if (TOOLS_MAP.count(sprite_name_tool)) {
        CCLOG("Tool click effect triggered by InteractionManager!");

        // 切换图片到点击状态
        this->setSpriteFrame(sprite_name_tool + direc + "-clicked.png");

        // 在 0.2 秒后恢复原图
        this->scheduleOnce([this](float dt) {
            if (sprite_name_tool != "")
                this->setSpriteFrame(sprite_name_tool + direc + ".png");
            }, 0.2f, "reset_texture_key");
    }
    else {
        CCLOG("Tool click received but no active tool selected.");
    }
}