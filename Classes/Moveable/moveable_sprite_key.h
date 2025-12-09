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
    const float speed = SPEED;//移动速度
    //数组各位置分别表示上、下、左、右方位
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

    //创建一个moveable_sprite_key的实例
    static moveable_sprite_key* create(const std::string& plist_name, float width, float height);

    //键盘监听器
    virtual void init_keyboardlistener();

    //按下键盘时，将对应方向参数修改为true
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    //松开键盘后，将对应方向参数修改会false
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    //更新位置
    void update(float deltaTime);

    //生成移动指令
    virtual void move_act(int direction);

    // 确保在 onEnter 中重新添加监听器
    void onEnter();

    // 确保在 onExit 中移除监听器
    void onExit();
};

class moveable_sprite_key_walk : public moveable_sprite_key
{
private:
    static std::string sprite_name_walk;
    static cocos2d::Texture2D* transparent_texture;

public:
    ~moveable_sprite_key_walk() { CCLOG("~moveable_sprite_key_walk"); }

    //创建一个moveable_sprite_key的实例
    static moveable_sprite_key_walk* create(const std::string& plist_name, const std::string& sprite_framename);

    //生成带有移动动画的移动指令
    void move_act(int direction) override;

    cocos2d::Vec2 get_pos();

    void moveable_sprite_key_walk::onEnter() {
        moveable_sprite_key::onEnter();  // 调用基类的 onEnter
        // 子类的其他初始化逻辑
    }

    void moveable_sprite_key_walk::onExit() {
        moveable_sprite_key::onExit();  // 调用基类的 onExit
        // 子类的其他清理逻辑
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

    //创建一个moveable_sprite_key_tool的实例
    static moveable_sprite_key_tool* create(const std::string& plist_name);

    //生成移动指令
    void moveable_sprite_key_tool::move_act(int direction) override;

    // 初始化鼠标监听器
    void init_mouselistener();

    // 鼠标按下时的回调
    void on_mouse_click(cocos2d::Event* event);

    void update(float deltaTime) override;

};

#endif __MOVEABLE_SPRITE_KEY_H__