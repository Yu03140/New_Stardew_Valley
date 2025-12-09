// SceneFactory.cpp
#include "SceneFactory.h"
#include "cocos2d.h"

USING_NS_CC;

//----------------------------------------------------
// SceneFactory::validateParams()
// ���ܣ���֤���������Ƿ���Ч
//----------------------------------------------------
bool SceneFactory::validateParams(const SceneCreateParams& params) const {
    // 基础校验：地图文件名非空；文件存在性仅警告，避免因资源搜索路径导致直接失败
    if (params.tileMapFile.empty()) {
        CCLOG("Error: Tile map file is empty");
        return false;
    }

    auto fileUtils = FileUtils::getInstance();
    if (!fileUtils->isFileExist(params.tileMapFile)) {
        CCLOG("Warning: Tile map file not found by isFileExist: %s (will still try to load)", params.tileMapFile.c_str());
    }

    if (params.mapScale <= 0.0f || params.mapScale > 10.0f) {
        CCLOG("Warning: Map scale %f may be unreasonable", params.mapScale);
    }

    return true;
}

//----------------------------------------------------
// SceneFactory::createTransition()
// ���ܣ�������������Ч��
//----------------------------------------------------
//TransitionScene* SceneFactory::createTransition(Scene* scene, float duration) {
//    if (!scene) return nullptr;
//
//    switch (getPreferredTransition()) {
//    case TransitionType::FADE:
//        return TransitionFade::create(duration, scene);
//
//    case TransitionType::SLIDE:
//        return TransitionSlideInR::create(duration, scene);
//
//    case TransitionType::FLIP:
//        return TransitionFlipX::create(duration, scene);
//
//    case TransitionType::NONE:
//    default:
//        return nullptr;
//    }
//}

//----------------------------------------------------
// SceneFactory::initSceneCommon()
// ���ܣ���ʼ������ͨ�ò���
//----------------------------------------------------
void SceneFactory::initSceneCommon(SceneBase* scene, const SceneCreateParams& params) {
    if (!scene) return;

    // ������Ƭ��ͼ
    if (!params.tileMapFile.empty()) {
        scene->loadTileMap(params.tileMapFile, params.mapScale);
    }

    // ���ñ�����
    if (params.showBackpack) {
        scene->setupBackpackLayer();
    }

    // �������������
    // ע�⣺����ֻ�����ñ�־��ʵ�ʸ�����������ʵ��
}

//----------------------------------------------------
// SceneFactory::setupCommonComponents()
// ���ܣ����ó���ͨ�����
//----------------------------------------------------
void SceneFactory::setupCommonComponents(SceneBase* scene, const SceneCreateParams& params) {
    if (!scene) return;

    // ��������������г�������Ҫ��ͨ�����
    // ���磺ʱ��ϵͳ������ϵͳ��ȫ��UI��

    // ���������ʼλ��
    if (params.playerStartPosition != Vec2::ZERO) {
        // ��Ҫ�������о���ʵ��
    }
}

//----------------------------------------------------
// SceneFactory::setupCamera()
// ���ܣ����������
//----------------------------------------------------
//void SceneFactory::setupCamera(SceneBase* scene, const SceneCreateParams& params) {
//    if (!scene) return;
//
//    // �������������
//    auto camera = scene->getDefaultCamera();
//    if (camera) {
//        // �����������������������
//        // ���磺���š���ת��ͶӰ���͵�
//    }
//}

//----------------------------------------------------
// SceneFactory::setupAudio()
// ���ܣ�������Ч
//----------------------------------------------------
//void SceneFactory::setupAudio(SceneBase* scene, const SceneCreateParams& params) {
//    if (!scene || params.backgroundMusic.empty()) return;
//
//    // ���ű�������
//    // ע�⣺ʵ�ʲ��ſ�����Ҫ������Ϸ���õ���
//    // SimpleAudioEngine::getInstance()->playBackgroundMusic(params.backgroundMusic.c_str(), true);
//}

//----------------------------------------------------
// SceneFactoryManager ʵ��
//----------------------------------------------------
SceneFactoryManager* SceneFactoryManager::instance = nullptr;

SceneFactoryManager* SceneFactoryManager::getInstance() {
    if (!instance) {
        instance = new SceneFactoryManager();
    }
    return instance;
}

SceneFactoryManager::~SceneFactoryManager() {
    cleanup();
}

//----------------------------------------------------
// SceneFactoryManager::registerFactory()
// ���ܣ�ע�Ṥ��
//----------------------------------------------------
void SceneFactoryManager::registerFactory(SceneFactory* factory) {
    if (!factory) return;

    std::string sceneName = factory->getSceneName();
    SceneType sceneType = factory->getSceneType();

    // ����Ƿ���ע��
    if (factoriesByName.find(sceneName) != factoriesByName.end()) {
        CCLOG("Warning: Factory for scene %s already registered", sceneName.c_str());
        return;
    }

    // ע��
    factoriesByName[sceneName] = factory;
    factoriesByType[sceneType] = factory;

    CCLOG("Registered factory for scene: %s", sceneName.c_str());
}

//----------------------------------------------------
// SceneFactoryManager::unregisterFactory()
// ���ܣ�ע������
//----------------------------------------------------
void SceneFactoryManager::unregisterFactory(const std::string& sceneName) {
    auto it = factoriesByName.find(sceneName);
    if (it != factoriesByName.end()) {
        SceneFactory* factory = it->second;
        SceneType sceneType = factory->getSceneType();

        factoriesByName.erase(it);
        factoriesByType.erase(sceneType);

        CCLOG("Unregistered factory for scene: %s", sceneName.c_str());
    }
}

//----------------------------------------------------
// SceneFactoryManager::getFactory()
// ���ܣ�ͨ�����ƻ�ȡ����
//----------------------------------------------------
SceneFactory* SceneFactoryManager::getFactory(const std::string& sceneName) {
    auto it = factoriesByName.find(sceneName);
    if (it != factoriesByName.end()) {
        return it->second;
    }
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::getFactory()
// ���ܣ�ͨ�����ͻ�ȡ����
//----------------------------------------------------
SceneFactory* SceneFactoryManager::getFactory(SceneType sceneType) {
    auto it = factoriesByType.find(sceneType);
    if (it != factoriesByType.end()) {
        return it->second;
    }
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::createScene()
// ���ܣ���������
//----------------------------------------------------
Scene* SceneFactoryManager::createScene(const std::string& sceneName) {
    SceneFactory* factory = getFactory(sceneName);
    if (factory) {
        return factory->createScene();
    }

    CCLOG("Error: No factory found for scene: %s", sceneName.c_str());
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::createScene()
// ���ܣ�ͨ�����ʹ�������
//----------------------------------------------------
Scene* SceneFactoryManager::createScene(SceneType sceneType) {
    SceneFactory* factory = getFactory(sceneType);
    if (factory) {
        return factory->createScene();
    }

    CCLOG("Error: No factory found for scene type: %d", static_cast<int>(sceneType));
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::createSceneWithParams()
// ���ܣ������������ĳ���
//----------------------------------------------------
Scene* SceneFactoryManager::createSceneWithParams(const std::string& sceneName,
    const SceneCreateParams& params) {
    SceneFactory* factory = getFactory(sceneName);
    if (factory) {
        return factory->createSceneWithParams(params);
    }

    CCLOG("Error: No factory found for scene: %s", sceneName.c_str());
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::createSceneWithParams()
// ���ܣ�ͨ�����ʹ����������ĳ���
//----------------------------------------------------
Scene* SceneFactoryManager::createSceneWithParams(SceneType sceneType,
    const SceneCreateParams& params) {
    SceneFactory* factory = getFactory(sceneType);
    if (factory) {
        return factory->createSceneWithParams(params);
    }

    CCLOG("Error: No factory found for scene type: %d", static_cast<int>(sceneType));
    return nullptr;
}

//----------------------------------------------------
// SceneFactoryManager::getAllSceneNames()
// ���ܣ���ȡ����ע��ĳ�������
//----------------------------------------------------
std::vector<std::string> SceneFactoryManager::getAllSceneNames() const {
    std::vector<std::string> names;
    for (const auto& pair : factoriesByName) {
        names.push_back(pair.first);
    }
    return names;
}

//----------------------------------------------------
// SceneFactoryManager::getScenesWithFeature()
// ���ܣ���ȡ֧���ض����Եĳ���
//----------------------------------------------------
//std::vector<std::string> SceneFactoryManager::getScenesWithFeature(const std::string& feature) const {
//    std::vector<std::string> scenes;
//
//    for (const auto& pair : factoriesByName) {
//        SceneFactory* factory = pair.second;
//        bool hasFeature = false;
//
//        if (feature == "farming" && factory->supportsFarming()) hasFeature = true;
//        else if (feature == "mining" && factory->supportsMining()) hasFeature = true;
//        else if (feature == "fishing" && factory->supportsFishing()) hasFeature = true;
//        else if (feature == "npcs" && factory->supportsNPCs()) hasFeature = true;
//        else if (feature == "buildings" && factory->hasBuildings()) hasFeature = true;
//
//        if (hasFeature) {
//            scenes.push_back(pair.first);
//        }
//    }
//
//    return scenes;
//}

//----------------------------------------------------
// SceneFactoryManager::cleanup()
// ���ܣ��������й���
//----------------------------------------------------
void SceneFactoryManager::cleanup() {
    // ע�⣺���ﲻɾ������������Ϊ���������Ǿ�̬��������ģ�����
    factoriesByName.clear();
    factoriesByType.clear();
}