#include "progress_bar.h"

bool progress_bar::init()
{
    // 创建进度条背景
    progressBarBackground = cocos2d::Sprite::create("progress_bar_bg.png");
    this->addChild(progressBarBackground);

    // 创建进度条前景
    progressBar = cocos2d::ProgressTimer::create(cocos2d::Sprite::create("progress_bar.png"));
    progressBar->setType(cocos2d::ProgressTimer::Type::BAR);
    progressBar->setBarChangeRate(cocos2d::Vec2(1, 0)); // 水平变化
    progressBar->setMidpoint(cocos2d::Vec2(0, 0)); // 设置进度条起始位置
    progressBar->setPercentage(0); // 初始化为 0%
    this->addChild(progressBar);

    return true;
}
progress_bar* progress_bar::create()
{
    progress_bar* ret = new progress_bar();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}
//显示进度条
void progress_bar::show_progress_bar(const cocos2d::Vec2& position)
{
    this->setPosition(position);
    this->setVisible(true); // 显示进度条
}
//更新进度
void progress_bar::update_progress_bar(float progress)
{
    if (progress < 0) {
        progress = 0;
    }
    if (progress > 100) {
        progress = 100; // 保证进度条不超过100%
    }
    progressBar->setPercentage(progress);
}
//隐藏进度条
void progress_bar::hide_progress_bar()
{
    this->setVisible(false);
}