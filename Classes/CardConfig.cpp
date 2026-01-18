// configs/models/CardConfig.cpp
#include "CardConfig.h"
#include <map>

USING_NS_CC;

namespace configs {

    // 单例实例
    CardConfig& CardConfig::getInstance() {
        static CardConfig instance;
        return instance;
    }

    bool CardConfig::loadConfig(const std::string& configPath) {
        // 这里可以加载JSON配置文件
        // 暂时使用默认配置
        return true;
    }

    std::string CardConfig::getSuitString(CardSuit suit) {
        static std::map<CardSuit, std::string> suitMap = {
            {CardSuit::HEART,   "?"},
            {CardSuit::DIAMOND, "?"},
            {CardSuit::CLUB,    "?"},
            {CardSuit::SPADE,   "?"}
        };

        auto it = suitMap.find(suit);
        if (it != suitMap.end()) {
            return it->second;
        }
        return "?";
    }

    std::string CardConfig::getRankString(int rank) {
        static std::map<int, std::string> rankMap = {
            {1,  "A"},
            {2,  "2"},
            {3,  "3"},
            {4,  "4"},
            {5,  "5"},
            {6,  "6"},
            {7,  "7"},
            {8,  "8"},
            {9,  "9"},
            {10, "10"},
            {11, "J"},
            {12, "Q"},
            {13, "K"}
        };

        auto it = rankMap.find(rank);
        if (it != rankMap.end()) {
            return it->second;
        }
        return std::to_string(rank);
    }

    Color4B CardConfig::getSuitColor(CardSuit suit) {
        // 红心和方块为红色，梅花和黑桃为黑色
        static CardDisplayConfig defaultConfig;

        if (suit == CardSuit::HEART || suit == CardSuit::DIAMOND) {
            return defaultConfig.redColor;
        }
        else {
            return defaultConfig.blackColor;
        }
    }

} // namespace configs