// CommandService.h
// 命令服务 - 负责管理游戏中的命令操作，实现撤销/重做功能
// 
// 这个类是撤销/重做功能的核心，负责：
// 1. 记录游戏中的所有操作（替换顶手牌、匹配卡牌）
// 2. 保存操作前后的游戏状态
// 3. 执行撤销操作，恢复到上一个状态
// 4. 执行重做操作，恢复到下一个状态
// 5. 管理撤销/重做历史记录
//
// 核心概念：
// - 命令模式：将每个操作封装为命令对象
// - 状态快照：保存操作前后的游戏状态
// - 历史栈：使用栈结构管理撤销/重做历史
// - 单例模式：CommandService是单例，全局唯一
//
// 使用示例：
// CommandService* service = CommandService::getInstance();
// service->init();
// service->executeReplaceTopCardCommand(cardId, gameModel);
// service->undo(gameModel);  // 撤销操作
// service->redo(gameModel);  // 重做操作
//
#ifndef __COMMAND_SERVICE_H__
#define __COMMAND_SERVICE_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "models/UndoModel.h"

namespace services {

class CommandService {
public:
    // ========== 单例模式方法 ==========
    
    // 获取单例实例
    // 返回：CommandService的单例对象
    //
    // 使用场景：
    // - 获取CommandService实例进行操作
    static CommandService* getInstance();
    
    // 销毁单例实例
    //
    // 使用场景：
    // - 游戏退出时清理资源
    static void destroyInstance();
    
    // ========== 初始化方法 ==========
    
    // 初始化命令服务
    // 返回：成功返回true，失败返回false
    //
    // 工作流程：
    // 1. 创建UndoModel对象
    // 2. 初始化撤销/重做历史
    //
    // 使用场景：
    // - 游戏启动时调用
    bool init();
    
    // ========== 命令执行方法 ==========
    
    // 执行替换顶手牌命令
    // 参数：
    //   clickedCardId - 被点击的卡牌ID
    //   gameModel - 游戏模型
    //
    // 工作流程：
    // 1. 保存操作前的游戏状态
    // 2. 执行替换顶手牌操作
    // 3. 保存操作后的游戏状态
    // 4. 记录操作到历史
    //
    // 使用场景：
    // - 用户点击手牌时触发
    void executeReplaceTopCardCommand(int clickedCardId, models::GameModel* gameModel);
    
    // 执行匹配桌面卡牌命令
    // 参数：
    //   tableCardId - 桌面牌ID
    //   gameModel - 游戏模型
    //
    // 工作流程：
    // 1. 保存操作前的游戏状态
    // 2. 执行匹配卡牌操作
    // 3. 保存操作后的游戏状态
    // 4. 记录操作到历史
    //
    // 使用场景：
    // - 用户点击桌面牌时触发
    void executeMatchTableCardCommand(int tableCardId, models::GameModel* gameModel);
    
    // ========== 撤销/重做方法 ==========
    
    // 执行撤销操作
    // 参数：gameModel - 游戏模型
    // 返回：撤销的操作记录，失败返回nullptr
    //
    // 工作流程：
    // 1. 检查是否可以撤销
    // 2. 从历史栈中弹出上一个操作
    // 3. 恢复操作前的游戏状态
    // 4. 将操作移到重做栈
    //
    // 使用场景：
    // - 用户点击撤销按钮时触发
    models::ActionRecord* undo(models::GameModel* gameModel);
    
    // 执行重做操作
    // 参数：gameModel - 游戏模型
    // 返回：重做的操作记录，失败返回nullptr
    //
    // 工作流程：
    // 1. 检查是否可以重做
    // 2. 从重做栈中弹出操作
    // 3. 恢复操作后的游戏状态
    // 4. 将操作移到历史栈
    //
    // 使用场景：
    // - 用户点击重做按钮时触发
    models::ActionRecord* redo(models::GameModel* gameModel);
    
    // ========== 查询方法 ==========
    
    // 检查是否可以撤销
    // 返回：可以撤销返回true，不能撤销返回false
    //
    // 使用场景：
    // - 更新撤销按钮状态
    bool canUndo() const;
    
    // 检查是否可以重做
    // 返回：可以重做返回true，不能重做返回false
    //
    // 使用场景：
    // - 更新重做按钮状态
    bool canRedo() const;
    
    // 获取撤销历史数量
    // 返回：撤销历史中的操作数量
    //
    // 使用场景：
    // - 显示撤销历史数量
    size_t getUndoCount() const;
    
    // 获取重做历史数量
    // 返回：重做历史中的操作数量
    //
    // 使用场景：
    // - 显示重做历史数量
    size_t getRedoCount() const;
    
    // ========== 清理方法 ==========
    
    // 清空历史记录
    //
    // 功能说明：
    // - 清空撤销历史
    // - 清空重做历史
    //
    // 使用场景：
    // - 开始新游戏时清空历史
    // - 重置游戏时清空历史
    void clearHistory();
    
private:
    // ========== 私有构造函数和析构函数 ==========
    
    // 构造函数
    // 私有构造函数，实现单例模式
    //
    // 功能说明：
    // - 初始化撤销/重做模型指针为nullptr
    CommandService();
    
    // 析构函数
    // 清理命令服务的所有资源
    //
    // 功能说明：
    // - 删除撤销/重做模型对象
    ~CommandService();
    
    // ========== 私有方法 ==========
    
    // 记录操作
    // 参数：
    //   actionType - 操作类型
    //   sourceCardId - 源卡牌ID
    //   targetCardId - 目标卡牌ID
    //   prevState - 操作前的游戏状态
    //   nextState - 操作后的游戏状态
    //
    // 功能说明：
    // - 创建操作记录对象
    // - 添加到撤销历史
    // - 清空重做历史
    //
    // 使用场景：
    // - 执行命令后记录操作
    void recordAction(const models::ActionType& actionType, int sourceCardId, int targetCardId,
                     const cocos2d::ValueMap& prevState, const cocos2d::ValueMap& nextState);
    
    // ========== 私有成员变量 ==========
    
    // 单例实例
    static CommandService* _instance;  // CommandService的单例对象
    
    // 撤销/重做模型
    models::UndoModel* _undoModel;     // 撤销模型 - 管理撤销/重做历史
};

} // namespace services

#endif // __COMMAND_SERVICE_H__
