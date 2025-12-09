#include "RecipeLayer.h"

USING_NS_CC;

RecipeLayer::RecipeLayer()
    : board(nullptr), ingredientLabel(nullptr), closeBtn(nullptr)
{
}

RecipeLayer::~RecipeLayer()
{
}

RecipeLayer* RecipeLayer::create()
{
    RecipeLayer* ret = new RecipeLayer();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        delete ret;
        return nullptr;
    }
}

bool RecipeLayer::init()
{
    // 创建底图
    board = Sprite::create("recipeboard.png");
    board->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(board);
    board->setScale(MapSize);

    // 创建食物精灵
    createFoodSprites();

    // 创建悬浮提示标签
    ingredientLabel = Label::createWithSystemFont("", "Arial", 24);
    ingredientLabel->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height * 0.8));
    ingredientLabel->setColor(Color3B::WHITE);
    this->addChild(ingredientLabel);

    // 创建关闭按钮
    createCloseButton();

    // 初始化食物和原材料的映射
    foodRecipes["ice_cream"] = { "cow_milk" };
    foodRecipes["french_fries"] = { "pumpkin_fruit" };
    foodRecipes["pizza"] = { "pumpkin_fruit", "chicken_egg"};

    return true;
}

void RecipeLayer::createFoodSprites()
{

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bag.plist");
    Vec2 Pos = Director::getInstance()->getVisibleSize() / 2;

    // 创造icecream精灵
    Sprite* icecream = Sprite::createWithSpriteFrameName("ice_cream.png");
    icecream->setPosition(Vec2(Pos.x-120,Pos.y));
    icecream->setTag(1);
    this->addChild(icecream, Taskbarlayer);
    icecream->setScale(MapSize * 2);

    // 创建frenchfries精灵
    Sprite* frenchfries = Sprite::createWithSpriteFrameName("french_fries.png");
    frenchfries->setPosition(Vec2(Pos.x, Pos.y));
    frenchfries->setTag(2);
    this->addChild(frenchfries, Taskbarlayer);
	frenchfries->setScale(MapSize * 2);

    // 创建pizza精灵
    Sprite* pizza = Sprite::createWithSpriteFrameName("pizza.png");
    pizza->setPosition(Vec2(Pos.x + 120, Pos.y));
    pizza->setTag(3);
    this->addChild(pizza, Taskbarlayer);
	pizza->setScale(MapSize * 2);

    // 添加触摸事件监听器
    auto icecreamListener = EventListenerTouchOneByOne::create();
    auto cakeListener = EventListenerTouchOneByOne::create();
    auto pizzaListener = EventListenerTouchOneByOne::create();

    // 设置事件监听器的触摸回调
    icecreamListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        if (target->getBoundingBox().containsPoint(touch->getLocation())) {
            CCLOG("Ice Cream clicked!");
            onFoodSpriteClicked(target);
            return true;
        }
        return false;
        };

    cakeListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        if (target->getBoundingBox().containsPoint(touch->getLocation())) {
            CCLOG("Cake clicked!");
            onFoodSpriteClicked(target);
            return true;
        }
        return false;
        };

    pizzaListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        if (target->getBoundingBox().containsPoint(touch->getLocation())) {
            CCLOG("Pizza clicked!");
            onFoodSpriteClicked(target);
            return true;
        }
        return false;
        };

    // 为每个精灵添加事件监听器
    icecream->getEventDispatcher()->addEventListenerWithSceneGraphPriority(icecreamListener, icecream);
    frenchfries->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cakeListener, frenchfries);
    pizza->getEventDispatcher()->addEventListenerWithSceneGraphPriority(pizzaListener, pizza);

}

void RecipeLayer::onFoodSpriteClicked(Ref* sender)
{
    Sprite* foodSprite = dynamic_cast<Sprite*>(sender);//?

    if (foodSprite)
    {
        // 根据精灵的Tag来判断点击的是哪个食物
        if (foodSprite->getTag() == 1)
            currentFood = "icecream";
        else if (foodSprite->getTag() == 2)
            currentFood = "cake";
        else if (foodSprite->getTag() == 3)
            currentFood = "pizza";

        // 更新食物原材料提示
        updateIngredientLabel(currentFood);
		canCookFood(currentFood);

    }
}


// 更新食物原材料提示
void RecipeLayer::updateIngredientLabel(const std::string& foodName)
{
    auto it = foodRecipes.find(foodName);
    if (it != foodRecipes.end())
    {
        std::string ingredients = "Ingredients: ";
        for (const auto& ingredient : it->second)
        {
            ingredients += ingredient + " ";
        }
        ingredientLabel->setString(ingredients);
    }
}

// 判断是否可以制作某个食物
void RecipeLayer::canCookFood(const std::string& foodName)
{
	//判断是否有足够的原材料
	auto backpacklayer = BackpackLayer::getInstance();
	auto it = foodRecipes.find(foodName);
	if (it != foodRecipes.end())
	{
        for (const auto& ingredient : it->second)
        {
			if (!backpacklayer->removeItem(ingredient))
			{
                return;
			}
        }
    }
	backpacklayer->addItem(foodName);
    return;
}

// 创建关闭按钮
void RecipeLayer::createCloseButton()
{
    closeBtn = MenuItemImage::create("XforTaskBar.png", "XforTaskBar.png", CC_CALLBACK_0(RecipeLayer::closeRecipe, this));
    closeBtn->setPosition(Vec2(Director::getInstance()->getVisibleSize().width - 50, Director::getInstance()->getVisibleSize().height - 50));

    Menu* closeMenu = Menu::create(closeBtn, nullptr);
    closeMenu->setPosition(Vec2::ZERO);
    this->addChild(closeMenu);
}

void RecipeLayer::closeRecipe()
{
    // 移除食谱层
    this->removeFromParentAndCleanup(true);
}
