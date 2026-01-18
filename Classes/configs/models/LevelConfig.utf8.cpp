// LevelConfig.cpp
// ????????????
#include "LevelConfig.h"
#include "CardConfig.h"
#include "json/document.h"

namespace configs {

LevelConfig::LevelConfig() : _nextCardId(1) {
}

LevelConfig::~LevelConfig() {
}

bool LevelConfig::loadLevelConfig(const std::string& levelConfigPath) {
    // ????????
    _playfieldCards.clear();
    _stackCards.clear();
    _nextCardId = 1;
    
    // ????JSON???
    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(levelConfigPath);
    if (data.getSize() == 0) {
        CCLOG("Failed to load level config file: %s", levelConfigPath.c_str());
        return false;
    }
    
    // ????JSON????
    std::string jsonContent(reinterpret_cast<const char*>(data.getBytes()), data.getSize());
    rapidjson::Document document;
    document.Parse<0>(jsonContent.c_str());
    
    if (document.HasParseError()) {
        CCLOG("Failed to parse level config file: %s, error code: %d", 
              levelConfigPath.c_str(), document.GetParseError());
        return false;
    }
    
    // ??????????????????
    if (document.HasMember("Playfield") && document["Playfield"].IsArray()) {
        const rapidjson::Value& playfield = document["Playfield"];
        for (rapidjson::SizeType i = 0; i < playfield.Size(); ++i) {
            const rapidjson::Value& cardJson = playfield[i];
            CardConfig::CardData cardData;
            
            // ????ID
            cardData.id = _nextCardId++;
            
            // ???????
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
            
            // ????????
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
            
            // ?????????????????????
            // ???????????2??????6??,??????????
            int column = i / 3; // 0??1????2??
            int row = i % 3; // 0, 1, 2????3??
            
            // ?????????????????1080/2 = 540.0f
            float centerX = 540.0f;
            float rowY = 1800.0f - row * 120.0f; // 1800, 1680, 1560
            
            // ??????????50px?
            float offsetX = row * 50.0f; // 0, 50, 100
            
            // ??????????,???????????,?????????
            if (column == 0) {
                // ???,????????????
                cardData.position.x = centerX - 250.0f + offsetX;
            } else {
                // ???,??????????
                cardData.position.x = centerX + 250.0f - offsetX;
            }
            
            // ???????????
            cardData.position.y = rowY;
            
            cardData.originalPos = cardData.position;
            cardData.textureName = CardConfig::getTextureName(cardData.suit, cardData.rank);
            cardData.type = CardConfig::CardType::TABLE_CARD;
            
            _playfieldCards.push_back(cardData);
        }
    }
    
    // ??????????????????
    if (document.HasMember("Stack") && document["Stack"].IsArray()) {
        const rapidjson::Value& stack = document["Stack"];
        for (rapidjson::SizeType i = 0; i < stack.Size(); ++i) {
            const rapidjson::Value& cardJson = stack[i];
            CardConfig::CardData cardData;
            
            // ????ID
            cardData.id = _nextCardId++;
            
            // ???????
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
            
            // ????????
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
            
            // ??????????????????????
            // ?????????????????????????
            if (i == 0) {
                // ???????
                cardData.position.x = 200.0f;
                cardData.position.y = 290.0f;
            } else if (i == 1) {
                // ???????????
                cardData.position.x = 270.0f;
                cardData.position.y = 290.0f;
            } else if (i == 2) {
                // ??????????
                cardData.position.x = 600.0f;
                cardData.position.y = 290.0f;
            }
            
            cardData.originalPos = cardData.position;
            cardData.textureName = CardConfig::getTextureName(cardData.suit, cardData.rank);
            cardData.type = CardConfig::CardType::HAND_CARD;
            
            _stackCards.push_back(cardData);
        }
    }
    
    return true;
}



} // namespace configs
