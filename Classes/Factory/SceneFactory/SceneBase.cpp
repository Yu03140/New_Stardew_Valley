// SceneBase.cpp
#include "SceneBase.h"
#include "cocos2d.h"

USING_NS_CC;

//----------------------------------------------------
// SceneBase::createScene()
// 功能：创建场景实例
//----------------------------------------------------
//Scene* SceneBase::createScene() {
//    // 由具体子类实现
//    return nullptr;
//}

//----------------------------------------------------
// SceneBase::init()
// 功能：初始化场景基类
//----------------------------------------------------
bool SceneBase::init() {
    if (!Scene::init()) {
        return false;
    }

    // 初始化监听器（统一在基类初始化，子类不需要重复调用）
    // 初始化鼠标和键盘监听器（统一在基类初始化，子类不需要重复调用）
    initMouseListener();
    initKeyboardListener();

    return true;
}

//----------------------------------------------------
// SceneBase::onEnter()
// 功能：场景进入时调用
//----------------------------------------------------
void SceneBase::onEnter() {
    Scene::onEnter();
    //CCLOG("Entering scene: %s", getSceneName().c_str());
}

//----------------------------------------------------
// SceneBase::onEnterTransitionDidFinish()
// 功能：场景进入过渡完成时调用
//----------------------------------------------------
void SceneBase::onEnterTransitionDidFinish() {
    Scene::onEnterTransitionDidFinish();
    //CCLOG("Enter transition finished for scene: %s", getSceneName().c_str());
}

//----------------------------------------------------
// SceneBase::onExit()
// 功能：场景退出时调用
//----------------------------------------------------
void SceneBase::onExit() {
    // 关键修复：移除键盘监听器，防止场景销毁后仍然触发回调导致悬空指针
    if (keyboardListener) {
        _eventDispatcher->removeEventListener(keyboardListener);
        keyboardListener = nullptr;
    }
    
    // 鼠标监听器已经在 initMouseListener() 中处理了移除逻辑
    // 但为了安全，这里也检查一下
    if (mouseListener) {
        _eventDispatcher->removeEventListener(mouseListener);
        mouseListener = nullptr;
    }
    
    // 释放 retain() 引用，允许场景被销毁
    this->release();
    
    Scene::onExit();
    //CCLOG("Exiting scene: %s", getSceneName().c_str());
}

//----------------------------------------------------
// SceneBase::update()
// 功能：每帧更新
//----------------------------------------------------
void SceneBase::update(float delta) {
    Scene::update(delta);
    // 基类更新逻辑，子类可以扩展
}

//----------------------------------------------------
// SceneBase::initMouseListener()
// 功能：初始化鼠标监听器
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
    // 注意：固定优先级不能为0（0保留给场景图优先级），所以使用1
    // 优先级1确保场景能及时更新MOUSE_POS全局变量，但低于背包层（10）和工具精灵（2）
    // 允许事件继续传播，让其他监听器也能接收事件
    _eventDispatcher->addEventListenerWithFixedPriority(mouseListener, 1);
}

//----------------------------------------------------
// SceneBase::initKeyboardListener()
// 功能：初始化键盘监听器
//----------------------------------------------------
void SceneBase::initKeyboardListener() {
    // 如果已经存在监听器，先移除它
    if (keyboardListener) {
        _eventDispatcher->removeEventListener(keyboardListener);
        keyboardListener = nullptr;
    }

    keyboardListener = EventListenerKeyboard::create();

    // 关键修复：使用 retain() 确保场景在监听器存在期间不会被销毁
    // 这样可以避免 lambda 捕获的 this 指针访问已销毁的对象
    this->retain();
    
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        // 检查场景是否仍然有效（有父节点说明场景还在场景栈中）
        if (this && this->getParent()) {
            this->onKeyPressed(keyCode, event);
        }
        // 不阻止事件传播，让其他监听器也能收到事件
        };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        // 检查场景是否仍然有效（有父节点说明场景还在场景栈中）
        if (this && this->getParent()) {
            this->onKeyReleased(keyCode, event);
        }
        // 不阻止事件传播，让其他监听器也能收到事件
        };

    // 使用固定优先级而不是场景图优先级，避免与玩家精灵的监听器冲突
    // 场景的监听器优先级较低，让玩家精灵的监听器优先处理
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, -1);
}

//----------------------------------------------------
// SceneBase::loadTileMap()
// 功能：加载瓦片地图
//----------------------------------------------------
void SceneBase::loadTileMap(const std::string& tmxFile, float scale) {
    tileMapFile = tmxFile;
    mapScale = scale;

    // 如果地图已经存在，先移除它
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

        // 计算场景尺寸
        Size mapContentSize = tileMap->getContentSize();
        sceneSize = Size(mapContentSize.width * scale, mapContentSize.height * scale);
        
        // 更新全局变量SceneWidth和SceneHeight（用于兼容旧代码）
        SceneWidth = sceneSize.width;
        SceneHeight = sceneSize.height;

        CCLOG("Loaded tile map: %s, Size: (%f, %f)",
            tmxFile.c_str(), sceneSize.width, sceneSize.height);
    }
    else {
        CCLOG("Failed to load tile map: %s", tmxFile.c_str());
    }
}

//----------------------------------------------------
// SceneBase::setupBackpackLayer()
// 功能：设置背包层
//----------------------------------------------------
void SceneBase::setupBackpackLayer() {
    // 如果背包层已经存在，先移除
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

        // 设置初始位置
        // 设置背包层位置（BackpackLayer内部的tilemap已经自己定位了，所以这里设置为原点即可）
        backpackLayer->setPosition(Vec2(0, 0));
    }
    else {
        CCLOG("Failed to setup backpack layer");
    }
}

//----------------------------------------------------
// SceneBase::updateCameraPosition()
// 功能：更新摄像机位置
//----------------------------------------------------
//void SceneBase::updateCameraPosition(float dt, Node* player) {
//    if (!cameraFollowsPlayer || !player || sceneSize.width == 0 || sceneSize.height == 0) {
//        return;
//    }
//
//    auto playerPosition = player->getPosition();
//    auto visibleSize = Director::getInstance()->getVisibleSize();
//
//    // 计算摄像机应该在的地图边界内的位置
//    float minCameraX = visibleSize.width / 2;
//    float maxCameraX = sceneSize.width - visibleSize.width / 2;
//    float minCameraY = visibleSize.height / 2;
//    float maxCameraY = sceneSize.height - visibleSize.height / 2;
//
//    // 如果地图小于屏幕大小，居中显示
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
//    // 获取默认摄像头
//    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
//    if (camera) {
//        camera->setPosition3D(Vec3(cameraX, cameraY, camera->getPosition3D().z));
//
//        // 更新UI位置
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
// 功能：鼠标点击事件处理
//----------------------------------------------------
void SceneBase::onMouseClick(Event* event) {
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (!mouseEvent) return;
    
    // 只处理左键点击（用于移动），右键点击用于交互
    if (mouseEvent->getMouseButton() != EventMouse::MouseButton::BUTTON_LEFT) {
        return;
    }

    Vec2 mousePosition = mouseEvent->getLocationInView();
    auto camera = Director::getInstance()->getRunningScene()->getDefaultCamera();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 转换为世界坐标
    if (camera) {
        Vec2 cameraPosition = camera->getPosition();
        Vec2 windowOrigin = cameraPosition - Vec2(visibleSize.width / 2, visibleSize.height / 2);
        mousePosition += windowOrigin;
    }

    // 更新全局鼠标位置，供其他模块使用
    MOUSE_POS = mousePosition;
    CCLOG("Mouse click at: (%f, %f)", MOUSE_POS.x, MOUSE_POS.y);
}

//----------------------------------------------------
// SceneBase::onKeyPressed()
// 功能：键盘按下事件处理
//----------------------------------------------------
void SceneBase::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    // 基类实现，子类可覆写
    CCLOG("Key pressed: %d", static_cast<int>(keyCode));
}

//----------------------------------------------------
// SceneBase::onKeyReleased()
// 功能：键盘释放事件处理
//----------------------------------------------------
void SceneBase::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    // 基类实现，子类可覆写
    CCLOG("Key released: %d", static_cast<int>(keyCode));
}

//----------------------------------------------------
// SceneBase::beforeLeave()
// 功能：离开场景前调用
//----------------------------------------------------
//void SceneBase::beforeLeave() {
//    CCLOG("Preparing to leave scene: %s", getSceneName().c_str());
//
//    // 移除鼠标监听器
//    if (mouseListener) {
//        _eventDispatcher->removeEventListener(mouseListener);
//        mouseListener = nullptr;
//    }
//
//    // 保存背包层引用，以便在下一个场景中恢复
//    if (backpackLayer) {
//        backpackLayer->retain();
//        backpackLayer->removeFromParent();
//    }
//}

//----------------------------------------------------
// SceneBase::afterEnter()
// 功能：进入场景后调用
//----------------------------------------------------
//void SceneBase::afterEnter() {
//    CCLOG("Finished entering scene: %s", getSceneName().c_str());
//
//    // 重新初始化鼠标监听器
//    initMouseListener();
//}

//----------------------------------------------------
// SceneBase::convertToTilePosition()
// 功能：世界坐标转换为瓦片坐标
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
// 功能：检查位置是否在地图内
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
// 功能：限制节点在地图边界内
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
// 功能：设置玩家（由子类实现具体逻辑）
//----------------------------------------------------
void SceneBase::setupPlayer() {
    // 基类实现，子类可覆写
}

//----------------------------------------------------
// SceneBase::setupUI()
// 功能：设置UI（由子类实现具体逻辑）
//----------------------------------------------------
void SceneBase::setupUI() {
    // 基类实现，子类可覆写
}