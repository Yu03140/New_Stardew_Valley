#ifndef __RECIPE_LAYER_H__
#define __RECIPE_LAYER_H__

#include "cocos2d.h"
#include "BackpackLayer.h"
#include "Global/Global.h"  

class RecipeLayer : public cocos2d::Layer
{
public:
    RecipeLayer();
    ~RecipeLayer();
    static RecipeLayer* create();
    virtual bool init();
    void closeRecipe();                                                 // 关闭食谱层的回调函数 
    void canCookFood(const std::string& foodName);                      // 判断是否可以制作某个食物
    void createFoodSprites();                                           // 创建食物精灵并设置悬浮提示 
    void onFoodSpriteClicked(cocos2d::Ref* sender);                     // 点击食物精灵的回调函数 
    void createCloseButton();                                           // 创建XforTaskBar按钮
    void updateIngredientLabel(const std::string& foodName);            // 更新食物的原材料提示

private:
    cocos2d::Sprite* board;                                             // 底图
    cocos2d::Label* ingredientLabel;                                    // 原材料提示标签
    cocos2d::MenuItemImage* closeBtn;                                   // 关闭按钮
    std::map<std::string, std::vector<std::string>> foodRecipes;        // 食物与原材料的映射
    std::string currentFood;                                            // 当前选中的食物名称
};

#endif // __RECIPE_LAYER_H__
