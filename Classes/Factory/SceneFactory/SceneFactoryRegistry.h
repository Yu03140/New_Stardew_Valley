// SceneFactoryRegistry.h
#ifndef __SCENE_FACTORY_REGISTRY_H__
#define __SCENE_FACTORY_REGISTRY_H__

#include "SceneFactory.h"
#include "FarmSceneFactory.h"
#include "HomeSceneFactory.h"
#include "MinesSceneFactory.h"
#include "ShedSceneFactory.h"
#include "MenuSceneFactory.h"

// 注册所有场景工厂
class SceneFactoryRegistry {
public:
    static void registerAllFactories() {
        auto manager = SceneFactoryManager::getInstance();
        
        // 注册所有工厂
        manager->registerFactory(new FarmSceneFactory());
        manager->registerFactory(new HomeSceneFactory());
        manager->registerFactory(new MinesSceneFactory());
        manager->registerFactory(new ShedSceneFactory());
        manager->registerFactory(new MenuSceneFactory());
        
        CCLOG("All scene factories registered");
    }
};

#endif // __SCENE_FACTORY_REGISTRY_H__

