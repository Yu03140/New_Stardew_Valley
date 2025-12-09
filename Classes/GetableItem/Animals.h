#ifndef __ANIMALS_H__
#define __ANIMALS_H__

#include "cocos2d.h"
#include "Global/Global.h"

#define FOOD "straw"
#define SPEED 10.0f
#define EXPERIENCE 10
#define EDGE1 120
#define EDGE0 10

const std::unordered_map<std::string, int> ANIMAL_MAP = { {"Pig",5},{"Goat",5},{"Chicken",1} ,{"Cow",7} };                                                  //动物的成长图鉴
const std::unordered_map<std::string, std::string> PRODUCE_MAP = { {"Pig","truffle"},{"Goat","goat_wool"},{"Chicken","chicken_egg"} ,{"Cow","cow_milk"} };  //动物生成物

class animals : public cocos2d::Sprite
{
private:

    Sprite* produce;
    int now_day = 0;                                                        //当前日期
    std::string animals_name;                                               //动物的名称
    int produce_day;                                                        //每次生成附属品所需要的天数
    bool is_produce = 0;                                                    //是否生成附属品
    int feed_count = 0;                                                     //喂养总天数
    int feed_today = 1;                                                     //今天剩余喂养次数
	static cocos2d::Texture2D* transparent_texture;
    cocos2d::Size produce_size;
    cocos2d::Vec2 produce_pos;
    static int count;
    int ID;
	float move_vecx[4] = { 0,0,-SPEED * 0.1f,SPEED * 0.1f };                //移动向量
	float move_vecy[4] = { SPEED * 0.1f,-SPEED * 0.1f ,0,0 };               //移动向量
    bool movement[4] = { false, false, false, false };
	bool is_hit_edge[4] = { false,false, false, false };                    //是否碰到边界
    int dic;
public:

    animals();
    void set_info(std::string name, cocos2d::Vec2 pos, cocos2d::Size size); //保存基本信息 
    static animals* create(const std::string& plist_name);
    void set_imag();
    void init_mouselistener();                                              // 初始化鼠标监听器
    void on_mouse_click(cocos2d::Event* event);                             // 鼠标按下时的回调
    void randmove(cocos2d::TMXTiledMap* tileMap);                           // 游荡
    void move_act(cocos2d::TMXTiledMap* tileMap);
    void scheduleRandomMove(cocos2d::TMXTiledMap* tileMap);
    void feed();                                                            // 喂食
    void create_produce();                                                  // 生成附属品
    void harvest();                                                         // 收获功能
    void update_day(float deltaTime);                                       // 新一天的更新
};

class AnimalsManager :public Node
{
private:
    std::vector<animals*> animals_list;
public:
    static AnimalsManager* create();
    void add_animals(animals* sprite);                                      // 添加精灵到容器
    void schedule_animals();                                                // 迭代器遍历访问精灵
};



#endif __ANIMALS_H__