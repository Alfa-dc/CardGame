  // GameModel.h
// 游戏模型类 - 负责管理游戏状态和卡牌数据
// 
// 这个类是整个游戏的核心数据管理器，负责：
// 1. 存储和管理所有卡牌对象
// 2. 维护手牌和桌面牌的分类
// 3. 跟踪顶手牌的状态
// 4. 支持序列化和反序列化，用于撤销/重做功能
//
// 核心概念：
// - 手牌(HAND_CARD): 玩家可以点击替换的牌，堆叠在紫色区域
// - 桌面牌(TABLE_CARD): 显示在橙色区域的牌，用于匹配
// - 顶手牌: 手牌堆最上面的一张，可以用来匹配桌面牌
// - 手牌顺序: 手牌的堆叠顺序，从下到上排列
//
// 使用示例：
// GameModel* gameModel = new GameModel();
// CardModel* card = CardModel::createFromConfig(configData);
// gameModel->addCard(card);
// CardModel* topCard = gameModel->getTopHandCard();
//
#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "cocos2d.h"
#include <unordered_map>
#include <vector>
#include "CardModel.h"

namespace models {

class GameModel {
public:
    // 构造函数 - 创建空的游戏模型
    GameModel();
    
    // 析构函数 - 清理所有卡牌资源
    ~GameModel();

    // ========== 卡牌管理方法 ==========
    
    // 添加卡牌到游戏中
    // 参数：cardModel - 要添加的卡牌对象
    // 
    // 功能说明：
    // - 将卡牌添加到卡牌映射表中
    // - 如果是手牌，添加到手牌顺序列表
    // - 如果是第一张手牌，自动设置为顶手牌
    //
    // 使用场景：
    // - 初始化游戏时从配置加载卡牌
    // - 从序列化数据恢复游戏状态
    void addCard(CardModel* cardModel);
    
    // 从游戏中移除卡牌
    // 参数：cardId - 要移除的卡牌ID
    //
    // 功能说明：
    // - 从卡牌映射表中删除卡牌
    // - 如果是手牌，从手牌顺序列表中移除
    // - 如果移除的是顶手牌，自动设置新的顶手牌
    // - 释放卡牌对象的内存
    //
    // 使用场景：
    // - 卡牌被匹配后从游戏中移除
    // - 重置游戏时清空所有卡牌
    void removeCard(int cardId);
    
    // 根据ID获取卡牌
    // 参数：cardId - 要查找的卡牌ID
    // 返回：找到的卡牌对象指针，未找到返回nullptr
    //
    // 使用场景：
    // - 查找特定卡牌进行操作
    // - 检查卡牌是否存在
    CardModel* getCardById(int cardId) const;
    
    // ========== 卡牌查询方法 ==========
    
    // 获取所有卡牌
    // 返回：包含所有卡牌的映射表（ID -> CardModel指针）
    //
    // 使用场景：
    // - 遍历所有卡牌进行批量操作
    // - 序列化游戏状态
    const std::unordered_map<int, CardModel*>& getAllCards() const { return _cards; }
    
    // 获取手牌列表
    // 返回：手牌对象的向量，按照手牌顺序排列
    //
    // 功能说明：
    // - 根据手牌顺序ID列表查找对应的卡牌对象
    // - 返回的列表顺序与手牌堆叠顺序一致
    //
    // 使用场景：
    // - 显示手牌堆叠效果
    // - 处理手牌点击事件
    std::vector<CardModel*> getHandCards() const;
    
    // 获取桌面卡牌列表
    // 返回：桌面牌对象的向量
    //
    // 功能说明：
    // - 遍历所有卡牌，筛选出类型为TABLE_CARD的卡牌
    // - 桌面牌的顺序不重要，所以直接返回所有桌面牌
    //
    // 使用场景：
    // - 显示桌面卡牌
    // - 检查卡牌匹配规则
    std::vector<CardModel*> getTableCards() const;
    
    // ========== 顶手牌管理方法 ==========
    
    // 获取顶手牌ID
    // 返回：当前顶手牌的ID，如果没有手牌返回-1
    //
    // 使用场景：
    // - 快速获取顶手牌ID而不需要获取完整对象
    int getTopHandCardId() const { return _topHandCardId; }
    
    // 设置顶手牌ID
    // 参数：cardId - 要设置为顶手牌的卡牌ID
    //
    // 功能说明：
    // - 清除当前顶手牌的状态
    // - 如果指定卡牌是桌面牌，将其转换为手牌
    // - 将指定卡牌移动到手牌顺序列表的末尾
    // - 设置指定卡牌为新的顶手牌
    //
    // 使用场景：
    // - 玩家点击手牌时，将其设置为顶手牌
    // - 卡牌匹配成功后，桌面牌成为新的顶手牌
    void setTopHandCardId(int cardId);
    
    // 获取手牌顺序
    // 返回：手牌ID的向量，从下到上排列
    //
    // 使用场景：
    // - 了解手牌的堆叠顺序
    // - 在UI中正确显示手牌堆叠效果
    const std::vector<int>& getHandCardOrder() const { return _handCardOrder; }
    
    // 获取顶手牌
    // 返回：顶手牌对象指针，如果没有手牌返回nullptr
    //
    // 使用场景：
    // - 获取顶手牌的完整信息
    // - 检查顶手牌是否可以与桌面牌匹配
    CardModel* getTopHandCard() const;
    
    // ========== 游戏状态管理方法 ==========
    
    // 重置游戏状态
    //
    // 功能说明：
    // - 删除所有卡牌对象并释放内存
    // - 清空手牌顺序列表
    // - 重置顶手牌ID为-1
    //
    // 使用场景：
    // - 开始新游戏前重置状态
    // - 重新加载关卡时清理旧数据
    void reset();
    
    // ========== 序列化方法 ==========
    
    // 序列化游戏状态
    // 返回：包含游戏所有状态的ValueMap对象
    //
    // 序列化的数据包括：
    // - topHandCardId: 当前顶手牌ID
    // - handOrder: 手牌顺序ID列表
    // - cards: 所有卡牌的序列化数据
    //
    // 使用场景：
    // - 保存游戏进度
    // - 执行操作前保存状态（用于撤销）
    cocos2d::ValueMap serialize() const;
    
    // 反序列化游戏状态
    // 参数：data - 包含游戏状态的ValueMap对象
    // 返回：成功返回true，失败返回false
    //
    // 功能说明：
    // - 先重置当前游戏状态
    // - 从数据中恢复顶手牌ID
    // - 恢复手牌顺序列表
    // - 为每张卡牌创建对象并恢复其状态
    //
    // 使用场景：
    // - 加载保存的游戏进度
    // - 撤销操作时恢复之前的状态
    bool deserialize(const cocos2d::ValueMap& data);
    
private:
    // ========== 私有成员变量 ==========
    
    std::unordered_map<int, CardModel*> _cards; // 所有卡牌 - 使用ID作为键的映射表
    int _topHandCardId;                        // 当前顶手牌ID - -1表示没有手牌
    std::vector<int> _handCardOrder;           // 手牌顺序列表 - 从下到上排列的ID列表
};

} // namespace models

#endif // __GAME_MODEL_H__
