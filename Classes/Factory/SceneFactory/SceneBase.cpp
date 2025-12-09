// SceneBase.cpp
#include "SceneBase.h"
#include "cocos2d.h"

USING_NS_CC;

//----------------------------------------------------
// SceneBase::createScene()
// ���ܣ���������ʵ��
//----------------------------------------------------
//Scene* SceneBase::createScene() {
//    // �ɾ�������ʵ��
//    return nullptr;
//}

//----------------------------------------------------
// SceneBase::init()
// ���ܣ���ʼ����������
//----------------------------------------------------
bool SceneBase::init() {
    if (!Scene::init()) {
        return false;
    }

    // ��ʼ�����������
    // 初始化鼠标和键盘监听器（统一在基类初始化，子类不需要重复调用）
    initMouseListener();
    initKeyboardListener();

    return true;
}

//----------------------------------------------------
// SceneBase::onEnter()
// ���ܣ���������ʱ����
//----------------------------------------------------
void SceneBase::onEnter() {
    Scene::onEnter();
    //CCLOG("Entering scene: %s", getSceneName().c_str());
}

//----------------------------------------------------
// SceneBase::onExit()
// ���ܣ������˳�ʱ����
//----------------------------------------------------
void SceneBase::onExit() {
    Scene::onExit();
    //CCLOG("Exiting scene: %s", getSceneName().c_str());
}

//----------------------------------------------------
// SceneBase::update()
// ���ܣ�ÿ֡����
//----------------------------------------------------
void SceneBase::update(float delta) {
    Scene::update(delta);
    // ��������߼�������������չ��
}

//----------------------------------------------------
// SceneBase::initMouseListener()
// ���ܣ���ʼ����������
//----------------------------------------------------
void SceneBase::initMouseListener() {
    if (mouseListener) {
        _eventDispatcher->removeEventListener(mouseListener);
        mouseListener = nullptr;
    }

    mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [this](Event* event) {
        this->onMouseClick(event);
        // 不阻止事件传播，让其他监听器（如工具精灵）也能收到事件
        };

    // 使用固定优先级而不是场景图优先级，避免与精灵的鼠标监听器冲突
    // 场景的监听器优先级较低，让精灵的监听器优先处理
    _eventDispatcher->addEventListenerWithFixedPriority(mouseListener, -1);
}

//----------------------------------------------------
// SceneBase::initKeyboardListener()
// ���ܣ���ʼ�����̼�����
//----------------------------------------------------
void SceneBase::initKeyboardListener() {
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        this->onKeyPressed(keyCode, event);
        // 不阻止事件传播，让其他监听器也能收到事件
        };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        this->onKeyReleased(keyCode, event);
        // 不阻止事件传播，让其他监听器也能收到事件
        };

    // 使用固定优先级而不是场景图优先级，避免与玩家精灵的监听器冲突
    // 场景的监听器优先级较低，让玩家精灵的监听器优先处理
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, -1);
}

//----------------------------------------------------
// SceneBase::loadTileMap()
// ���ܣ�������Ƭ��ͼ
//----------------------------------------------------
void SceneBase::loadTileMap(const std::string& tmxFile, float scale) {
    tileMapFile = tmxFile;
    mapScale = scale;

    // �����ͼ�Ѿ����ڣ����Ƴ���
    if (tileMap) {
        this->removeChild(tileMap);
        tileMap = nullptr;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    tileMap = TMXTiledMap::create(tmxFile);
    if (tileMap) {
        tileMap->setAnchorPoint(Vec2(0.5, 0.5));
        float mapPosX = visibleSize.width / 2 + origin.x;
        float mapPosY = visibleSize.height / 2 + origin.y;
        tileMap->setPosition(Vec2(mapPosX, mapPosY));
        tileMap->setScale(scale);

        this->addChild(tileMap, 0);

        // ���㳡���ߴ�
        Size mapContentSize = tileMap->getContentSize();
        sceneSize = Size(mapContentSize.width * scale, mapContentSize.height * scale);

        CCLOG("Loaded tile map: %s, Size: (%f, %f)",
            tmxFile.c_str(), sceneSize.width, sceneSize.height);
    }
    else {
        CCLOG("Failed to load tile map: %s", tmxFile.c_str());
    }
}

//----------------------------------------------------
// SceneBase::setupBackpackLayer()
// ���ܣ����ñ�����
//----------------------------------------------------
void SceneBase::setupBackpackLayer() {
    // ����������Ѿ����ڣ����Ƴ�
    if (this->getChildByName("backpackLayer")) {
        this->removeChildByName("backpackLayer");
    }

    backpackLayer = BackpackLayer::getInstance();
    if (backpackLayer) {
        // 重要：如果背包层已经有父节点，先移除它
        // 因为BackpackLayer是单例，可能在之前的场景中已经被添加
        if (backpackLayer->getParent()) {
            backpackLayer->removeFromParent();
        }
        
        // 然后添加到当前场景
        this->addChild(backpackLayer, Backpacklayer);
        backpackLayer->setName("backpackLayer");

        // ���ó�ʼλ��
        // 设置背包层位置（BackpackLayer内部的tilemap已经自己定位了，所以这里设置为原点即可）
        backpackLayer->setPosition(Vec2(0, 0));
    }
    else {
        CCLOG("Failed to setup backpack layer");
    }
}

//----------------------------------------------------
// SceneBase::updateCameraPosition()
// ���ܣ����������λ��
//----------------------------------------------------
//void SceneBase::updateCameraPosition(float dt, Node* player) {
//    if (!cameraFollowsPlayer || !player || sceneSize.width == 0 || sceneSize.height == 0) {
//        return;
//    }
//
//    auto playerPosition = player->getPosition();
//    auto visibleSize = Director::getInstance()->getVisibleSize();
//
//    // ���������Ӧ���ڵ�ͼ�߽��ڵ�λ��
//    float minCameraX = visibleSize.width / 2;
//    float maxCameraX = sceneSize.width - visibleSize.width / 2;
//    float minCameraY = visibleSize.height / 2;
//    float maxCameraY = sceneSize.height - visibleSize.height / 2;
//
//    // �����ͼ����ĻС���������ʾ
//    if (maxCameraX < minCameraX) {
//        minCameraX = maxCameraX = sceneSize.width / 2;
//    }
//    if (maxCameraY < minCameraY) {
//        minCameraY = maxCameraY = sceneSize.height / 2;
//    }
//
//    float cameraX = clamp(playerPosition.x, minCameraX, maxCameraX);
//    float cameraY = clamp(playerPosition.y, minCameraY, maxCameraY);
//
//    // ��ȡĬ������ͷ
//    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
//    if (camera) {
//        camera->setPosition3D(Vec3(cameraX, cameraY, camera->getPosition3D().z));
//
//        // ����UIλ��
//        float offsetX = cameraX - visibleSize.width / 2;
//        float offsetY = cameraY - visibleSize.height / 2;
//
//        if (backpackLayer) {
//            backpackLayer->setPosition(Vec2(offsetX, offsetY));
//        }
//    }
//}

//----------------------------------------------------
// SceneBase::onMouseClick()
// ���ܣ�������¼�����
//----------------------------------------------------
void SceneBase::onMouseClick(Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent) return;

    Vec2 mousePosition = mouseEvent->getLocationInView();
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // ת������������
    if (camera) {
        Vec2 cameraPosition = camera->getPosition();
        Vec2 windowOrigin = cameraPosition - Vec2(visibleSize.width / 2, visibleSize.height / 2);
        mousePosition += windowOrigin;
    }

    // ����ȫ�����λ�ã�������ȫ�ֱ�����
    MOUSE_POS = mousePosition;
    CCLOG("Mouse click at: (%f, %f)", MOUSE_POS.x, MOUSE_POS.y);
}

//----------------------------------------------------
// SceneBase::onKeyPressed()
// ���ܣ����������¼�����
//----------------------------------------------------
void SceneBase::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    // ����ʵ�֣��������д
    CCLOG("Key pressed: %d", static_cast<int>(keyCode));
}

//----------------------------------------------------
// SceneBase::onKeyReleased()
// ���ܣ������ͷ��¼�����
//----------------------------------------------------
void SceneBase::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    // ����ʵ�֣��������д
    CCLOG("Key released: %d", static_cast<int>(keyCode));
}

//----------------------------------------------------
// SceneBase::beforeLeave()
// ���ܣ��뿪����ǰ����
//----------------------------------------------------
//void SceneBase::beforeLeave() {
//    CCLOG("Preparing to leave scene: %s", getSceneName().c_str());
//
//    // �Ƴ���������
//    if (mouseListener) {
//        _eventDispatcher->removeEventListener(mouseListener);
//        mouseListener = nullptr;
//    }
//
//    // ���汳�������ã��Ա���������������������
//    if (backpackLayer) {
//        backpackLayer->retain();
//        backpackLayer->removeFromParent();
//    }
//}

//----------------------------------------------------
// SceneBase::afterEnter()
// ���ܣ����볡�������
//----------------------------------------------------
//void SceneBase::afterEnter() {
//    CCLOG("Finished entering scene: %s", getSceneName().c_str());
//
//    // ���³�ʼ����������
//    initMouseListener();
//}

//----------------------------------------------------
// SceneBase::convertToTilePosition()
// ���ܣ���������ת��Ϊ��Ƭ����
//----------------------------------------------------
Vec2 SceneBase::convertToTilePosition(const Vec2& worldPosition) {
    if (!tileMap) return Vec2::ZERO;

    Vec2 mapPosition = tileMap->convertToNodeSpace(worldPosition);
    Size tileSize = tileMap->getTileSize() * mapScale;

    int tileX = static_cast<int>(mapPosition.x / tileSize.width);
    int tileY = static_cast<int>(mapPosition.y / tileSize.height);

    return Vec2(tileX, tileY);
}

//----------------------------------------------------
// SceneBase::isPositionInMap()
// ���ܣ����λ���Ƿ��ڵ�ͼ��
//----------------------------------------------------
bool SceneBase::isPositionInMap(const Vec2& position) {
    if (!tileMap) return false;

    Vec2 mapPos = tileMap->getPosition();
    Size mapSize = sceneSize;

    return (position.x >= mapPos.x - mapSize.width / 2) &&
        (position.x <= mapPos.x + mapSize.width / 2) &&
        (position.y >= mapPos.y - mapSize.height / 2) &&
        (position.y <= mapPos.y + mapSize.height / 2);
}

//----------------------------------------------------
// SceneBase::clampToMapBoundary()
// ���ܣ����ƽڵ��ڵ�ͼ�߽���
//----------------------------------------------------
void SceneBase::clampToMapBoundary(Node* node) {
    if (!node || !tileMap) return;

    Vec2 position = node->getPosition();
    Vec2 mapPos = tileMap->getPosition();
    Size mapSize = sceneSize;
    Size nodeSize = node->getContentSize() * node->getScale();

    float minX = mapPos.x - mapSize.width / 2 + nodeSize.width / 2;
    float maxX = mapPos.x + mapSize.width / 2 - nodeSize.width / 2;
    float minY = mapPos.y - mapSize.height / 2 + nodeSize.height / 2;
    float maxY = mapPos.y + mapSize.height / 2 - nodeSize.height / 2;

    position.x = clamp(position.x, minX, maxX);
    position.y = clamp(position.y, minY, maxY);

    node->setPosition(position);
}

//----------------------------------------------------
// SceneBase::setupPlayer()
// ���ܣ�������ң�������ʵ�־����߼���
//----------------------------------------------------
void SceneBase::setupPlayer() {
    // �����ʵ�֣�������д
}

//----------------------------------------------------
// SceneBase::setupUI()
// ���ܣ�����UI��������ʵ�־����߼���
//----------------------------------------------------
void SceneBase::setupUI() {
    // �����ʵ�֣�������д
}