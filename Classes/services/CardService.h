// CardService.h
// 卡牌服务 - 负责卡牌相关的业务逻辑和工具方法
// 
// 这个类提供卡牌相关的各种服务，包括：
// 1. 从配置创建卡牌模型
// 2. 检查卡牌是否可以匹配
// 3. 获取卡牌的纹理名称
// 4. 获取花色和点数的字符串表示
//
// 核心概念：
// - 单例模式：CardService是单例，全局唯一
// - 业务逻辑：封装卡牌相关的复杂逻辑
// - 工具方法：提供常用的卡牌操作方法
// - 配置管理：从配置数据创建卡牌模型
//
// 使用示例：
// CardService* service = CardService::getInstance();
// service->init();
// CardModel* card = service->createCardModelFromConfig(configData);
// bool canMatch = service->isCardsMatchable(tableCard, handCard);
//
#ifndef __CARD_SERVICE_H__
#define __CARD_SERVICE_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include "configs/models/CardConfig.h"

namespace services {

class CardService {
public:
    // ========== 单例模式方法 ==========
    
    // 获取单例实例
    // 返回：CardService的单例对象
    //
    // 使用场景：
    // - 获取CardService实例进行操作
    static CardService* getInstance();
    
    // 销毁单例实例
    //
    // 使用场景：
    // - 游戏退出时清理资源
    static void destroyInstance();
    
    // ========== 初始化方法 ==========
    
    // 初始化卡牌服务
    // 返回：成功返回true，失败返回false
    //
    // 使用场景：
    // - 游戏启动时调用
    bool init();
    
    // ========== 卡牌创建方法 ==========
    
    // 从配置创建卡牌模型
    // 参数：configData - 卡牌配置数据
    // 返回：创建的卡牌模型，失败返回nullptr
    //
    // 工作流程：
    // 1. 使用CardModel的工厂方法创建卡牌
    // 2. 设置卡牌的所有属性
    // 3. 返回创建的卡牌模型
    //
    // 使用场景：
    // - 从配置文件加载卡牌时使用
    // - 从序列化数据恢复卡牌时使用
    models::CardModel* createCardModelFromConfig(const configs::CardConfig::CardData& configData);
    
    // ========== 卡牌匹配方法 ==========
    
    // 检查两张卡牌是否可以匹配
    // 参数：
    //   tableCard - 桌面牌
    //   handCard - 手牌
    // 返回：可以匹配返回true，不能匹配返回false
    //
    // 匹配规则：
    // - 两张牌的花色相同
    // - 两张牌的点数相同
    //
    // 使用场景：
    // - 检查卡牌是否可以匹配
    bool isCardsMatchable(const models::CardModel* tableCard, const models::CardModel* handCard);
    
    // 检查两个点数是否可以匹配
    // 参数：
    //   tableRank - 桌面牌点数
    //   handRank - 手牌点数
    // 返回：可以匹配返回true，不能匹配返回false
    //
    // 匹配规则：
    // - 两个点数相同
    //
    // 使用场景：
    // - 快速检查点数是否匹配
    bool isCardsMatchableByRank(int tableRank, int handRank);
    
    // ========== 纹理名称方法 ==========
    
    // 获取卡牌纹理名称
    // 参数：
    //   suit - 卡牌花色
    //   rank - 卡牌点数
    // 返回：卡牌纹理的文件名
    //
    // 使用场景：
    // - 设置卡牌的纹理
    std::string getCardTextureName(const configs::CardConfig::Suit& suit, int rank);
    
    // ========== 字符串表示方法 ==========
    
    // 获取花色的字符串表示
    // 参数：suit - 卡牌花色
    // 返回：花色的中文名称
    //
    // 返回值：
    // - HEART: "红心"
    // - DIAMOND: "方块"
    // - CLUB: "梅花"
    // - SPADE: "黑桃"
    //
    // 使用场景：
    // - 显示花色名称
    std::string getSuitString(const configs::CardConfig::Suit& suit);
    
    // 获取点数的字符串表示
    // 参数：rank - 卡牌点数
    // 返回：点数的字符串表示
    //
    // 返回值：
    // - 1: "A"
    // - 11: "J"
    // - 12: "Q"
    // - 13: "K"
    // - 其他: 数字字符串
    //
    // 使用场景：
    // - 显示点数名称
    std::string getRankString(int rank);
    
private:
    // ========== 私有构造函数 ==========
    
    // 构造函数
    // 私有构造函数，实现单例模式
    CardService();
    
    // 析构函数
    ~CardService();
    
    // ========== 私有成员变量 ==========
    
    // 单例实例
    static CardService* _instance;  // CardService的单例对象
};

} // namespace services

#endif // __CARD_SERVICE_H__
