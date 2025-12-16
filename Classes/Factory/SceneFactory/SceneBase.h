// SceneBase.h
#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "cocos2d.h"
#include "Charactor/BackpackLayer.h"
#include "Moveable/moveable_sprite_key.h"
#include "Global/Global.h"


// 场景类型枚举
enum class SceneType {
    FARMGROUND_SCENE,
    MINES_SCENE,
    HOME_SCENE,
    HENU_SCENE,
    SHED_SCENE,
    UNKNOWN
};

class SceneBase : public cocos2d::Scene {
public:
    virtual ~SceneBase() = default;

    // 创建场景 - 由子类各自实现
    //static cocos2d::Scene* createScene();

    // 初始化接口
    virtual bool init() override;

    // 通用接口
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExit() override;
    virtual void update(float delta) override;

    // 所有场景的通用方法
    virtual void initMouseListener();
    virtual void initKeyboardListener();
    //virtual void updateCameraPosition(float dt, cocos2d::Node* player);

    // 获取场景信息
    virtual SceneType getSceneType() const = 0;
    virtual std::string getSceneName() const = 0;

    // 场景切换相关
    //virtual void beforeLeave();
    //virtual void afterEnter();

    // 通用属性访问器
    cocos2d::TMXTiledMap* getTileMap() const { return tileMap; }
    BackpackLayer* getBackpackLayer() const { return backpackLayer; }
    const cocos2d::Size& getSceneSize() const { return sceneSize; }

    // 设置背包层（用于场景切换时保留引用）
    void setBackpackLayer(BackpackLayer* layer) { backpackLayer = layer; }
    
    // 设置场景参数
    void setSceneParams(const std::string& tmxFile, float scale = 4.0f) {
        tileMapFile = tmxFile;
        mapScale = scale;
    }

	// 通用初始化方法
    virtual void loadTileMap(const std::string& tmxFile, float scale = 4.0f);
    virtual void setupBackpackLayer();

protected:
    // 通用属性
    cocos2d::TMXTiledMap* tileMap = nullptr;
    BackpackLayer* backpackLayer = nullptr;
    cocos2d::Size sceneSize;
    float mapScale = 4.0f;
    std::string tileMapFile;

    // 鼠标监听器
    cocos2d::EventListenerMouse* mouseListener = nullptr;
    
    // 键盘监听器
    cocos2d::EventListenerKeyboard* keyboardListener = nullptr;

    // 摄像机跟随玩家
    bool cameraFollowsPlayer = true;

    // 通用初始化方法
    //virtual void loadTileMap(const std::string& tmxFile, float scale = 4.0f) ;
    //virtual void setupBackpackLayer();
    virtual void setupPlayer();
    virtual void setupUI();

    // 事件处理
    virtual void onMouseClick(cocos2d::Event* event);
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // 辅助函数
    cocos2d::Vec2 convertToTilePosition(const cocos2d::Vec2& worldPosition);
    bool isPositionInMap(const cocos2d::Vec2& position);
    void clampToMapBoundary(cocos2d::Node* node);

private:
    // 初始化键盘监听器
    //void initKeyboardListener();

    // 限制值的范围
    template <typename T>
    T clamp(T value, T low, T high) {
        if (value < low) return low;
        if (value > high) return high;
        return value;
    }
};

#endif // __SCENE_BASE_H__