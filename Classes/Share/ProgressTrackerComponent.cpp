#include "ProgressTrackerComponent.h"
#include "cocos2d.h"

ProgressTrackerComponent::ProgressTrackerComponent(std::function<bool()> condition, std::function<void()> action)
    : count(0), condition(condition), action(action)
{
}

ProgressTrackerComponent::ProgressTrackerComponent()
    : count(0), condition(nullptr), action(nullptr)
{
}

void ProgressTrackerComponent::setup(std::function<bool()> condition, std::function<void()> action)
{
    this->condition = condition;
    this->action = action;
}

void ProgressTrackerComponent::increment(int amount)
{
    count += amount;
    CCLOG("[ProgressTracker] Count incremented by %d, new count: %d", amount, count);
}

bool ProgressTrackerComponent::checkAndTrigger()
{
    if (condition && condition())
    {
        CCLOG("[ProgressTracker] Condition met, triggering action...");
        if (action)
        {
            action();
            return true;
        }
    }
    return false;
}

void ProgressTrackerComponent::reset()
{
    count = 0;
    CCLOG("[ProgressTracker] Count reset to 0");
}

int ProgressTrackerComponent::getCount() const
{
    return count;
}

void ProgressTrackerComponent::setCount(int value)
{
    count = value;
    CCLOG("[ProgressTracker] Count set to %d", count);
}
