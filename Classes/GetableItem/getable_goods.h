//----------------------------------------------------
// 精灵：可通过工具在地图中获取的物品
// 说明：需要该物品名称、获取所需点击次数、获取所需工具
//----------------------------------------------------
#ifndef __GETABLE_GOOD_H__
#define  __GETABLE_GOOD_H__

#include "cocos2d.h"
#include "Global/Global.h"
#include "progress_bar.h"

#define DEFAULT_WIDTH 80
#define DEFAULT_HEIGHT 80
#define EXPERIENCE 5
#define PERCENT 0.5
const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> GOODS_MAP =
{
	{"grass",{{"get","straw"},{"tool","Hoe"}}},
	{"stones",{{"get","stone"},{"tool","Pick"}}},
	{"bigstone",{{"get","stone"},{"tool","Pick"}}},
	{"mine",{{"get","copper"},{"tool","Pick"}}},
	{"tree",{{"get","wood"},{"tool","Axe"}}},
	{"badGreenhouse",{{"get",""},{"tool",""}}}
};
const std::unordered_map<std::string, int> GOODS_CLICK_MAP =
{
	{"grass",3 * 2},{"stones",5 * 2},{"bigstone",10 * 2},{"mine",15 * 2},{"tree",10 * 2},{"badGreenhouse", 30 * 2}
};

class getable_goods : public cocos2d::Sprite
{
private:
	Size sprite_size;
	static cocos2d::Texture2D* transparent_texture;
	progress_bar* click_bar = nullptr;
protected:
	std::string sprite_name;
	int click_count = 0;
	bool is_getable = 0;//1->有可获取；0->已经被获取，此处为空
public:
	// 创建实例
	void set_info(std::string name, Size size);
	static getable_goods* create(const std::string& plist_name);
	virtual void setImag();

	bool get_is_getable() { return is_getable; }
	// 初始化鼠标监听器
	void init_mouselistener();
	// 鼠标按下时的回调
	void on_mouse_click(cocos2d::Event* event);
	void show_click_bar();
	void hide_click_bar();
	virtual void update();
	void add_in(ValueMap dict, getable_goods* sprite, std::string name, cocos2d::TMXTiledMap* tileMap);
	virtual ~getable_goods() {}
};


class GoodsManager :public Node
{
private:
	// 使用 std::vector 来存储精灵
	std::vector<getable_goods*> goods;
	int getable_count = 0;
public:
	static GoodsManager* create();
	// 添加精灵到容器
	void add_goods(getable_goods* sprite);

	// 迭代器遍历访问精灵
	void get_getable_count();

	// 随机访问一个精灵（通过迭代器）
	void random_access();
	void start_scheduler();
	void stop_scheduler();

};


#endif __GETABLE_GOOD_H__