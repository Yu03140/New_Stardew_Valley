#pragma once
#ifndef SHOP_BOARD_H
#define SHOP_BOARD_H

#include "cocos2d.h"
#include <string>
#include "Global/Global.h"


class Shop_Board : public cocos2d::Node {
public:

    static Shop_Board* create(std::function<void()> Shop_BoardCallback);


    virtual bool init(std::function<void()> Shop_BoardCallback);

    void show();


    void hide();

    int judgePrice() {
        if (timeSystem->getSeason() == 1) {
            return 12;
        }
        else {
            return 10;
        }
    }

private:

    bool isOpen = 0; ///< 任务是否完成的标志。
    cocos2d::Label* descriptionLabel; ///< 显示商品价格描述的标签。

    cocos2d::MenuItemImage* item_strawberry; ///<商品1。
    cocos2d::MenuItemImage* item_pumpkin; ///<商品2。




    cocos2d::MenuItemImage* closeButton; ///< “关闭”按钮。
    std::function<void()> Shop_BoardCallback; ///< 完成任务的回调函数。

    /**
     * @brief “完成任务”按钮的回调函数。
     */
    void buyitem_strawberry();
    void buyitem_pumpkin();

    /**
     * @brief “关闭”按钮的回调函数。
     */
    void onClose();
};

#endif // TASKBAR_H