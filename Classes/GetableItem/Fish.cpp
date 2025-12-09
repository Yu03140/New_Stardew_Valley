#include "Fish.h"
#define MAG_TIME_CROP 1.5f
// 定义静态成员变量并初始化
cocos2d::Texture2D* fish::transparent_texture = nullptr;
cocos2d::Size fish::fish_size = cocos2d::Size(0, 0);

// 创建实例
fish* fish::create(const std::string& plist_name, float width, float height)
{
    //加载plist文件
    cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist_name);
    fish_size.width = width;
    fish_size.height = height;

    //创建实例
    fish* fish_sprite = new fish();

    // 创建透明的内存块，设置为全透明 (RGBA8888 格式)
    int dataSize = width * height * 4;  // 每个像素 4 字节（RGBA 格式）
    unsigned char* transparentData = new unsigned char[dataSize];

    // 填充透明数据 (每个像素的 4 个通道值都为 0)
    memset(transparentData, 0, dataSize);

    // 创建透明纹理
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, width, height, cocos2d::Size(width, height));
    transparent_texture = transparentTexture;

    // 释放内存
    delete[] transparentData;


    //判断是否能成功创建
    if (transparentTexture)
    {
        fish_sprite->initWithTexture(transparentTexture);

        fish_sprite->autorelease();
        fish_sprite->init_mouselistener();
        return fish_sprite;
    }
    CC_SAFE_DELETE(fish_sprite);
    return nullptr;
}

// 初始化鼠标监听器
void fish::init_mouselistener()
{
    // 创建鼠标监听器
    auto listener = cocos2d::EventListenerMouse::create();
    // 鼠标按下时的回调
    listener->onMouseDown = CC_CALLBACK_1(fish::on_mouse_click, this);
    // 获取事件分发器，添加监听器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
// 鼠标按下时的回调
void fish::on_mouse_click(cocos2d::Event* event)
{
    //获取作物位置
    Vec2 fish_pos = this->convertToWorldSpace(Vec2(0, 0));
    // 计算点击的有效范围
    float min_x = fish_pos.x;
    float max_x = fish_pos.x + fish_size.width * MapSize;
    float min_y = fish_pos.y;
    float max_y = fish_pos.y + fish_size.height * MapSize;
    if (is_in_control) {
        //输出鼠标点击位置和有效范围
        if ((MOUSE_POS.x > min_x &&
            MOUSE_POS.x < max_x &&
            MOUSE_POS.y > min_y &&
            MOUSE_POS.y < max_y))
        {
                if (backpackLayer->getSelectedItem().find("Can") != std::string::npos) //手上的工具为水壶，执行浇水
                {
                    CCLOG("water this crop");
                    this->water(backpackLayer->getSelectedItem());
                }
                else if (backpackLayer->getSelectedItem().find("Rod") != std::string::npos)//手上的工具为鱼竿
                {
                    CCLOG("doing fishing");
                    this->fishing(backpackLayer->getSelectedItem());
                }
                else
                {
                    CCLOG("couldn't do anything to the crop");
                }

        }
    }
}

//取水
void fish::water(std::string name)
{
    this->setSpriteFrame("water.png");

    //背包水+1
    backpackLayer->addItem(name);
    CCLOG("water successfully");
}
  
//钓鱼
void fish::fishing(std::string name)
{

    auto sprite = cocos2d::Sprite::create("menu.png");
    if (sprite) {
        // 设置精灵的位置（屏幕中心为例）
        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
        auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();
        sprite->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);

        // 确保精灵在最上层
        sprite->setGlobalZOrder(1000); // 较大的值让它在更上层显示

        // 添加到当前场景
        this->addChild(sprite);
        CCLOG("create sprite.");

   // // 延迟一秒后隐藏精灵
   // this->scheduleOnce([sprite](float dt) {
   //     sprite->setVisible(false); // 或者直接用 removeFromParent()
   //     }, 1.0f, "hide_sprite");
    }
    else {
        CCLOG("Failed to create sprite.");
    }
    std::string framename = "bluefish.png";
    this->setSpriteFrame(framename);

    CCLOG("%s", HARVEST_FISH_MAP.at(fish_name));
    //把生成物加入背包 
    backpackLayer->addItem(fish_name,1);
    //人物经验增加10
    Player* player = Player::getInstance("me");
    player->playerproperty.addExperience(EXPERIENCE);
    CCLOG("fishing successfully");

}

//清除
void fish::clear()
{
    fish_name = "";
    this->initWithTexture(transparent_texture);
}
