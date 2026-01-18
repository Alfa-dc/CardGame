// LevelConfig.h
// 关卡配置类 - 负责存储和管理关卡配置信息
//
// 这个类是关卡配置的核心管理类，负责：
// 1. 从配置文件加载关卡配置
// 2. 存储桌面卡牌配置数据
// 3. 存储手牌配置数据
// 4. 提供访问关卡配置数据的方法
//
// 核心概念：
// - 桌面卡牌：显示在游戏桌面上的卡牌，玩家可以点击匹配
// - 手牌：玩家可以点击替换的卡牌，堆叠在紫色区域
// - 卡牌花色：卡牌的花色（梅花、方块、红心、黑桃）
// - 卡牌点数：卡牌的点数（A、2-10、J、Q、K）
//
// 使用示例：
// LevelConfig levelConfig;
// levelConfig.loadLevelConfig("level_config.json");
// std::vector<CardConfig::CardData> playfieldCards = levelConfig.getPlayfieldCards();
// std::vector<CardConfig::CardData> stackCards = levelConfig.getStackCards();
#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include "cocos2d.h"
#include "CardConfig.h"

namespace configs {

// 卡牌花色类型枚举
enum class CardSuitType {
    CST_NONE = -1,
    CST_CLUBS,      // 梅花
    CST_DIAMONDS,   // 方块
    CST_HEARTS,     // 红心
    CST_SPADES,     // 黑桃
    CST_NUM_CARD_SUIT_TYPES
};

// 卡牌面类型枚举
enum class CardFaceType {
    CFT_NONE = -1,
    CFT_ACE,      // A
    CFT_TWO,      // 2
    CFT_THREE,    // 3
    CFT_FOUR,     // 4
    CFT_FIVE,     // 5
    CFT_SIX,      // 6
    CFT_SEVEN,    // 7
    CFT_EIGHT,    // 8
    CFT_NINE,     // 9
    CFT_TEN,      // 10
    CFT_JACK,     // J
    CFT_QUEEN,    // Q
    CFT_KING,     // K
    CFT_NUM_CARD_FACE_TYPES
};

// 关卡配置类
class LevelConfig {
public:
    // 构造函数 - 创建空的关卡配置
    LevelConfig();
    
    // 析构函数 - 清理关卡配置资源
    ~LevelConfig();
    
    // 加载关卡配置
    // 参数：levelConfigPath - 关卡配置文件路径
    // 返回：成功返回true，失败返回false
    bool loadLevelConfig(const std::string& levelConfigPath);
    
    // 获取桌面卡牌配置
    // 返回：桌面卡牌配置数据的向量
    const std::vector<CardConfig::CardData>& getPlayfieldCards() const { return _playfieldCards; }
    
    // 获取手牌配置
    // 返回：手牌配置数据的向量
    const std::vector<CardConfig::CardData>& getStackCards() const { return _stackCards; }
    
private:
    
    std::vector<CardConfig::CardData> _playfieldCards; // 桌面卡牌配置数据
    std::vector<CardConfig::CardData> _stackCards;     // 手牌配置数据
    int _nextCardId;                                   // 下一张卡牌的ID
};

} // namespace configs

#endif // __LEVEL_CONFIG_H__