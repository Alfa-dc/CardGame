// LevelConfig.cpp
// 关卡配置类实现
#pragma execution_character_set("utf-8")
#include "LevelConfig.h"
#include "CardConfig.h"
#include "json/document.h"

namespace configs {

LevelConfig::LevelConfig() : _nextCardId(1) {
}

LevelConfig::~LevelConfig() {
}

bool LevelConfig::loadLevelConfig(const std::string& levelConfigPath) {
    // 重置配置数据
    _playfieldCards.clear();
    _stackCards.clear();
    _nextCardId = 1;
    
    // 读取JSON配置文件
    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(levelConfigPath);
    if (data.getSize() == 0) {
        CCLOG("Failed to load level config file: %s", levelConfigPath.c_str());
        return false;
    }
    
    // 解析JSON数据
    rapidjson::Document document;
    std::string content(reinterpret_cast<const char*>(data.getBytes()), data.getSize());
    document.Parse(content.c_str());
    
    if (document.HasParseError()) {
        CCLOG("Failed to parse level config file: %s", levelConfigPath.c_str());
        return false;
    }
    
    // 处理Playfield卡片配置
    if (document.HasMember("Playfield") && document["Playfield"].IsArray()) {
        const rapidjson::Value& playfield = document["Playfield"];
        for (rapidjson::SizeType i = 0; i < playfield.Size(); ++i) {
            const rapidjson::Value& cardJson = playfield[i];
            CardConfig::CardData cardData;
            
            // 生成唯一ID
            cardData.id = _nextCardId++;
            
            // 设置花色
            if (cardJson.HasMember("CardSuit") && cardJson["CardSuit"].IsInt()) {
                int suit = cardJson["CardSuit"].GetInt();
                switch (static_cast<CardSuitType>(suit)) {
                    case CardSuitType::CST_CLUBS: cardData.suit = CardConfig::Suit::CLUB; break;
                    case CardSuitType::CST_DIAMONDS: cardData.suit = CardConfig::Suit::DIAMOND; break;
                    case CardSuitType::CST_HEARTS: cardData.suit = CardConfig::Suit::HEART; break;
                    case CardSuitType::CST_SPADES: cardData.suit = CardConfig::Suit::SPADE; break;
                    default: cardData.suit = CardConfig::Suit::HEART; break;
                }
            }
            
            // 设置牌面
            if (cardJson.HasMember("CardFace") && cardJson["CardFace"].IsInt()) {
                int face = cardJson["CardFace"].GetInt();
                switch (static_cast<CardFaceType>(face)) {
                    case CardFaceType::CFT_ACE: cardData.rank = 1; break;
                    case CardFaceType::CFT_TWO: cardData.rank = 2; break;
                    case CardFaceType::CFT_THREE: cardData.rank = 3; break;
                    case CardFaceType::CFT_FOUR: cardData.rank = 4; break;
                    case CardFaceType::CFT_FIVE: cardData.rank = 5; break;
                    case CardFaceType::CFT_SIX: cardData.rank = 6; break;
                    case CardFaceType::CFT_SEVEN: cardData.rank = 7; break;
                    case CardFaceType::CFT_EIGHT: cardData.rank = 8; break;
                    case CardFaceType::CFT_NINE: cardData.rank = 9; break;
                    case CardFaceType::CFT_TEN: cardData.rank = 10; break;
                    case CardFaceType::CFT_JACK: cardData.rank = 11; break;
                    case CardFaceType::CFT_QUEEN: cardData.rank = 12; break;
                    case CardFaceType::CFT_KING: cardData.rank = 13; break;
                    default: cardData.rank = 1; break;
                }
            }
            
            // 设置卡片位置和属性
            // 每列3行，共2列6张牌，采用交错布局
            int column = i / 3; // 0表示左列，1表示右列
            int row = i % 3; // 0, 1, 2 表示每行
            
            // 屏幕中心x坐标为1080/2 = 540.0f
            float centerX = 540.0f;
            float rowY = 1800.0f - row * 120.0f; // 1800, 1680, 1560
            
            // 每行卡片向右偏移50px，形成交错效果
            float offsetX = row * 50.0f; // 0, 50, 100
            
            // 根据列设置x坐标，左列从左到右，右列从右到左
            if (column == 0) {
                // 左列，x坐标从左到右递增
                cardData.position.x = centerX - 250.0f + offsetX;
            } else {
                // 右列，x坐标从右到左递减
                cardData.position.x = centerX + 250.0f - offsetX;
            }
            
            // 设置y坐标
            cardData.position.y = rowY;
            
            cardData.originalPos = cardData.position;
            cardData.textureName = CardConfig::getTextureName(cardData.suit, cardData.rank);
            cardData.type = CardConfig::CardType::TABLE_CARD;
            
            _playfieldCards.push_back(cardData);
        }
    }
    
    // 处理堆叠卡片配置
    if (document.HasMember("Stack") && document["Stack"].IsArray()) {
        const rapidjson::Value& stack = document["Stack"];
        for (rapidjson::SizeType i = 0; i < stack.Size(); ++i) {
            const rapidjson::Value& cardJson = stack[i];
            CardConfig::CardData cardData;
            
            // 生成唯一ID
            cardData.id = _nextCardId++;
            
            // 设置花色
            if (cardJson.HasMember("CardSuit") && cardJson["CardSuit"].IsInt()) {
                int suit = cardJson["CardSuit"].GetInt();
                switch (static_cast<CardSuitType>(suit)) {
                    case CardSuitType::CST_CLUBS: cardData.suit = CardConfig::Suit::CLUB; break;
                    case CardSuitType::CST_DIAMONDS: cardData.suit = CardConfig::Suit::DIAMOND; break;
                    case CardSuitType::CST_HEARTS: cardData.suit = CardConfig::Suit::HEART; break;
                    case CardSuitType::CST_SPADES: cardData.suit = CardConfig::Suit::SPADE; break;
                    default: cardData.suit = CardConfig::Suit::HEART; break;
                }
            }
            
            // 设置牌面
            if (cardJson.HasMember("CardFace") && cardJson["CardFace"].IsInt()) {
                int face = cardJson["CardFace"].GetInt();
                switch (static_cast<CardFaceType>(face)) {
                    case CardFaceType::CFT_ACE: cardData.rank = 1; break;
                    case CardFaceType::CFT_TWO: cardData.rank = 2; break;
                    case CardFaceType::CFT_THREE: cardData.rank = 3; break;
                    case CardFaceType::CFT_FOUR: cardData.rank = 4; break;
                    case CardFaceType::CFT_FIVE: cardData.rank = 5; break;
                    case CardFaceType::CFT_SIX: cardData.rank = 6; break;
                    case CardFaceType::CFT_SEVEN: cardData.rank = 7; break;
                    case CardFaceType::CFT_EIGHT: cardData.rank = 8; break;
                    case CardFaceType::CFT_NINE: cardData.rank = 9; break;
                    case CardFaceType::CFT_TEN: cardData.rank = 10; break;
                    case CardFaceType::CFT_JACK: cardData.rank = 11; break;
                    case CardFaceType::CFT_QUEEN: cardData.rank = 12; break;
                    case CardFaceType::CFT_KING: cardData.rank = 13; break;
                    default: cardData.rank = 1; break;
                }
            }
            
            // 设置堆叠卡片位置（紫色区域）
            // 紫色区域高度为580.0f，所以y坐标应在0-580之间
            if (i == stack.Size() - 1) {
                // 最后一张牌作为手牌，放在右边
                cardData.position.x = 800.0f; // 右边位置
                cardData.position.y = 290.0f; // 垂直居中
                cardData.type = CardConfig::CardType::HAND_CARD;
            } else {
                // 其他牌在左边稍微错开堆叠
                cardData.position.x = 200.0f + i * 20.0f; // 左边堆叠，每张卡片向右偏移20px
                cardData.position.y = 290.0f; // 垂直居中
                cardData.type = CardConfig::CardType::HAND_CARD;
            }
            cardData.originalPos = cardData.position;
            cardData.textureName = CardConfig::getTextureName(cardData.suit, cardData.rank);
            
            _stackCards.push_back(cardData);
        }
    }
    
    return true;
}



} // namespace configs