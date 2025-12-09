#ifndef BACKPACK_LAYER_H
#define BACKPACK_LAYER_H

#include "cocos2d.h"

struct ItemSlot {
    std::string name;                                                                    // 物品名称
    int quantity;                                                                        // 物品数量
    cocos2d::Sprite* sprite;                                                             // 显示物品的精灵
};

class BackpackLayer : public cocos2d::Layer {
public:

    BackpackLayer();
    virtual ~BackpackLayer();
    static BackpackLayer* create();
    static BackpackLayer* getInstance();
    void BackpackLayer::destroyInstance();
    virtual bool init();
    void onMouseDown(cocos2d::Event* event);                                            // 鼠标点击事件处理函数
    bool BackpackLayer::addItem(const std::string& itemName, const int num = 1);        // 添加物品
	bool BackpackLayer::removeItem(const std::string& itemName, const int num = 1);     // 移除物品
    std::string getSelectedItem() const;                                                // 得到选中的物品
    cocos2d::TMXTiledMap* tilemap;                                                      // 瓦片地图

private:

	static BackpackLayer* instance;                                                     // 单例模式
    float X0;
    float Y0;
    std::string selectedItem;
	void updateItemTexture(int slotIndex);     							                // 更新物品纹理
	void clearItemTexture(int slotIndex);	  							                // 清除物品纹理
    std::vector<ItemSlot> itemSlots;                                                    // 背包格子数据
};

#endif // BACKPACK_LAYER_H
