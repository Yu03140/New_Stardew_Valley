#ifndef __PROGRESS_BAR_H__
#define  __PROGRESS_BAR_H__

#include "cocos2d.h"

class progress_bar : public cocos2d::Sprite
{
private:
	cocos2d::ProgressTimer* progressBar = nullptr;  // 进度条前景
	cocos2d::Sprite* progressBarBackground = nullptr; // 进度条背景
	float progress = 0;  // 控制进度条的进度

public:
	bool init();
	static progress_bar* create();
	void show_progress_bar(const cocos2d::Vec2& position);
	void update_progress_bar(float delta);
	void hide_progress_bar();
};
#endif __PROGRESS_BAR_H__