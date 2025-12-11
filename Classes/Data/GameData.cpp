#include "GameData.h"

GameData* GameData::_instance = nullptr;

GameData* GameData::getInstance() {
    if (!_instance) {
        _instance = new GameData();
        _instance->loadAllData(); // 第一次获取时自动加载
    }
    return _instance;
}

GameData::GameData() {}

void GameData::loadAllData() {
    // 1. 加载 Item 数据 (原 GOODS_MAP)
    // 格式: id, drop, tool, clicks
    _itemModels["grass"] = new ItemModel("grass", "straw", "Hoe", 6);
    _itemModels["stones"] = new ItemModel("stones", "stone", "Pick", 10);
    _itemModels["bigstone"] = new ItemModel("bigstone", "stone", "Pick", 20);
    _itemModels["mine"] = new ItemModel("mine", "copper", "Pick", 30);
    _itemModels["tree"] = new ItemModel("tree", "wood", "Axe", 20);
    _itemModels["badGreenhouse"] = new ItemModel("badGreenhouse", "", "", 60);

    // 2. 加载 Crop 数据 (原 CROP_MAP + HARVEST_MAP)
    // 格式: id, harvestItem, developDays, season
    _cropModels["strawberry"] = new CropModel("strawberry", "strawberry_fruit", 5, SPRING);
    _cropModels["sunflour"] = new CropModel("sunflour", "sunflour_fruit", 4, SUMMER);
    _cropModels["pumpkin"] = new CropModel("pumpkin", "sunflour_fruit", 5, AUTUMN); // 备注：原来你的代码里南瓜产出写的是 sunflour_fruit，我照搬了

    // 3. 加载 Animal 数据 (原 ANIMAL_MAP + PRODUCE_MAP)
    // 格式: id, produceItem, produceCycle
    _animalModels["Pig"] = new AnimalModel("Pig", "truffle", 5);
    _animalModels["Goat"] = new AnimalModel("Goat", "goat_wool", 5);
    _animalModels["Chicken"] = new AnimalModel("Chicken", "chicken_egg", 1);
    _animalModels["Cow"] = new AnimalModel("Cow", "cow_milk", 7);

    // 4. 加载 Fish 数据 (原 FISH_MAP + HARVEST_FISH_MAP)
    FishModel* bluefish = new FishModel("bluefish", "bluefish");
    bluefish->addSeasonRule(SPRING, 5); // 春天 5 天
    bluefish->addSeasonRule(SUMMER, 4); // 夏天 4 天
    bluefish->addSeasonRule(AUTUMN, 5); // 秋天 5 天
    _fishModels["bluefish"] = bluefish;
}

ItemModel* GameData::getItemModel(const std::string& id) {
    if (_itemModels.find(id) != _itemModels.end()) return _itemModels[id];
    return nullptr;
}

CropModel* GameData::getCropModel(const std::string& id) {
    if (_cropModels.find(id) != _cropModels.end()) return _cropModels[id];
    return nullptr;
}

AnimalModel* GameData::getAnimalModel(const std::string& id) {
    if (_animalModels.find(id) != _animalModels.end()) return _animalModels[id];
    return nullptr;
}

FishModel* GameData::getFishModel(const std::string& id) {
    if (_fishModels.find(id) != _fishModels.end()) return _fishModels[id];
    return nullptr;
}