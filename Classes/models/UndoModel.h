// UndoModel.h
// 撤销模型 - 负责管理撤销/重做历史记录
//
// 这个类是撤销/重做系统的核心数据模型，负责：
// 1. 存储操作历史记录
// 2. 管理撤销栈和重做栈
// 3. 支持撤销和重做操作
// 4. 限制历史记录的最大数量
//
// 核心概念：
// - 撤销栈：存储已执行的操作，用于撤销
// - 重做栈：存储已撤销的操作，用于重做
// - 操作记录：记录操作的类型、涉及的卡牌ID、操作前后的游戏状态
// - 状态快照：保存操作前后的游戏状态
//
// 使用示例：
// UndoModel* undoModel = new UndoModel();
// undoModel->setMaxHistorySize(100);
// undoModel->addActionRecord(record);
// ActionRecord* record = undoModel->undo();
#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"
#include <stack>

namespace models {

// 操作类型枚举 - 定义游戏中所有可能的操作类型
enum class ActionType {
    REPLACE_TOP_CARD,  // 替换顶手牌操作
    MATCH_TABLE_CARD,  // 匹配桌面卡牌操作
    UNKNOWN            // 未知操作
};

// 操作记录结构体 - 记录一次操作的完整信息
struct ActionRecord {
    ActionType type;                  // 操作类型
    int sourceCardId;                 // 源卡牌ID（被点击的卡牌）
    int targetCardId;                 // 目标卡牌ID（被影响的卡牌）
    cocos2d::ValueMap prevGameState;  // 操作前的游戏状态快照
    cocos2d::ValueMap nextGameState;  // 操作后的游戏状态快照
    
    // 构造函数 - 初始化操作记录
    ActionRecord(ActionType actionType = ActionType::UNKNOWN) : 
        type(actionType), sourceCardId(-1), targetCardId(-1) {}
};

// 撤销模型类 - 管理撤销/重做历史记录
class UndoModel {
public:
    // 构造函数
    UndoModel();
    
    // 析构函数
    ~UndoModel();
    
    // ========== 操作记录管理 ==========
    
    // 添加操作记录
    // 参数：record - 操作记录
    //
    // 功能说明：
    // - 将操作记录添加到撤销栈
    // - 清空重做栈（因为执行新操作后，之前的重做记录失效）
    // - 如果历史记录超过最大数量，则删除最旧的记录
    void addActionRecord(const ActionRecord& record);
    
    // ========== 撤销/重做操作 ==========
    
    // 执行撤销操作 - 从撤销栈中取出上一个操作记录
    // 返回：操作记录对象，包含撤销的操作信息
    //
    // 功能说明：
    // - 从撤销栈中取出栈顶的操作记录
    // - 将该操作记录移动到重做栈
    // - 返回操作记录的副本
    ActionRecord* undo();
    
    // 执行重做操作 - 从重做栈中取出下一个操作记录
    // 返回：操作记录对象，包含重做的操作信息
    //
    // 功能说明：
    // - 从重做栈中取出栈顶的操作记录
    // - 将该操作记录移动到撤销栈
    // - 返回操作记录的副本
    ActionRecord* redo();
    
    // ========== 状态查询 ==========
    
    // 检查是否可以撤销
    // 返回：true表示可以撤销，false表示不能
    bool canUndo() const { return !_undoStack.empty(); }
    
    // 检查是否可以重做
    // 返回：true表示可以重做，false表示不能
    bool canRedo() const { return !_redoStack.empty(); }
    
    // 获取撤销历史记录数量
    size_t getUndoCount() const { return _undoStack.size(); }
    
    // 获取重做历史记录数量
    size_t getRedoCount() const { return _redoStack.size(); }
    
    // ========== 历史记录管理 ==========
    
    // 清空操作历史
    // 功能说明：
    // - 清空撤销栈和重做栈
    // - 用于重置游戏时清空历史
    void clearHistory();
    
    // 设置/获取最大历史记录数量
    // 参数：size - 最大历史记录数量
    void setMaxHistorySize(size_t size) { _maxHistorySize = size; }
    size_t getMaxHistorySize() const { return _maxHistorySize; }
    
private:
    // ========== 私有成员变量 ==========
    
    std::stack<ActionRecord> _undoStack;  // 撤销栈 - 存储已执行的操作
    std::stack<ActionRecord> _redoStack;  // 重做栈 - 存储已撤销的操作
    size_t _maxHistorySize;               // 最大历史记录数量 - 限制历史记录的数量
};

} // namespace models

#endif // __UNDO_MODEL_H__
