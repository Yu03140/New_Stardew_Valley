#pragma once
#ifndef SHOP_BOARD_LAYER_H
#define SHOP_BOARD_LAYER_H

#include "cocos2d.h"
#include "Shop_board.h"
#include "Attribute/attribute.h" 

class Shop_Board_Layer : public cocos2d::Layer {
public:

    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(Shop_Board_Layer);

private:

};

#endif // TASKBARSCENE_H
