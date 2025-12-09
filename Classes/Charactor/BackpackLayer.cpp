#include "BackpackLayer.h"
#include "cocos2d.h"
#include "Scene/FarmGround.h"

USING_NS_CC;

BackpackLayer* BackpackLayer::instance = nullptr;

BackpackLayer::BackpackLayer()
    : selectedItem(""), tilemap(nullptr), X0(0), Y0(0) {}

BackpackLayer::~BackpackLayer() {
    itemSlots.clear();
    CCLOG("BackpackLayer destroy"); 
}

// 创建背包界面
BackpackLayer* BackpackLayer::create() {
    BackpackLayer* ret = new BackpackLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        ret->retain();
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}


BackpackLayer* BackpackLayer::getInstance() {
    if (!instance) {
        CCLOG("Recreate instance");
        instance = BackpackLayer::create();
    }
    return instance;
}

// 销毁实例
void BackpackLayer::destroyInstance() {
    if (instance) {
        instance->removeFromParent();
        instance = nullptr;
    }
}


// 初始化函数
bool BackpackLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    // 获取瓦片地图背景
    auto visibleSize = Director::getInstance()->getVisibleSize();
    tilemap = TMXTiledMap::create("backpack_map.tmx");  
    if (!tilemap) {
        CCLOG("Failed to load backpack_map.tmx");
        return false;
    }

    this->addChild(tilemap, 0);
    auto mapSize = tilemap->getContentSize();

    // 设置瓦片地图位置
    X0 = (visibleSize.width - mapSize.width) / 2;  
    Y0 = visibleSize.height * 0.05f;              
    tilemap->setPosition(Vec2(X0, Y0));

    // 获取对象层（每个背包格子的位置）
    auto objectGroup = tilemap->getObjectGroup("Slots");  
    if (!objectGroup) {
        CCLOG("Failed to get object group 'Slots'");
        return false;
    }

    // 背包格子管理（假设最大12格）
    itemSlots.resize(12);
    for (int i = 0; i < 12; ++i) {
        itemSlots[i].name = "";      // 物品名
        itemSlots[i].quantity = 0;   // 物品数量

        // 获取对象层中每个格子的坐标
        auto object = objectGroup->getObject("Slot" + std::to_string(i + 1));  
        float posX = object["x"].asFloat();
        float posY = object["y"].asFloat();
        float width = object["width"].asFloat();
        float height = object["height"].asFloat();

        // 创建透明纹理的精灵
        auto sprite = Sprite::create();                                              
		sprite->setPosition(Vec2(posX, posY));                                    
		sprite->setAnchorPoint(Vec2(0, 0));                                       
		sprite->setContentSize(Size(width, height));                              
        tilemap->addChild(sprite, 1);                                             
        itemSlots[i].sprite = sprite;
    }

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bag.plist");        //传入背包纹理图集

	// 初始添加物品
    addItem("Axe1");
    addItem("Can1");
    addItem("Hoe1");
    addItem("Pick1");
    addItem("Rod1");
    addItem("strawberry", 5);
    addItem("pumpkin", 5);
    addItem("Rod1");
    addItem("pumpkin_fruit");
	addItem("chicken_egg");
	addItem("fertilizer1",5);


    // 添加点击事件监听器
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BackpackLayer::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

// 鼠标点击事件处理函数
void BackpackLayer::onMouseDown(Event* event) {
    EventMouse* mouseEvent = static_cast<EventMouse*>(event);
    Vec2 clickPosition = mouseEvent->getLocation();  // 获取鼠标点击位置

	// 获得鼠标的Tiled坐标
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float visibleHeight = visibleSize.height;
    clickPosition.y = visibleHeight - clickPosition.y; 

    // 遍历每个背包格子，检查是否点击了物品
    for (int i = 0; i < itemSlots.size(); ++i) {
        auto& slot = itemSlots[i];
        Rect slotRect(slot.sprite->getPositionX() + X0, slot.sprite->getPositionY() + Y0 - slot.sprite->getContentSize().height,
            slot.sprite->getContentSize().width * 3, slot.sprite->getContentSize().height * 3);

        if (slotRect.containsPoint(clickPosition)) {
            selectedItem = slot.name;
            break;  
        }
    }
}


// 添加物品
bool BackpackLayer::addItem(const std::string& itemName, const int num) {
    for (int i = 0; i < itemSlots.size(); ++i) {
        if (itemSlots[i].name == itemName) {
            // 如果物品已存在，增加数量并更新显示
            itemSlots[i].quantity+=num;
            updateItemTexture(i);
			return true;
        }
    }
    for (int i = 0; i < itemSlots.size(); ++i) {
        if (itemSlots[i].name == "") {
            // 找到空格子，放入物品
            itemSlots[i].name = itemName;
            itemSlots[i].quantity = num;
            updateItemTexture(i);
			return true; 
        }
    }
	return false; 
}

// 减少物品
bool BackpackLayer::removeItem(const std::string& itemName, const int num) {
    for (int i = 0; i < itemSlots.size(); ++i) {
		// 存在该物品且数量足够
        if (itemSlots[i].name == itemName && itemSlots[i].quantity >= num) {
            itemSlots[i].quantity -= num;
            if (itemSlots[i].quantity == 0) {
                // 物品数量为 0，清空该位置的纹理
                itemSlots[i].name = "";
				selectedItem = "";
                clearItemTexture(i);

            }
            else {
                updateItemTexture(i);
            }
            return true; // 删除物品成功
        }
    }
    return false;
}

// 获取选中物品
std::string BackpackLayer::getSelectedItem() const {
    if (selectedItem.empty()) {
        return "";  
    }
    else 
        return selectedItem;
}


// 更新物品显示纹理
void BackpackLayer::updateItemTexture(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= itemSlots.size()) return;

    auto& slot = itemSlots[slotIndex];
    if (slot.name != "") {
        std::string spriteFrameName = slot.name + ".png";  
        auto spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
        if (spriteFrame) {
            slot.sprite->setSpriteFrame(spriteFrame); 
            slot.sprite->setScale(3.0f);
        }
        else {
            CCLOG("Failed to find sprite frame: %s", spriteFrameName.c_str());
        }

        if (slot.sprite->getChildByTag(1001)) {
            slot.sprite->removeChildByTag(1001);
        }

        // 更新数量显示
        auto label = Label::createWithSystemFont(std::to_string(slot.quantity), "Arial", 6);
        label->setAnchorPoint(Vec2(0.0f, 0.6f));
        label->setPosition(slot.sprite->getContentSize().width, 0);
        label->setTextColor(Color4B::BLACK);
        slot.sprite->addChild(label, 1);
		label->setTag(1001);// 设置tag值

    }
}

// 清除物品纹理
void BackpackLayer::clearItemTexture(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= itemSlots.size()) return;

    auto& slot = itemSlots[slotIndex];
    slot.sprite->removeAllChildren();  

    //清空纹理
	auto spriteSize = slot.sprite->getContentSize();
    int dataSize = spriteSize.width * spriteSize.width * 4;  
    unsigned char* transparentData = new unsigned char[dataSize];
    memset(transparentData, 0, dataSize);
    cocos2d::Texture2D* transparentTexture = new cocos2d::Texture2D();
    transparentTexture->initWithData(transparentData, dataSize, cocos2d::backend::PixelFormat::RGBA8888, spriteSize.width, spriteSize.width, cocos2d::Size(spriteSize.width, spriteSize.width));
    slot.sprite->setTexture(transparentTexture);  
    delete[] transparentData;
}


