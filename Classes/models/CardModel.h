// CardModel.h
// 卡牌模型类 - 负责存储卡牌的基本数据
// 
// 这个类是整个游戏的基础数据结构，表示一张扑克牌的所有属性
// 它不包含任何显示逻辑，只负责数据的存储和管理
// 
// 主要功能：
// 1. 存储卡牌的基本信息（ID、花色、点数、类型）
// 2. 管理卡牌的位置信息（当前位置和原始位置）
// 3. 控制卡牌的显示状态（是否可见、是否为顶手牌）
// 4. 支持序列化和反序列化，用于撤销/重做功能
// 
// 使用示例：
// CardModel* card = CardModel::createFromConfig(configData);
// card->setPosition(Vec2(100, 200));
// card->setIsTopCard(true);
//
#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "configs/models/CardConfig.h"

namespace models {

class CardModel {
public:
    // 默认构造函数 - 创建一个空卡牌
    CardModel();
    
    // 完整构造函数 - 创建指定属性的卡牌
    // 参数说明：
    // id: 卡牌的唯一标识符
    // suit: 卡牌花色（红心、方块、梅花、黑桃）
    // rank: 卡牌点数（1-13，其中1代表A，11-13代表JQK）
    // type: 卡牌类型（手牌或桌面牌）
    // position: 卡牌在屏幕上的初始位置
    CardModel(int id, const configs::CardConfig::Suit& suit, int rank,
             const configs::CardConfig::CardType& type, const cocos2d::Vec2& position);
    
    // 析构函数 - 清理卡牌资源
    ~CardModel();

    // ========== 属性访问器 ==========
    
    // 获取卡牌ID - 每张卡牌都有唯一的ID用于标识
    int getId() const { return _id; }
    
    // 获取/设置卡牌花色
    // 花色包括：HEART(红心)、DIAMOND(方块)、CLUB(梅花)、SPADE(黑桃)
    const configs::CardConfig::Suit& getSuit() const { return _suit; }
    void setSuit(const configs::CardConfig::Suit& suit) { _suit = suit; }
    
    // 获取/设置卡牌点数
    // 点数范围：1-13，其中1=A，2-10=数字牌，11=J，12=Q，13=K
    int getRank() const { return _rank; }
    void setRank(int rank) { _rank = rank; }
    
    // 获取/设置卡牌类型
    // 类型包括：HAND_CARD(手牌)、TABLE_CARD(桌面牌)、UNKNOWN(未知)
    const configs::CardConfig::CardType& getType() const { return _type; }
    void setType(const configs::CardConfig::CardType& type) { _type = type; }
    
    // 获取/设置卡牌位置
    // 位置是卡牌在屏幕上的坐标，用于显示和动画
    const cocos2d::Vec2& getPosition() const { return _position; }
    void setPosition(const cocos2d::Vec2& position) { _position = position; }
    
    // 获取/设置卡牌原始位置
    // 原始位置用于卡牌移动后恢复到初始位置
    const cocos2d::Vec2& getOriginalPosition() const { return _originalPosition; }
    void setOriginalPosition(const cocos2d::Vec2& position) { _originalPosition = position; }
    
    // 获取/设置卡牌纹理名称
    // 纹理名称是卡牌图片文件的路径，用于渲染显示
    const std::string& getTextureName() const { return _textureName; }
    void setTextureName(const std::string& textureName) { _textureName = textureName; }
    
    // 获取/设置是否为顶手牌
    // 顶手牌是手牌堆中最上面的一张，可以用来匹配桌面牌
    bool isTopCard() const { return _isTopCard; }
    void setIsTopCard(bool isTop) { _isTopCard = isTop; }
    
    // 获取/设置卡牌可见性
    // 可见性控制卡牌是否在屏幕上显示，被匹配的牌会设为不可见
    bool isVisible() const { return _isVisible; }
    void setVisible(bool visible) { _isVisible = visible; }
    
    // 获取/设置卡牌Z轴值（层级）
    // Z轴值控制卡牌的显示层级，值越大显示越靠前
    // 例如：桌面卡牌的zOrder为100-102，手牌的zOrder为50-52
    int getZOrder() const { return _zOrder; }
    void setZOrder(int zOrder) { _zOrder = zOrder; }
    
    // ========== 工厂方法 ==========
    
    // 从配置创建卡牌模型 - 静态工厂方法
    // 参数：configData - 包含卡牌配置数据的结构体
    // 返回：创建的CardModel指针，失败返回nullptr
    // 这个方法简化了卡牌的创建过程，直接从配置数据生成卡牌对象
    static CardModel* createFromConfig(const configs::CardConfig::CardData& configData);
    
    // ========== 序列化方法 ==========
    
    // 序列化卡牌数据 - 将卡牌状态转换为可存储的格式
    // 返回：包含卡牌所有属性的ValueMap
    // 用途：用于撤销/重做功能，保存卡牌的当前状态
    cocos2d::ValueMap serialize() const;
    
    // 反序列化卡牌数据 - 从存储的格式恢复卡牌状态
    // 参数：data - 包含卡牌数据的ValueMap
    // 返回：成功返回true，失败返回false
    // 用途：用于撤销/重做功能，将卡牌恢复到之前的状态
    bool deserialize(const cocos2d::ValueMap& data);
    
private:
    // ========== 私有成员变量 ==========
    
    int _id;                          // 卡牌ID - 唯一标识符
    configs::CardConfig::Suit _suit;  // 卡牌花色 - 红心、方块、梅花、黑桃
    int _rank;                        // 卡牌点数 - 1-13，对应A-K
    configs::CardConfig::CardType _type;  // 卡牌类型 - 手牌或桌面牌
    cocos2d::Vec2 _position;          // 卡牌位置 - 当前屏幕坐标
    cocos2d::Vec2 _originalPosition;  // 卡牌原始位置 - 初始坐标，用于恢复
    std::string _textureName;         // 卡牌纹理名称 - 图片文件路径
    bool _isTopCard;                  // 是否为顶手牌 - 手牌堆最上面的牌
    bool _isVisible;                  // 卡牌是否可见 - 控制显示状态
    int _zOrder;                      // 卡牌Z轴值 - 显示层级，值越大显示越靠前
};

} // namespace models

#endif // __CARD_MODEL_H__
