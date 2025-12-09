#include "AppDelegate.h"   // 引入应用程序委托类头文件
#include "Scene/MenuScene.h"  // 引入场景类头文件
#include "Global/Global.h"


// 是否启用音频引擎
// #define USE_AUDIO_ENGINE 1
#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

// 使用cocos2d的全局命名空间，避免每次都写Cocos2d::
USING_NS_CC;  

// 用于设置设计分辨率
static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);  // 设计分辨率
static cocos2d::Size smallResolutionSize = cocos2d::Size(800, 600);    // 小分辨率
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1600, 1200); // 中分辨率
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);  // 大分辨率

// 析构函数
AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();   // 如果启用了音频引擎，释放资源
#endif
}

// 初始化OpenGL上下文属性
void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

//如果使用第三方库，需要在这里注册
static int register_all_packages()
{
    return 0; // 表示没有外部包需要注册
}

// 应用程序完成启动并准备进入主循环时调用
bool AppDelegate::applicationDidFinishLaunching() {

    // 获取导演类实例，导演类负责管理场景切换和帧率控制
    auto director = Director::getInstance();

	// 获取OpenGL视图
    auto glview = director->getOpenGLView();

	// 如果OpenGL视图为空,创造一个新的视图
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("cocos", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("cocos");
#endif
        director->setOpenGLView(glview);
    }


    // 打开 FPS 显示，便于调试
    director->setDisplayStats(true);

    // 设置帧率，默认值为 1.0/60，即每秒 60 帧
    director->setAnimationInterval(1.0f / 60);

    // 设置设计分辨率
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    
    // 获取设备的实际分辨率
    auto frameSize = glview->getFrameSize();
    // 根据设备分辨率调整内容缩放比例
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    // 注册所有外部包（暂时没有）
    register_all_packages();

    // 创建一个场景对象，并运行
    auto scene = MenuScene::createScene();
    director->runWithScene(scene);

    return true;
}

// 当应用程序进入后台时调用（如接到电话或用户按下 Home 键）
void AppDelegate::applicationDidEnterBackground() {
    // 停止动画，暂停游戏
    Director::getInstance()->stopAnimation();

// 音频引擎调控
#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// 当应用程序重新进入前台时调用
void AppDelegate::applicationWillEnterForeground() {
    // 恢复动画，继续游戏
    Director::getInstance()->startAnimation();

// 音频引擎调控
#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
