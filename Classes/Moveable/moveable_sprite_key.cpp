#include "moveable_sprite_key.h"

// ��̬��Ա��������
std::string moveable_sprite_key::sprite_name = "";
std::string moveable_sprite_key_walk::sprite_name_walk = "";
std::string moveable_sprite_key_tool::sprite_name_tool = "";
cocos2d::Texture2D* moveable_sprite_key_tool::transparent_texture = nullptr;
cocos2d::Texture2D* moveable_sprite_key::transparent_texture = nullptr;

std::unordered_set<std::string> TOOLS_MAP =
{ "Axe1", "Rod1", "Hoe1", "Pick1", "Can1" ,"Axe2", "Rod2", "Hoe2", "Pick2", "Can2" };


//����һ��moveable_sprite_key��ʵ��
moveable_sprite_key* moveable_sprite_key::create(const std::string& plist_name, float width, float height)
{
    //����plist�ļ�
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //����ʵ��
    moveable_sprite_key* sprite = new moveable_sprite_key();

    // ����͸�����ڴ�飬����Ϊȫ͸�� (RGBA8888 ��ʽ)
    int dataSize = width * height * 4;  // ÿ������ 4 �ֽڣ�RGBA ��ʽ��
    unsigned char* transparentData = new unsigned char[dataSize];

    // ���͸������ (ÿ�����ص� 4 ��ͨ��ֵ��Ϊ 0)
    memset(transparentData, 0, dataSize);

    // ����͸������
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, width, height, cocos2d::Size(width, height));
    transparent_texture = transparentTexture;

    // �ͷ��ڴ�
    delete[] transparentData;
    //�ж��Ƿ��ܳɹ�����
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

//���̼�����
void moveable_sprite_key::init_keyboardlistener()
{
    // 先移除旧的监听器，避免重复注册
    _eventDispatcher->removeEventListenersForTarget(this);
    // �������̼�����
    auto listener = cocos2d::EventListenerKeyboard::create();

    // ��������ʱ�Ļص�
    listener->onKeyPressed = CC_CALLBACK_2(moveable_sprite_key::onKeyPressed, this);

    // �����ͷ�ʱ�Ļص�
    listener->onKeyReleased = CC_CALLBACK_2(moveable_sprite_key::onKeyReleased, this);

    // ��ȡ�¼��ַ��������Ӽ�����
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

}

//���¼���ʱ������Ӧ��������޸�Ϊtrue
void moveable_sprite_key::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW)
    {
        movement[0] = true; // ��
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW)
    {
        movement[1] = true; // ��
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
    {
        movement[2] = true; // ��
    }
    else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
    {
        movement[3] = true; // ��
    }
}

//�ɿ����̺󣬽���Ӧ��������޸Ļ�false
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

    // �ɿ�����ʱ��ֹͣ���ж���
    this->stopAllActions();
    isAnimating = false;

}

//����λ��
void moveable_sprite_key::update(float deltaTime)
{
    //�����Ƿ񵽴�߽���жϽ��,�ֱ�Ϊ��������
    bool is_hit_edge[4] = { false,false, false, false };

    //��ȡ�����λ��
    sprite_pos = this->getPosition();

    //��ȡ���ڵĴ�С��Ϣ
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

    //��ȡ�����λ��
    sprite_pos = this->getPosition();
}

//�����ƶ�ָ��
void moveable_sprite_key::move_act(int direction)
{
    //�������ӦͼƬ��׺
    std::string dic[4] = { "-back","-front","-left","-right" };
    this->setSpriteFrame(sprite_name + dic[direction] + ".png");
    //�����ƶ�����
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

// ȷ���� onEnter ���������Ӽ�����
void moveable_sprite_key::onEnter() {
    Sprite::onEnter();  // ��֤����� onEnter ������
    
    // ���Ƴ��ɵļ������������ظ�ע��
    //_eventDispatcher->removeEventListenersForTarget(this);
    
    // ���³�ʼ��������
    init_keyboardlistener();
}

// ȷ���� onExit ���Ƴ�������
void moveable_sprite_key::onExit() {
    _eventDispatcher->removeEventListenersForTarget(this);  // �Ƴ�������
    Sprite::onExit();  // ��֤����� onExit ������
}


//����moveable_sprite_key_walkʵ��
moveable_sprite_key_walk* moveable_sprite_key_walk::create(const std::string& plist_name, const std::string& sprite_framename)

{
    //����plist�ļ�
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //ͬ�������ͼ������
    sprite_name_walk = sprite_framename;

    //����ʵ��
    std::string default_framename = sprite_name_walk + "-front.png";
    moveable_sprite_key_walk* sprite = new moveable_sprite_key_walk();

    // ��ȡָ������֡
    cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(default_framename);

    //�ж��Ƿ��ܳɹ�����
    if (frame)
    {
        sprite->initWithSpriteFrame(frame);
        sprite->autorelease();
        sprite->setScale(6.0f);  // ������Ŵ� 6 ��
        sprite->init_keyboardlistener();
        CCLOG("Creation moveable_sprite_key_walk successfully!");
        return sprite;
    }
    CCLOG("Creation moveable_sprite_key_walk unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

//���ɴ����ƶ��������ƶ�ָ��
void moveable_sprite_key_walk::move_act(int direction)
{
    //�������ӦͼƬ��׺
    std::string dic[4] = { "-back","-front","-left","-right" };

    //�����ƶ�����
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    //��������
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + ".png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "2.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "3.png"));
    frames.pushBack(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(sprite_name_walk + dic[direction] + "4.png"));
    // ������������
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.1f); // ÿ֡��� 0.1s
    auto animate = cocos2d::Animate::create(animation);
    auto repeat = cocos2d::RepeatForever::create(animate);

    this->runAction(move_action);
    // �����û���ڲ��Ŷ������ſ�ʼ����
    if (!isAnimating) {
        // ִ���ƶ��Ͷ���
        this->runAction(repeat);
        isAnimating = true;  // ��Ƕ������ڲ���
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
    //����plist�ļ�
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);

    //����ʵ��
    moveable_sprite_key_tool* sprite = new moveable_sprite_key_tool();

    // ����͸�����ڴ�飬����Ϊȫ͸�� (RGBA8888 ��ʽ)
    int dataSize = TOOL_HEIGHT * TOOL_WIDTH * 4;  // ÿ������ 4 �ֽڣ�RGBA ��ʽ��
    unsigned char* transparentData = new unsigned char[dataSize];

    // ���͸������ (ÿ�����ص� 4 ��ͨ��ֵ��Ϊ 0)
    memset(transparentData, 0, dataSize);

    // ����͸������
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, TOOL_WIDTH, TOOL_HEIGHT, cocos2d::Size(TOOL_WIDTH, TOOL_HEIGHT));
    transparent_texture = transparentTexture;

    // �ͷ��ڴ�
    delete[] transparentData;

    //�ж��Ƿ��ܳɹ�����
    if (transparentTexture)
    {
        sprite->initWithTexture(transparentTexture);
        sprite->autorelease();
        sprite->setScale(3.0f);  // ������Ŵ� 3 ��
        sprite->init_keyboardlistener();
        CCLOG("Creation moveable_sprite_key_tool successfully!");
        return sprite;
    }
    CCLOG("Creation moveable_sprite_key_tool unsuccessfully!");
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

void moveable_sprite_key_tool::update(float deltaTime) {

    // �ȵ��ø���� update
    moveable_sprite_key::update(deltaTime);

    //����������ڵĹ����뱳��ѡ�еĲ�һ�£��������
    if (backpackLayer->getSelectedItem() != sprite_name_tool) {
        sprite_name_tool = backpackLayer->getSelectedItem();
        //�������ƷΪ���ߣ�����Ҫ�淽�����
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

//�����ƶ�ָ��
void moveable_sprite_key_tool::move_act(int direction)
{
    std::string dic[4] = { "-back","-front","-left","-right" };
    if (sprite_name_tool != "") {
        if (TOOLS_MAP.count(backpackLayer->getSelectedItem())) {
            //�������ӦͼƬ��׺
            this->setSpriteFrame(sprite_name_tool + dic[direction] + ".png");
        }
    }
    direc = dic[direction];

    //�����ƶ�����
    auto move_action = cocos2d::MoveBy::create(0.1f, cocos2d::Vec2(move_vecx[direction], move_vecy[direction]));
    this->runAction(move_action);
}

// ��ʼ����������
void moveable_sprite_key_tool::init_mouselistener()
{

    //// 先移除旧的鼠标监听器，避免重复注册
    //auto listeners = _eventDispatcher->getListeners(this);
    //for (auto& listener : listeners) {
    //    if (listener->getType() == cocos2d::EventListener::Type::MOUSE) {
    //        _eventDispatcher->removeEventListener(listener);
    //    }
    //}

    // ������������
    auto listener = cocos2d::EventListenerMouse::create();

    // ��갴��ʱ�Ļص�
    listener->onMouseDown = CC_CALLBACK_1(moveable_sprite_key_tool::on_mouse_click, this);

    // ��ȡ�¼��ַ��������Ӽ�����
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// ��갴��ʱ�Ļص�
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
            // �л�����
            this->setSpriteFrame(sprite_name_tool + direc + "-clicked.png");

            // �� 0.2 ���ָ�ԭͼ
            this->scheduleOnce([this](float dt) {
                if (sprite_name_tool != "")
                    this->setSpriteFrame(sprite_name_tool + direc + ".png");
                }, 0.2f, "reset_texture_key");
        }
    }
    else
        is_in_control = 0;

}
