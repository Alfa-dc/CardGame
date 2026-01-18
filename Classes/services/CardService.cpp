// CardService.cpp
// 卡牌服务实现文件 - 提供卡牌相关的工具方法
#pragma execution_character_set("utf-8")
#include "CardService.h"

namespace services {

// 单例实例指针
CardService* CardService::_instance = nullptr;

// 获取单例实例
// 返回：卡牌服务的单例实例
CardService* CardService::getInstance() {
    if (!_instance) {
        _instance = new CardService();
        _instance->init();
    }
    return _instance;
}

// 销毁单例实例
void CardService::destroyInstance() {
    CC_SAFE_DELETE(_instance);
}

// 构造函数
CardService::CardService() {
}

// 析构函数
CardService::~CardService() {
}

// 初始化卡牌服务
// 返回：成功返回true，失败返回false
bool CardService::init() {
    return true;
}

// 根据配置数据创建卡牌模型
// 参数：configData - 卡牌配置数据
// 返回：创建的卡牌模型，失败返回nullptr
models::CardModel* CardService::createCardModelFromConfig(const configs::CardConfig::CardData& configData) {
    return models::CardModel::createFromConfig(configData);
}

// 检查两张卡牌是否可以匹配
// 参数：
//   tableCard - 桌面卡牌
//   handCard - 手牌
// 返回：可以匹配返回true，不能匹配返回false
bool CardService::isCardsMatchable(const models::CardModel* tableCard, const models::CardModel* handCard) {
    if (!tableCard || !handCard) {
        return false;
    }
    
    return isCardsMatchableByRank(tableCard->getRank(), handCard->getRank());
}

// 根据点数检查两张卡牌是否可以匹配
// 参数：
//   tableRank - 桌面卡牌的点数
//   handRank - 手牌的点数
// 返回：可以匹配返回true，不能匹配返回false
//
// 匹配规则：两张牌的点数相差1
// 例如：3和4可以匹配，5和5不能匹配
bool CardService::isCardsMatchableByRank(int tableRank, int handRank) {
    // 计算点数差值
    int diff = abs(tableRank - handRank);
    // 点数差值为1则可以匹配
    return diff == 1;
}

// 获取卡牌纹理名称
// 参数：
//   suit - 花色
//   rank - 点数
// 返回：纹理名称
std::string CardService::getCardTextureName(const configs::CardConfig::Suit& suit, int rank) {
    return configs::CardConfig::getTextureName(suit, rank);
}

// 获取花色字符串
// 参数：suit - 花色
// 返回：花色的字符串表示
std::string CardService::getSuitString(const configs::CardConfig::Suit& suit) {
    return configs::CardConfig::getSuitString(suit);
}

// 获取点数字符串
// 参数：rank - 点数
// 返回：点数的字符串表示
std::string CardService::getRankString(int rank) {
    return configs::CardConfig::getRankString(rank);
}

} // namespace services
