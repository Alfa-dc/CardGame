// CardConfig.h
// 卡牌配置类 - 负责卡牌相关的配置和工具方法
//
// 这个类是卡牌相关的配置中心，负责：
// 1. 定义卡牌的花色、类型等枚举
// 2. 提供卡牌数据结构
// 3. 提供卡牌纹理路径的获取方法
// 4. 提供卡牌测试数据的创建方法
//
// 核心概念：
// - Suit: 卡牌花色枚举（红心、方块、梅花、黑桃）
// - CardType: 卡牌类型枚举（手牌、桌面牌、未知）
// - CardData: 卡牌数据结构，包含卡牌的所有属性
//
// 使用示例：
// std::string suitPath = CardConfig::getSuitTexturePath(CardConfig::Suit::HEART);
// std::string numberPath = CardConfig::getBigNumberTexturePath(CardConfig::Suit::DIAMOND, 5);
// std::vector<CardConfig::CardData> testData = CardConfig::createTestData();
#ifndef __CARD_CONFIG_H__
#define __CARD_CONFIG_H__

#include "cocos2d.h"

namespace configs {

class CardConfig {
public:
    // 卡牌花色枚举
    enum class Suit {  
        HEART,      // 红心
        DIAMOND,    // 方块
        CLUB,       // 梅花
        SPADE       // 黑桃
    };

    // 卡牌类型枚举
    enum class CardType {
        HAND_CARD,      // 手牌
        TABLE_CARD,     // 桌面牌
        UNKNOWN         // 未知类型
    };

    // 卡牌数据结构
    struct CardData {
        int id;                     // 卡牌ID
        Suit suit;                  // 卡牌花色
        int rank;                   // 卡牌点数 (1-13, 1=A, 11=J, 12=Q, 13=K)
        CardType type;              // 卡牌类型
        cocos2d::Vec2 position;     // 卡牌位置
        cocos2d::Vec2 originalPos;  // 卡牌原始位置，用于重置
        std::string textureName;    // 卡牌纹理名称

        CardData() : id(0), suit(Suit::HEART), rank(1),
            type(CardType::HAND_CARD) {
        }
        
        CardData(int _id, Suit _suit, int _rank, CardType _type,
                cocos2d::Vec2 _position, cocos2d::Vec2 _originalPos,
                const std::string& _textureName) :
            id(_id), suit(_suit), rank(_rank), type(_type),
            position(_position), originalPos(_originalPos),
            textureName(_textureName) {
        }
    };

    // 获取花色字符串表示
    static std::string getSuitString(Suit suit) {
        switch (suit) {
        case Suit::HEART: return "heart";
        case Suit::DIAMOND: return "diamond";
        case Suit::CLUB: return "club";
        case Suit::SPADE: return "spade";
        default: return "unknown";
        }
    }

    // 获取点数字符串表示
    static std::string getRankString(int rank) {
        switch (rank) {
        case 1: return "A";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        default: return std::to_string(rank);
        }
    }

    // 获取卡牌纹理名称
    static std::string getTextureName(Suit suit, int rank) {
        // 所有卡牌使用相同的通用纹理
        return "res/card_general.png";
    }
    
    // 获取花色纹理路径
    static std::string getSuitTexturePath(Suit suit) {
        switch (suit) {
        case Suit::HEART: return "res/suits/heart.png";
        case Suit::DIAMOND: return "res/suits/diamond.png";
        case Suit::CLUB: return "res/suits/club.png";
        case Suit::SPADE: return "res/suits/spade.png";
        default: return "res/suits/heart.png";
        }
    }
    
    // 获取大数字纹理路径（右下角）
    static std::string getBigNumberTexturePath(Suit suit, int rank) {
        // 根据花色确定数字颜色
        bool isRed = (suit == Suit::HEART || suit == Suit::DIAMOND);
        std::string colorPrefix = isRed ? "red" : "black";
        
        // 将点数转换为字符串
        std::string rankStr;
        switch (rank) {
        case 1: rankStr = "A";
            break;
        case 11: rankStr = "J";
            break;
        case 12: rankStr = "Q";
            break;
        case 13: rankStr = "K";
            break;
        default: rankStr = std::to_string(rank);
            break;
        }
        
        return "res/number/big_" + colorPrefix + "_" + rankStr + ".png";
    }
    
    // 获取小数字纹理路径（左上角）
    static std::string getSmallNumberTexturePath(Suit suit, int rank) {
        // 根据花色确定数字颜色
        bool isRed = (suit == Suit::HEART || suit == Suit::DIAMOND);
        std::string colorPrefix = isRed ? "red" : "black";
        
        // 将点数转换为字符串
        std::string rankStr;
        switch (rank) {
        case 1: rankStr = "A";
            break;
        case 11: rankStr = "J";
            break;
        case 12: rankStr = "Q";
            break;
        case 13: rankStr = "K";
            break;
        default: rankStr = std::to_string(rank);
            break;
        }
        
        return "res/number/small_" + colorPrefix + "_" + rankStr + ".png";
    }

    // 创建测试数据
    static std::vector<CardData> createTestData() {
        std::vector<CardData> cards;

        // 创建手牌（紫色区域）
        cards.push_back({ 1, Suit::HEART, 1, CardType::HAND_CARD,
                        cocos2d::Vec2(200, 200), cocos2d::Vec2(200, 200),
                        getTextureName(Suit::HEART, 1) });  // 红心A

        cards.push_back({ 2, Suit::CLUB, 4, CardType::HAND_CARD,
                        cocos2d::Vec2(300, 200), cocos2d::Vec2(300, 200),
                        getTextureName(Suit::CLUB, 4) });   // 梅花4

        cards.push_back({ 3, Suit::SPADE, 2, CardType::HAND_CARD,
                        cocos2d::Vec2(400, 200), cocos2d::Vec2(400, 200),
                        getTextureName(Suit::SPADE, 2) });  // 黑桃2

        // 创建桌面牌（棕色区域）
        cards.push_back({ 4, Suit::DIAMOND, 3, CardType::TABLE_CARD,
                        cocos2d::Vec2(300, 400), cocos2d::Vec2(300, 400),
                        getTextureName(Suit::DIAMOND, 3) }); // 方块3

        cards.push_back({ 5, Suit::HEART, 5, CardType::TABLE_CARD,
                        cocos2d::Vec2(400, 400), cocos2d::Vec2(400, 400),
                        getTextureName(Suit::HEART, 5) });   // 红心5

        cards.push_back({ 6, Suit::SPADE, 10, CardType::TABLE_CARD,
                        cocos2d::Vec2(500, 400), cocos2d::Vec2(500, 400),
                        getTextureName(Suit::SPADE, 10) });  // 黑桃10

        return cards;
    }
};

} // namespace configs

#endif // __CARD_CONFIG_H__
