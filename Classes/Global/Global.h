#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <vector>
#include "cocos2d.h"
#include "Charactor/BackpackLayer.h"
#include "TimeSystem/TimeSystem.h"
#include "Attribute/attribute.h"
#include "TaskBar/TaskBarLayer.h"
#include "Board/Board.h"
#include "./Moveable/moveable_sprite_key.h"

//全局定义
#define CONTROL_RANGE 150 
#define TOOL_WIDTH 40
#define TOOL_HEIGHT 60


// 【新增：前向声明 moveable_sprite_key_tool 类】
// 避免直接包含 moveable_sprite_key.h 来打破可能的循环依赖
namespace cocos2d {
    class Vec2;
    class Texture2D;
}
class TimeSystem;
class BackpackLayer;
class moveable_sprite_key_tool;


extern TimeSystem* timeSystem;
extern cocos2d::Vec2 character_pos;
extern BackpackLayer* backpackLayer;
extern bool is_in_control;
extern float SceneWidth;
extern float SceneHeight;
extern cocos2d::Vec2 MOUSE_POS;
extern bool is_infarm;
extern moveable_sprite_key_tool* sprite_tool;

#define Playerlayer 1
#define Backpacklayer 2
#define Timesystemlayer 3
#define Taskbarlayer 4
#define MapSize 4

#endif // GLOBAL_H



