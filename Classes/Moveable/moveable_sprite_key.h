#ifndef __MOVEABLE_SPRITE_KEY_H__
#define __MOVEABLE_SPRITE_KEY_H__

#include "cocos2d.h"
#include "Global/Global.h"
#include <unordered_set>

#define SPEED 35.0f
#define TOOL_WIDTH 40
#define TOOL_HEIGHT 60

class moveable_sprite_key : public cocos2d::Sprite
{
protected:
    const float speed = SPEED;//�ƶ��ٶ�
    //�����λ�÷ֱ��ʾ�ϡ��¡����ҷ�λ
    float move_vecx[4] = { 0,0,-speed * 0.1f,speed * 0.1f };
    float move_vecy[4] = { speed * 0.1f,-speed * 0.1f ,0,0 };
    bool movement[4] = { false, false, false, false };
    bool isAnimating = false;
private:
    static std::string sprite_name;
    static cocos2d::Texture2D* transparent_texture;
    bool is_passable = 1;
    cocos2d::Vec2 sprite_pos;
public:
    virtual ~moveable_sprite_key() { CCLOG("~moveable_sprite_key"); }

    //����һ��moveable_sprite_key��ʵ��
    static moveable_sprite_key* create(const std::string& plist_name, float width, float height);

    //���̼�����
    virtual void init_keyboardlistener();

    //���¼���ʱ������Ӧ��������޸�Ϊtrue
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    //�ɿ����̺󣬽���Ӧ��������޸Ļ�false
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    //����λ��
    void update(float deltaTime);

    //�����ƶ�ָ��
    virtual void move_act(int direction);

    // ȷ���� onEnter ���������Ӽ�����
    void onEnter();

    // ȷ���� onExit ���Ƴ�������
    void onExit();
};

class moveable_sprite_key_walk : public moveable_sprite_key
{
private:
    static std::string sprite_name_walk;
    static cocos2d::Texture2D* transparent_texture;
    cocos2d::Vec2 targetPosition;  // 目标位置
    bool hasTarget = false;  // 是否有目标位置

public:
    ~moveable_sprite_key_walk() { CCLOG("~moveable_sprite_key_walk"); }

    //����һ��moveable_sprite_key��ʵ��
    static moveable_sprite_key_walk* create(const std::string& plist_name, const std::string& sprite_framename);

    //���ɴ����ƶ��������ƶ�ָ��
    void move_act(int direction) override;

    cocos2d::Vec2 get_pos();
    
    // 移动到目标位置
    void moveToPosition(const cocos2d::Vec2& targetPosition);
    
    // 重写update方法，添加自动移动到目标位置的逻辑
    void update(float deltaTime) override;

    void moveable_sprite_key_walk::onEnter() {
        moveable_sprite_key::onEnter();  // ���û���� onEnter
        // �����������ʼ���߼�
    }

    void moveable_sprite_key_walk::onExit() {
        moveable_sprite_key::onExit();  // ���û���� onExit
        // ��������������߼�
    }
};

class moveable_sprite_key_tool : public moveable_sprite_key
{
private:
    static std::string sprite_name_tool;
    std::string direc = "-front";
    static cocos2d::Texture2D* transparent_texture;

public:
    ~moveable_sprite_key_tool() { CCLOG("~moveable_sprite_key_tool"); }

    //����һ��moveable_sprite_key_tool��ʵ��
    static moveable_sprite_key_tool* create(const std::string& plist_name);

    //�����ƶ�ָ��
    void moveable_sprite_key_tool::move_act(int direction) override;

    // ��ʼ����������
    void init_mouselistener();

    // ��갴��ʱ�Ļص�
    void on_mouse_click(cocos2d::Event* event);

    void update(float deltaTime) override;

};

#endif __MOVEABLE_SPRITE_KEY_H__