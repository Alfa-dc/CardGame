// GameModelFromLevelGenerator.cpp
// 游戏模型生成器实现文件 - 根据关卡配置生成游戏模型
#pragma execution_character_set("utf-8")
#include "GameModelFromLevelGenerator.h"
#include "models/CardModel.h"

namespace generators {

// 单例实例指针初始化
GameModelFromLevelGenerator* GameModelFromLevelGenerator::_instance = nullptr;

// 构造函数
GameModelFromLevelGenerator::GameModelFromLevelGenerator() {
    // 初始化生成器
}

// 析构函数
GameModelFromLevelGenerator::~GameModelFromLevelGenerator() {
    // 清理资源
}

// 获取单例实例
// 返回：生成器的单例实例
GameModelFromLevelGenerator* GameModelFromLevelGenerator::getInstance() {
    if (!_instance) {
        _instance = new GameModelFromLevelGenerator();
    }
    return _instance;
}

// 销毁单例实例
void GameModelFromLevelGenerator::destroyInstance() {
    CC_SAFE_DELETE(_instance);
    _instance = nullptr;
}

// 根据关卡配置生成游戏模型
// 参数：levelConfig - 关卡配置对象
// 返回：生成的游戏模型
models::GameModel* GameModelFromLevelGenerator::generateGameModel(const configs::LevelConfig& levelConfig) {
    // 创建新的游戏模型
    models::GameModel* gameModel = new models::GameModel();
    
    // 从关卡配置中获取桌面卡牌和手牌数据
    auto playfieldCards = levelConfig.getPlayfieldCards();
    auto stackCards = levelConfig.getStackCards();
    
    // 合并所有卡牌数据到一个列表中
    std::vector<configs::CardConfig::CardData> allCards;
    allCards.insert(allCards.end(), playfieldCards.begin(), playfieldCards.end());
    allCards.insert(allCards.end(), stackCards.begin(), stackCards.end());
    
    // 为每张卡牌创建卡牌模型并添加到游戏模型中
    for (const auto& cardData : allCards) {
        models::CardModel* cardModel = createCardModelFromConfig(cardData);
        if (cardModel) {
            gameModel->addCard(cardModel);
        }
    }
    
    // 设置初始顶手牌ID（如果有手牌，则设置最后一张手牌为顶手牌）
    if (!stackCards.empty()) {
        // 如果手牌数量大于等于3，则设置第3张手牌的ID为顶手牌ID
        if (stackCards.size() >= 3) {
            // 设置第3张手牌的ID为顶手牌ID
            gameModel->setTopHandCardId(stackCards[2].id);
        } else if (!stackCards.empty()) {
            // 如果手牌数量不足3张，则设置第一张手牌的ID为顶手牌ID
            gameModel->setTopHandCardId(stackCards[0].id);
        }
    }
    
    return gameModel;
}

// 根据卡牌配置创建卡牌模型
// 参数：cardData - 卡牌配置数据
// 返回：创建的卡牌模型
models::CardModel* GameModelFromLevelGenerator::createCardModelFromConfig(const configs::CardConfig::CardData& cardData) {
    // 使用CardModel的静态方法从配置数据创建卡牌模型
    return models::CardModel::createFromConfig(cardData);
}

} // namespace generators
