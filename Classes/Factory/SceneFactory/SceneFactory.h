// SceneFactory.h
#ifndef __SCENE_FACTORY_H__
#define __SCENE_FACTORY_H__

#include "cocos2d.h"
#include "SceneBase.h"

// 场景创建参数
struct SceneCreateParams {
    std::string tileMapFile;
    float mapScale = 4.0f;
    //bool cameraFollowPlayer = true;
    bool showBackpack = true;
    cocos2d::Vec2 playerStartPosition;

    // 可扩展的其他参数
    //std::string playerSpriteFile;
    //std::string backgroundMusic;

    SceneCreateParams() = default;

    SceneCreateParams(const std::string& mapFile, float scale = 4.0f)
        : tileMapFile(mapFile), mapScale(scale) {
    }
};

// 抽象场景工厂类
class SceneFactory {
public:
    virtual ~SceneFactory() = default;

    // 工厂方法 - 创建场景实例
    virtual cocos2d::Scene* createScene() = 0;

    // 带参数的工厂方法
    virtual cocos2d::Scene* createSceneWithParams(const SceneCreateParams& params) = 0;

    // 获取场景名称
    virtual std::string getSceneName() const = 0;

    // 获取场景类型
    virtual SceneType getSceneType() const = 0;

    // 场景描述
    //virtual std::string getSceneDescription() const { return "A game scene"; }

    // 场景是否支持某些特性
    //virtual bool supportsFarming() const { return false; }
    //virtual bool supportsMining() const { return false; }
    //virtual bool supportsFishing() const { return false; }
    //virtual bool supportsNPCs() const { return false; }
    //virtual bool hasBuildings() const { return false; }

    //// 创建默认参数
    //virtual SceneCreateParams getDefaultParams() const = 0;

    // 验证参数是否有效
    virtual bool validateParams(const SceneCreateParams& params) const;

    //// 场景切换的过渡效果
    //enum class TransitionType {
    //    NONE,
    //    FADE,
    //    SLIDE,
    //    FLIP,
    //    CUSTOM
    //};

    // 获取场景切换效果
    //virtual TransitionType getPreferredTransition() const { return TransitionType::FADE; }

    // 创建过渡效果
    //virtual cocos2d::TransitionScene* createTransition(cocos2d::Scene* scene, float duration = 0.5f);

protected:
    // 创建场景的辅助方法
    virtual SceneBase* createSceneBase() = 0;

    // 初始化场景的通用部分
    virtual void initSceneCommon(SceneBase* scene, const SceneCreateParams& params);

    // 设置场景通用组件
    virtual void setupCommonComponents(SceneBase* scene, const SceneCreateParams& params);

    // 配置摄像机
    //virtual void setupCamera(SceneBase* scene, const SceneCreateParams& params);

    // 配置音效
    //virtual void setupAudio(SceneBase* scene, const SceneCreateParams& params);
};

// 工厂管理类（单例）
class SceneFactoryManager {
public:
    static SceneFactoryManager* getInstance();

    // 注册工厂
    void registerFactory(SceneFactory* factory);

    // 注销工厂
    void unregisterFactory(const std::string& sceneName);

    // 通过名称获取工厂
    SceneFactory* getFactory(const std::string& sceneName);

    // 通过类型获取工厂
    SceneFactory* getFactory(SceneType sceneType);

    // 创建场景
    cocos2d::Scene* createScene(const std::string& sceneName);
    cocos2d::Scene* createScene(SceneType sceneType);

    // 创建带参数的场景
    cocos2d::Scene* createSceneWithParams(const std::string& sceneName, const SceneCreateParams& params);
    cocos2d::Scene* createSceneWithParams(SceneType sceneType, const SceneCreateParams& params);

    // 获取所有注册的场景名称
    std::vector<std::string> getAllSceneNames() const;

    // 获取所有支持的特性
    std::vector<std::string> getScenesWithFeature(const std::string& feature) const;

    // 清理所有工厂
    void cleanup();

private:
    SceneFactoryManager() = default;
    ~SceneFactoryManager();

    static SceneFactoryManager* instance;

    std::unordered_map<std::string, SceneFactory*> factoriesByName;
    std::unordered_map<SceneType, SceneFactory*> factoriesByType;

    // 禁止复制
    SceneFactoryManager(const SceneFactoryManager&) = delete;
    SceneFactoryManager& operator=(const SceneFactoryManager&) = delete;
};

#endif // __SCENE_FACTORY_H__