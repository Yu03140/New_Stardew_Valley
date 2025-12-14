#include "Global.h"
#include "./Moveable/moveable_sprite_key.h"

TimeSystem* timeSystem = nullptr;
BackpackLayer* backpackLayer = nullptr;
moveable_sprite_key_tool* sprite_tool = nullptr;

cocos2d::Vec2 character_pos = cocos2d::Vec2::ZERO;
cocos2d::Vec2 MOUSE_POS = cocos2d::Vec2::ZERO;
bool is_infarm;
bool is_in_control = 0;

float SceneWidth;
float SceneHeight;



