// SceneBase.h
#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "cocos2d.h"
#include "Charactor/BackpackLayer.h"
#include "Moveable/moveable_sprite_key.h"
#include "Global/Global.h"


// ��������ö��
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

    // �������� - �������峡��
    //static cocos2d::Scene* createScene();

    // ��ʼ���ӿ�
    virtual bool init() override;

    // ͨ�ýӿ�
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float delta) override;

    // �������е�ͨ�÷���
    virtual void initMouseListener();
    virtual void initKeyboardListener();
    //virtual void updateCameraPosition(float dt, cocos2d::Node* player);

    // ��ȡ������Ϣ
    virtual SceneType getSceneType() const = 0;
    virtual std::string getSceneName() const = 0;

    // �����л����
    //virtual void beforeLeave();
    //virtual void afterEnter();

    // ͨ�����Է�����
    cocos2d::TMXTiledMap* getTileMap() const { return tileMap; }
    BackpackLayer* getBackpackLayer() const { return backpackLayer; }
    const cocos2d::Size& getSceneSize() const { return sceneSize; }

    // ���ñ����㣨���ڳ����л�ʱ�������ӣ�
    void setBackpackLayer(BackpackLayer* layer) { backpackLayer = layer; }
    
    // 设置场景参数
    void setSceneParams(const std::string& tmxFile, float scale = 4.0f) {
        tileMapFile = tmxFile;
        mapScale = scale;
    }

	// ͨ�ó�ʼ������
    virtual void loadTileMap(const std::string& tmxFile, float scale = 4.0f);
    virtual void setupBackpackLayer();

protected:
    // ͨ������
    cocos2d::TMXTiledMap* tileMap = nullptr;
    BackpackLayer* backpackLayer = nullptr;
    cocos2d::Size sceneSize;
    float mapScale = 4.0f;
    std::string tileMapFile;

    // ���������
    cocos2d::EventListenerMouse* mouseListener = nullptr;

    // ������������
    bool cameraFollowsPlayer = true;

    // ͨ�ó�ʼ������
    //virtual void loadTileMap(const std::string& tmxFile, float scale = 4.0f) ;
    //virtual void setupBackpackLayer();
    virtual void setupPlayer();
    virtual void setupUI();

    // �¼�����
    virtual void onMouseClick(cocos2d::Event* event);
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    // ���ߺ���
    cocos2d::Vec2 convertToTilePosition(const cocos2d::Vec2& worldPosition);
    bool isPositionInMap(const cocos2d::Vec2& position);
    void clampToMapBoundary(cocos2d::Node* node);

private:
    // ��ʼ�����������
    //void initKeyboardListener();

    // ����ֵ�ķ�Χ
    template <typename T>
    T clamp(T value, T low, T high) {
        if (value < low) return low;
        if (value > high) return high;
        return value;
    }
};

#endif // __SCENE_BASE_H__