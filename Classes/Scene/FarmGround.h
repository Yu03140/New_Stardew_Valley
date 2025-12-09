#ifndef __FARM_GROUND_H__
#define __FARM_GROUND_H__

#include "cocos2d.h"
#include "Charactor/BackpackLayer.h"
#include "Moveable/moveable_sprite_key.h"
#include "Global/Global.h"
#include "GetableItem/Crop.h"
#include "GetableItem/Animals.h"
#include "NPC/NPC.h"
#include "MinesScene.h"
#include "HomeScene.h"
#include "ShedScene.h"
#include "GetableItem/Fish.h"
#include "Shop/Shop_board_Layer.h"

#define MapSize 4
#define TOOL_WIDTH 40
#define TOOL_HEIGHT 60

struct Crops {
    std::string name;                                             // 物品名称
    crop* sprite;                                                 // 显示物品的精灵
};  

struct Fish {
    std::string name;                                             // 障碍物名称
    fish* sprite;                                                 // 显示物品的精灵
};

class FarmScene : public cocos2d::Scene {

public:
	static FarmScene* getInstance();						      // 获取 FarmScene 实例
    static Scene* createScene();                                  // 创建 FarmScene
	virtual bool init();									      // 初始化 FarmScene
	void addItem(const std::string& itemName);				      // 添加物品 
    CREATE_FUNC(FarmScene);                                       // 使用 CREATE_FUNC 宏
	cocos2d::TMXTiledMap* tileMap;						          // 瓦片地图 
    void update(float dt, moveable_sprite_key_walk* sprite_move); // 更新方法，每帧调用
	void replaceHouseImage();   						          // 替换房子图片  
	cocos2d::Sprite* badWarmHouse;							      // 棚屋精灵
	bool checkForwarmhouseClick(Vec2 mousePosition);              // 棚屋点击检测

private:
    void updateItemTexture(int slotIndex);                        // 更新物品纹理
    void clearItemTexture(int slotIndex);                         // 更新物品纹理
	void updateCameraPosition(float dt, Node* player);		      // 更新摄像机位置      
	std::vector<Crops> crops;									  // 作物容器
	std::vector<Fish> fish;									      // 鱼容器
	static FarmScene* instance;								      // FarmScene 实例
	void init_mouselistener();							 		  // 初始化鼠标监听器
    void on_mouse_click(cocos2d::Event* event);                   // 鼠标回调
	void checkForButtonClick(Vec2 mousePosition);                 // 检查按钮点击   
	void onEnter(); 										      // 进入场景
	void onExit();											      // 退出场景
    NPC* npc1;                                                    // NPC对象
	NPC* npc2;      							                  // NPC对象        
	void update(float delta);									  // 检查玩家是否与NPC接近并触发互动
	void checkNPCInteraction();								      // 检查NPC互动
	Board* board;										  		  // 任务面板
	TaskBarLayer* taskBarLayer;								      // 任务栏
	Shop_Board_Layer* shop_board_layer;							  // 商店面板
    cocos2d::Sprite* house;										  // 房子精灵
    cocos2d::Sprite* shed;								          // 棚屋精灵
    moveable_sprite_key_walk* sprite_move;						  // 人物控制类的引用

};

#endif  __FARM_GROUND_H__