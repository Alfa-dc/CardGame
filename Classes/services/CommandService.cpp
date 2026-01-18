// CommandService.cpp
// 命令服务实现文件 - 负责管理游戏中的操作命令和撤销/重做功能
#pragma execution_character_set("utf-8")
#include "CommandService.h"
#include "CardService.h"

namespace services {

// 单例实例指针
CommandService* CommandService::_instance = nullptr;

// 获取单例实例
// 返回：命令服务的单例实例
CommandService* CommandService::getInstance() {
    if (!_instance) {
        _instance = new CommandService();
        _instance->init();
    }
    return _instance;
}

// 销毁单例实例
void CommandService::destroyInstance() {
    CC_SAFE_DELETE(_instance);
}

// 构造函数
CommandService::CommandService() : _undoModel(nullptr) {
}

// 析构函数
CommandService::~CommandService() {
    CC_SAFE_DELETE(_undoModel);
}

// 初始化命令服务
// 返回：成功返回true，失败返回false
bool CommandService::init() {
    // 创建撤销模型
    _undoModel = new models::UndoModel();
    // 设置最大历史记录数量为100
    _undoModel->setMaxHistorySize(100);
    return true;
}

// 执行替换顶手牌命令
// 参数：
//   clickedCardId - 被点击的卡牌ID
//   gameModel - 游戏模型
//
// 功能说明：
// 1. 保存操作前的游戏状态
// 2. 执行替换顶手牌操作
// 3. 保存操作后的游戏状态
// 4. 记录操作到撤销栈
void CommandService::executeReplaceTopCardCommand(int clickedCardId, models::GameModel* gameModel) {
    if (!gameModel) {
        return;
    }
    
    // 获取当前顶手牌ID
    int topCardId = gameModel->getTopHandCardId();
    
    // 如果点击的就是当前顶手牌，则不需要替换
    if (clickedCardId == topCardId) {
        return;
    }
    
    // 保存操作前的游戏状态
    cocos2d::ValueMap prevState = gameModel->serialize();
    
    // 获取旧顶手牌和新顶手牌的模型
    models::CardModel* oldTopCard = gameModel->getCardById(topCardId);
    models::CardModel* newTopCard = gameModel->getCardById(clickedCardId);
    
    if (oldTopCard && newTopCard) {
        // 保存旧顶手牌的位置
        cocos2d::Vec2 oldTopPosition = oldTopCard->getPosition();
        
        // 设置新的顶手牌ID
        gameModel->setTopHandCardId(clickedCardId);
        
        // 隐藏旧顶手牌
        oldTopCard->setVisible(false);
        
        // 将新顶手牌移动到旧顶手牌的位置
        newTopCard->setPosition(oldTopPosition);
    }
    
    // 保存操作后的游戏状态
    cocos2d::ValueMap nextState = gameModel->serialize();
    
    // 记录操作到撤销栈
    recordAction(models::ActionType::REPLACE_TOP_CARD, clickedCardId, topCardId, prevState, nextState);
}

// 执行匹配桌面卡牌命令
// 参数：
//   tableCardId - 桌面卡牌ID
//   gameModel - 游戏模型
//
// 功能说明：
// 1. 保存操作前的游戏状态
// 2. 执行卡牌匹配操作
// 3. 保存操作后的游戏状态
// 4. 记录操作到撤销栈
void CommandService::executeMatchTableCardCommand(int tableCardId, models::GameModel* gameModel) {
    if (!gameModel) {
        return;
    }
    
    // 获取顶手牌ID
    int topCardId = gameModel->getTopHandCardId();
    
    // 获取顶手牌和桌面牌的模型
    models::CardModel* topCard = gameModel->getCardById(topCardId);
    models::CardModel* tableCard = gameModel->getCardById(tableCardId);
    
    // 检查顶手牌和桌面牌是否可以匹配（点数相差1）
    if (!CardService::getInstance()->isCardsMatchable(
            gameModel->getCardById(tableCardId), 
            gameModel->getCardById(topCardId))) {
        return;
    }
    
    // 保存操作前的游戏状态
    cocos2d::ValueMap prevState = gameModel->serialize();
    
    if (topCard && tableCard) {
        // 保存顶手牌的位置
        cocos2d::Vec2 topCardPosition = topCard->getPosition();
        
        // 隐藏顶手牌
        topCard->setVisible(false);
        
        // 将桌面牌移动到顶手牌的位置
        tableCard->setPosition(topCardPosition);
        
        // 将桌面牌设置为新的顶手牌
        gameModel->setTopHandCardId(tableCardId);
        
        // 显示桌面牌
        tableCard->setVisible(true);
    }
    
    // 保存操作后的游戏状态
    cocos2d::ValueMap nextState = gameModel->serialize();
    
    // 记录操作到撤销栈
    recordAction(models::ActionType::MATCH_TABLE_CARD, tableCardId, topCardId, prevState, nextState);
}

// 执行撤销操作
// 参数：gameModel - 游戏模型
// 返回：操作记录对象，包含撤销的操作信息
models::ActionRecord* CommandService::undo(models::GameModel* gameModel) {
    if (!gameModel) {
        return nullptr;
    }
    
    // 从撤销栈中获取上一个操作记录
    models::ActionRecord* record = _undoModel->undo();
    if (!record) {
        return nullptr;
    }
    
    // 恢复操作前的游戏状态
    bool result = gameModel->deserialize(record->prevGameState);
    
    if (!result) {
        delete record;
        return nullptr;
    }
    
    return record;
}

// 执行重做操作
// 参数：gameModel - 游戏模型
// 返回：操作记录对象，包含重做的操作信息
models::ActionRecord* CommandService::redo(models::GameModel* gameModel) {
    if (!gameModel) {
        return nullptr;
    }
    
    // 从重做栈中获取下一个操作记录
    models::ActionRecord* record = _undoModel->redo();
    if (!record) {
        return nullptr;
    }
    
    // 恢复操作后的游戏状态
    bool result = gameModel->deserialize(record->nextGameState);
    
    if (!result) {
        delete record;
        return nullptr;
    }
    
    return record;
}

// 检查是否可以撤销
// 返回：true表示可以撤销，false表示不能
bool CommandService::canUndo() const {
    return _undoModel->canUndo();
}

// 检查是否可以重做
// 返回：true表示可以重做，false表示不能
bool CommandService::canRedo() const {
    return _undoModel->canRedo();
}

// 获取撤销历史记录数量
size_t CommandService::getUndoCount() const {
    return _undoModel->getUndoCount();
}

// 获取重做历史记录数量
size_t CommandService::getRedoCount() const {
    return _undoModel->getRedoCount();
}

// 清空操作历史
void CommandService::clearHistory() {
    _undoModel->clearHistory();
}

// 记录操作到历史
// 参数：
//   actionType - 操作类型
//   sourceCardId - 源卡牌ID
//   targetCardId - 目标卡牌ID
//   prevState - 操作前的状态
//   nextState - 操作后的状态
void CommandService::recordAction(const models::ActionType& actionType, int sourceCardId, int targetCardId,
                                 const cocos2d::ValueMap& prevState, const cocos2d::ValueMap& nextState) {
    // 创建操作记录
    models::ActionRecord record(actionType);
    record.sourceCardId = sourceCardId;
    record.targetCardId = targetCardId;
    record.prevGameState = prevState;
    record.nextGameState = nextState;
    
    // 将操作记录添加到撤销模型
    _undoModel->addActionRecord(record);
}

} // namespace services
