#include "TaskBarLayer.h"
#include "Global/Global.h"
// 不再需要包含Scene文件夹下的头文件

cocos2d::Scene* TaskBarLayer::createScene() {
    auto scene = cocos2d::Scene::create(); // ����һ���³���
    auto layer = TaskBarLayer::create();  // ������������
    scene->addChild(layer); // �������������ӵ�������
    return scene; // ���س���
}

bool TaskBarLayer::init() {
    if (!Layer::init()) { // ���ø����ʼ������
        return false; // ���ʧ�ܣ��򷵻� false
    }

    // ���� TaskBar
    taskBar = TaskBar::create();
    //��һ��
    taskBar->addTask("Collect one strawberry\nfor Fizzer.", [this]() {
        if (backpackLayer->removeItem("strawberry", 1)) {
            auto player = Player::getInstance();
            player->playerproperty.addExperience(50);
            player->playerproperty.addMoney(50);
            cocos2d::log("First task completed: 50 XP and 50$ added.");

            // ����������
            taskBar->taskCompleted = true; // ��ʽ����������ɱ�־
        }
        else {
            cocos2d::log("Failed to complete the first task: Strawberry not found!");
        }
        });
    taskBar->showNextTask();

    taskBar->addTask("Repair the house\nfor Bouncer, please.", [this]() {
        if (1) {
            auto player = Player::getInstance();
            player->playerproperty.addExperience(50);
            player->playerproperty.addMoney(50);

            this->taskBar->taskCompleted = true; // ʹ�� this �������ʳ�Ա����
            cocos2d::log("Second task completed: 50 XP and 50$ added.");
        }

        });


    // ��������������Ļ����
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    taskBar->setAnchorPoint(Vec2(0.5, 0.5));
    taskBar->setPosition(cocos2d::Vec2(visibleSize.width / 2 - visibleSize.width * (0.1), visibleSize.height / 2 - visibleSize.height * (0.05)));
    taskBar->setVisible(false); // ��ʼ���ɼ�
    taskBar->setScale(4.0);
    this->addChild(taskBar, 2); // �����������ӵ���ǰ����

    // ������ť��������ʾ������
    auto showTaskBarButton = cocos2d::MenuItemImage::create(
        "TaskBarOrigin.png", // ��ť������״̬ͼ��
        "TaskBarOrigin.png", // ��ť��ѡ��״̬ͼ��
        [this](Ref* sender) { // ʹ�� [this] ����ǰ��ʵ��ָ��
            this->taskBar->show(); // ��ʾ������
        });

    showTaskBarButton->setScale(1.5);
    showTaskBarButton->setPosition(cocos2d::Vec2(visibleSize.width * 0.72, visibleSize.height * 0.6)); // ���ð�ťλ��
    showTaskBarButton->setOpacity(255); // ������ȫ��͸��


    // �����˵�������ť���ӵ��˵���
    auto menu = cocos2d::Menu::create(showTaskBarButton, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO + Vec2(visibleSize.width * (-0.01), visibleSize.width * (-0.05))); // ���ò˵�λ��Ϊԭ��
    menu->setScale(1.5);
    this->addChild(menu, 2); // ���˵����ӵ���ǰ����

    return true; // ��ʼ���ɹ�
}