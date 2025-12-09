#ifndef __SUB_SCENE_H__
#define __SUB_SCENE_H__

#include "cocos2d.h"
#include "Global/Global.h"
#include"Moveable/moveable_sprite_key.h"

class SubScene : public cocos2d::Scene
{
public:

    static Scene* createScene() {};

    // 初始化场景
    virtual bool init() {
        // 调用父类初始化方法
        if (!Scene::init())
        {
            return false;
        }
        // 初始化瓦片地图
        initTileMap();

        // 加载地图，放在中间
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();

        if (tileMap) {
            tileMap->setAnchorPoint(Vec2(0.5, 0.5));
            float Map_posX = visibleSize.width / 2;
            float Map_posY = visibleSize.height / 2;
            tileMap->setPosition(Vec2(Map_posX, Map_posY));
            this->addChild(tileMap, 0);
            tileMap->setScale(MapSize);
        }
        else {
            CCLOG("Failed to load the tile map");
        }


        // 添加鼠标点击事件监听器
        auto mouseListener = EventListenerMouse::create();
        mouseListener->onMouseDown = CC_CALLBACK_1(SubScene::changeScene, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

        // 开启update函数
        this->scheduleUpdate();

        //----------------------------------------------------
        // 功能：添加移动主角
        // 说明：添加主角，主角位于地图中央
        // 图层：Playerlayer
        //----------------------------------------------------
        auto sprite_move = moveable_sprite_key_walk::create("Jas_Winter.plist", "Jas_Winter");
        if (sprite_move)
        {
            sprite_move->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
            this->addChild(sprite_move, Playerlayer);
            sprite_move->init_keyboardlistener();

        }
        main_char = sprite_move;
        this->schedule([this](float dt) {
            if (main_char)
            {
                main_char->update(dt); // 更新人物移动逻辑
            }
            }, "update_key_person");

        // 计算经过缩放后的实际尺寸
        Size originalSize = sprite_move->getContentSize();
        float scale = sprite_move->getScale();
        Size scaledSize = Size(originalSize.width * scale, originalSize.height * scale);
        auto sprite_tool = moveable_sprite_key_tool::create("Tools.plist");
        if (sprite_tool)
        {
            sprite_tool->setPosition(Vec2(visibleSize.width / 2 + origin.x + scaledSize.width / 2, visibleSize.height / 2 + origin.y));
            this->addChild(sprite_tool, Playerlayer);
            sprite_tool->init_keyboardlistener();
            sprite_tool->init_mouselistener();
        }
        main_tool = sprite_tool;
        this->schedule([this](float dt) {
            if (main_tool)
            {
                main_tool->update(dt); // 更新物品移动逻辑
            }
            }, "update_key_tool");

        return true;
    }

    // 添加一个初始化地图的函数
    virtual void initTileMap() {}

    // 重写update方法，便于扩展动态逻辑
    virtual void update(float delta) override {}

    // 鼠标点击事件处理
    virtual void changeScene(cocos2d::Event* event) {}

    void onEnter() {
        Scene::onEnter();

        //----------------------------------------------------
        // 功能：添加背包图层
        // 说明：添加背包图层到当前场景，初始化背包
        // 图层：Backpacklayer
        //----------------------------------------------------
        auto backpacklayer = BackpackLayer::getInstance();
        if (!backpacklayer->getParent()) {
            this->addChild(backpacklayer, Backpacklayer);
            backpacklayer->setPosition(Vec2(0, 0));
            backpacklayer->setName("backpacklayer");
            CCLOG("Success to load the backpack layer");
        }
        else
            CCLOG("Failed to load the backpack layer");

        // 在场景进入时添加键盘监听器
        main_char->init_keyboardlistener();
        this->schedule([this](float dt) {
            if (main_char)
            {
                main_char->update(dt); // 更新人物移动逻辑
            }
            }, "update_key_person");

        // 在场景进入时添加鼠标和键盘监听器
        main_tool->init_keyboardlistener();
        main_tool->init_mouselistener();
        this->schedule([this](float dt) {
            if (main_tool)
            {
                main_tool->update(dt); // 更新物品移动逻辑
            }
            }, "update_key_tool");
    }

    void onExit() {
        Scene::onExit();  // 调用基类的 onExit，确保场景的基本退出流程
        _eventDispatcher->removeEventListenersForTarget(main_char);
        if (backpackLayer) {
            CCLOG("!!Begin to remove");
            // 从当前场景中移除背包层，但不销毁它的内存
            backpackLayer->removeFromParent();
            CCLOG("!!remove backpackLayer successfully!");
        }
    }

protected:
    // 瓦片地图的指针
    cocos2d::TMXTiledMap* tileMap;
    moveable_sprite_key_walk* main_char;
    moveable_sprite_key_tool* main_tool;
};

#endif // __SUB_SCENE_H__