#pragma execution_character_set("utf-8")
// GameModel.cpp
// 游戏模型类实现
// 
// 这个文件实现了GameModel类的所有方法
// GameModel是整个游戏的核心数据管理器，负责管理所有卡牌和游戏状态
//
// 主要实现内容：
// 1. 构造函数和析构函数 - 创建和销毁游戏模型
// 2. 卡牌管理方法 - 添加、删除、查询卡牌
// 3. 顶手牌管理方法 - 设置和获取顶手牌
// 4. 游戏状态管理方法 - 重置游戏状态
// 5. 序列化方法 - 保存和恢复游戏状态
//
// 核心概念：
// - 手牌堆叠：手牌按照顺序堆叠，最上面的是顶手牌
// - 桌面牌：显示在桌面上的牌，用于匹配
// - 顶手牌：手牌堆最上面的一张，可以用来匹配桌面牌
// - 序列化：将游戏状态转换为可存储的格式，用于撤销/重做
#include "GameModel.h"
#include "configs/models/CardConfig.h"

namespace models {

// ========== 构造函数和析构函数 ==========

// 构造函数
// 创建一个空的游戏模型对象
// 
// 初始化状态：
// - _topHandCardId: -1 (表示没有手牌)
// - _cards: 空映射表 (没有卡牌)
// - _handCardOrder: 空列表 (没有手牌)
GameModel::GameModel() : _topHandCardId(-1) {
}

// 析构函数
// 清理游戏模型的所有资源
// 
// 功能说明：
// - 调用reset()方法删除所有卡牌对象
// - 释放所有动态分配的内存
GameModel::~GameModel() {
    reset();
}

// ========== 卡牌管理方法 ==========

// 添加卡牌到游戏中
// 参数：cardModel - 要添加的卡牌对象指针
//
// 工作流程：
// 1. 检查卡牌指针是否有效，无效则直接返回
// 2. 获取卡牌ID
// 3. 将卡牌添加到卡牌映射表中（使用ID作为键）
// 4. 如果是手牌，添加到手牌顺序列表的末尾
// 5. 如果是第一张手牌，自动设置为顶手牌
//
// 使用场景：
// - 初始化游戏时从配置加载卡牌
// - 从序列化数据恢复游戏状态
void GameModel::addCard(CardModel* cardModel) {
    // 检查卡牌指针是否有效
    if (!cardModel) return;
    
    // 获取卡牌ID并添加到映射表
    int cardId = cardModel->getId();
    _cards[cardId] = cardModel;
    
    // 如果是手牌，添加到手牌顺序列表
    if (cardModel->getType() == configs::CardConfig::CardType::HAND_CARD) {
        _handCardOrder.push_back(cardId);
        
        // 如果是第一张手牌，设置为顶手牌
        if (_topHandCardId == -1) {
            _topHandCardId = cardId;
            cardModel->setIsTopCard(true);
        }
    }
}

// 从游戏中移除卡牌
// 参数：cardId - 要移除的卡牌ID
//
// 工作流程：
// 1. 在卡牌映射表中查找指定ID的卡牌
// 2. 如果找到：
//    a. 如果是手牌，从手牌顺序列表中移除
//    b. 如果移除的是顶手牌，设置新的顶手牌（手牌列表的最后一个）
//    c. 删除卡牌对象并释放内存
//    d. 从映射表中移除卡牌
//
// 使用场景：
// - 卡牌被匹配后从游戏中移除
// - 重置游戏时清空所有卡牌
void GameModel::removeCard(int cardId) {
    // 在映射表中查找卡牌
    auto it = _cards.find(cardId);
    if (it != _cards.end()) {
        // 如果是手牌，从手牌顺序列表中移除
        auto orderIt = std::find(_handCardOrder.begin(), _handCardOrder.end(), cardId);
        if (orderIt != _handCardOrder.end()) {
            _handCardOrder.erase(orderIt);
            
            // 如果移除的是顶手牌，设置新的顶手牌
            if (_topHandCardId == cardId) {
                if (!_handCardOrder.empty()) {
                    // 新的顶手牌是手牌列表的最后一个
                    _topHandCardId = _handCardOrder.back();
                    CardModel* newTopCard = _cards[_topHandCardId];
                    if (newTopCard) {
                        newTopCard->setIsTopCard(true);
                    }
                } else {
                    // 没有手牌了，重置顶手牌ID
                    _topHandCardId = -1;
                }
            }
        }
        
        // 删除卡牌对象并从映射表中移除
        delete it->second;
        _cards.erase(it);
    }
}

// ========== 卡牌查询方法 ==========

// 根据ID获取卡牌
// 参数：cardId - 要查找的卡牌ID
// 返回：找到的卡牌对象指针，未找到返回nullptr
//
// 使用场景：
// - 查找特定卡牌进行操作
// - 检查卡牌是否存在
CardModel* GameModel::getCardById(int cardId) const {
    auto it = _cards.find(cardId);
    if (it != _cards.end()) {
        return it->second;
    }
    return nullptr;
}

// 获取手牌列表
// 返回：手牌对象的向量，按照手牌顺序排列
//
// 工作流程：
// 1. 创建一个空的向量
// 2. 遍历手牌顺序ID列表
// 3. 根据每个ID查找对应的卡牌对象
// 4. 将找到的卡牌添加到向量中
// 5. 返回向量
//
// 使用场景：
// - 显示手牌堆叠效果
// - 处理手牌点击事件
std::vector<CardModel*> GameModel::getHandCards() const {
    std::vector<CardModel*> handCards;
    // 遍历手牌顺序ID列表
    for (int cardId : _handCardOrder) {
        // 根据ID查找卡牌
        auto card = getCardById(cardId);
        if (card) {
            handCards.push_back(card);
        }
    }
    return handCards;
}

// 获取桌面卡牌列表
// 返回：桌面牌对象的向量
//
// 工作流程：
// 1. 创建一个空的向量
// 2. 遍历所有卡牌
// 3. 筛选出类型为TABLE_CARD的卡牌
// 4. 将桌面牌添加到向量中
// 5. 返回向量
//
// 使用场景：
// - 显示桌面卡牌
// - 检查卡牌匹配规则
std::vector<CardModel*> GameModel::getTableCards() const {
    std::vector<CardModel*> tableCards;
    // 遍历所有卡牌
    for (const auto& pair : _cards) {
        // 筛选桌面牌
        if (pair.second->getType() == configs::CardConfig::CardType::TABLE_CARD) {
            tableCards.push_back(pair.second);
        }
    }
    return tableCards;
}

// ========== 顶手牌管理方法 ==========

// 设置顶手牌ID
// 参数：cardId - 要设置为顶手牌的卡牌ID
//
// 工作流程：
// 1. 检查卡牌是否存在，不存在则直接返回
// 2. 清除当前顶手牌的状态
// 3. 如果指定卡牌是桌面牌，将其转换为手牌
// 4. 从手牌顺序列表中移除指定卡牌（如果存在）
// 5. 将指定卡牌添加到手牌顺序列表的末尾
// 6. 设置指定卡牌为新的顶手牌
//
// 使用场景：
// - 玩家点击手牌时，将其设置为顶手牌
// - 卡牌匹配成功后，桌面牌成为新的顶手牌
void GameModel::setTopHandCardId(int cardId) {
    // 检查卡牌是否存在
    CardModel* card = getCardById(cardId);
    if (!card) {
        return;
    }
    
    // 清除当前顶手牌状态
    CardModel* currentTop = getTopHandCard();
    if (currentTop) {
        currentTop->setIsTopCard(false);
    }
    
    // 如果是桌面卡牌，转换为手牌
    if (card->getType() == configs::CardConfig::CardType::TABLE_CARD) { 
        card->setType(configs::CardConfig::CardType::HAND_CARD);
    }
    
    // 从手牌顺序列表中移除当前卡牌
    auto orderIt = std::find(_handCardOrder.begin(), _handCardOrder.end(), cardId);  // 查找手牌顺序列表中是否存在该卡牌
    if (orderIt != _handCardOrder.end()) {
        _handCardOrder.erase(orderIt);
    }
    
    // 将卡牌添加到手牌顺序列表末尾作为新的顶手牌
    _handCardOrder.push_back(cardId);
    _topHandCardId = cardId;
    card->setIsTopCard(true);
}

// 获取顶手牌
// 返回：顶手牌对象指针，如果没有手牌返回nullptr
//
// 使用场景：
// - 获取顶手牌的完整信息
// - 检查顶手牌是否可以与桌面牌匹配
CardModel* GameModel::getTopHandCard() const {
    return getCardById(_topHandCardId);
}

// ========== 游戏状态管理方法 ==========

// 重置游戏状态
//
// 工作流程：
// 1. 遍历所有卡牌，删除每个卡牌对象并释放内存
// 2. 清空卡牌映射表
// 3. 清空手牌顺序列表
// 4. 重置顶手牌ID为-1
//
// 使用场景：
// - 开始新游戏前重置状态
// - 重新加载关卡时清理旧数据
// - 反序列化前清理当前状态
void GameModel::reset() {
    // 删除所有卡牌对象并释放内存
    for (auto& pair : _cards) {
        delete pair.second;
    }
    // 清空所有数据结构
    _cards.clear();
    _handCardOrder.clear();
    _topHandCardId = -1;
}

// ========== 序列化方法 ==========

// 序列化游戏状态
// 返回：包含游戏所有状态的ValueMap对象
//
// 序列化的数据包括：
// - topHandCardId: 当前顶手牌ID
// - handOrder: 手牌顺序ID列表（转换为ValueVector）
// - cards: 所有卡牌的序列化数据（转换为ValueMap）
//
// 工作流程：
// 1. 创建ValueMap对象
// 2. 添加顶手牌ID
// 3. 将手牌顺序列表转换为ValueVector并添加
// 4. 遍历所有卡牌，序列化每张卡牌并添加到cards映射表中
// 5. 返回ValueMap对象
//
// 使用场景：
// - 保存游戏进度
// - 执行操作前保存状态（用于撤销）
cocos2d::ValueMap GameModel::serialize() const {
    cocos2d::ValueMap data;
    // 保存顶手牌ID
    data["topHandCardId"] = cocos2d::Value(_topHandCardId);
    
    // 序列化手牌顺序列表
    cocos2d::ValueVector handOrderVec;
    for (int cardId : _handCardOrder) {
        handOrderVec.push_back(cocos2d::Value(cardId));
    }
    data["handOrder"] = cocos2d::Value(handOrderVec);
    
    // 序列化所有卡牌数据
    cocos2d::ValueMap cardsMap;
    for (const auto& pair : _cards) {
        cocos2d::ValueMap cardData = pair.second->serialize();
        cardsMap[std::to_string(pair.first)] = cocos2d::Value(cardData);
    }
    data["cards"] = cocos2d::Value(cardsMap);
    
    return data;
}

// 反序列化游戏状态
// 参数：data - 包含游戏状态的ValueMap对象
// 返回：成功返回true，失败返回false
//
// 工作流程：
// 1. 重置当前游戏状态
// 2. 检查必要字段是否存在（topHandCardId、handOrder、cards）
// 3. 恢复顶手牌ID
// 4. 恢复手牌顺序列表
// 5. 遍历所有卡牌数据，为每张卡牌创建对象并恢复其状态
// 6. 返回成功
//
// 错误处理：
// - 如果缺少必要字段，返回false
// - 如果卡牌反序列化失败，删除卡牌对象并返回false
//
// 使用场景：
// - 加载保存的游戏进度
// - 撤销操作时恢复之前的状态
bool GameModel::deserialize(const cocos2d::ValueMap& data) {
    // 重置当前游戏状态
    reset();
    
    // 检查必要字段是否存在
    if (!data.count("topHandCardId") || !data.count("handOrder") || !data.count("cards")) {
        return false;
    }
    
    // 恢复顶手牌ID
    _topHandCardId = data.at("topHandCardId").asInt();
    
    // 反序列化手牌顺序列表
    const cocos2d::ValueVector& handOrderVec = data.at("handOrder").asValueVector();
    for (const auto& value : handOrderVec) {
        _handCardOrder.push_back(value.asInt());
    }
    
    // 反序列化所有卡牌数据
    const cocos2d::ValueMap& cardsMap = data.at("cards").asValueMap();
    for (const auto& pair : cardsMap) {
        int cardId = std::stoi(pair.first);
        const cocos2d::ValueMap& cardData = pair.second.asValueMap();
        
        // 创建卡牌对象并反序列化
        CardModel* card = new CardModel();
        if (card && card->deserialize(cardData)) {
            _cards[cardId] = card;
        } else {
            // 反序列化失败，删除卡牌对象
            delete card;
            return false;
        }
    }
    
    return true;
}

} // namespace models
