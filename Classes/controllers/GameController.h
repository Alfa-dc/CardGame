// GameController.h
// 游戏主控制器 - 负责协调游戏各组件，管理游戏流程
// 
// 这个类是整个游戏的核心控制器，负责：
// 1. 协调模型、视图和其他控制器
// 2. 管理游戏的整体流程（开始、重置、撤销、重做）
// 3. 处理用户交互（卡牌点击、按钮点击）
// 4. 实现游戏的核心逻辑（卡牌匹配、替换）
// 5. 管理撤销/重做功能
//
// 核心概念：
// - MVC架构：GameController是控制器层，协调模型和视图
// - 命令模式：使用CommandService实现撤销/重做
// - 事件驱动：通过回调函数处理用户交互
// - 状态管理：通过GameModel管理游戏状态
//
// 使用示例：
// GameController* controller = new GameController();
// if (controller->init()) {
//     controller->startGame(1);  // 开始第一关
//     cocos2d::Director::getInstance()->replaceScene(controller->getGameView());
// }
//
#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"

#include "services/CommandService.h"


#include "configs/loaders/LevelConfigLoader.h"
#include "generators/GameModelFromLevelGenerator.h"

namespace controllers {

class GameController {
public:
    // 构造函数
    // 初始化所有成员变量为默认值
    GameController();
    
    // 析构函数
    // 清理所有动态分配的资源
    ~GameController();
    
    // ========== 初始化方法 ==========
    
    // 初始化游戏控制器
    // 返回：成功返回true，失败返回false
    //
    // 工作流程：
    // 1. 初始化卡牌控制器
    // 2. 初始化游戏区域控制器和堆叠控制器
    // 3. 初始化撤销管理器
    // 4. 设置撤销管理器的回调函数
    //
    // 使用场景：
    // - 在创建GameController后调用
    bool init();
    
    // ========== 游戏流程方法 ==========
    
    // 开始游戏
    // 参数：levelId - 关卡ID
    //
    // 工作流程：
    // 1. 保存当前关卡ID
    // 2. 加载关卡配置
    // 3. 根据配置生成游戏模型
    // 4. 初始化游戏区域控制器和堆叠控制器
    // 5. 初始化游戏视图
    // 6. 设置事件监听器
    // 7. 创建所有卡牌视图并添加到游戏视图
    // 8. 更新UI
    // 9. 播放入场动画
    //
    // 使用场景：
    // - 游戏启动时调用
    // - 重新开始关卡时调用
    void startGame(int levelId);
    
    // ========== 访问器方法 ==========
    
    // 获取游戏视图
    // 返回：游戏视图对象指针
    //
    // 使用场景：
    // - 将游戏视图添加到场景中
    views::GameView* getGameView() const { return _gameView; }
    
    // 获取游戏模型
    // 返回：游戏模型对象指针
    //
    // 使用场景：
    // - 访问游戏状态
    models::GameModel* getGameModel() const { return _gameModel; }
    
    // ========== 事件处理方法 ==========
    
    // 处理卡牌点击事件
    // 参数：cardView - 被点击的卡牌视图
    // 返回：成功处理返回true，失败返回false
    //
    // 工作流程：
    // 1. 检查是否正在执行撤销/重做操作
    // 2. 获取卡牌ID和卡牌模型
    // 3. 判断卡牌类型（手牌或桌面牌）
    // 4. 如果是手牌：替换顶手牌
    // 5. 如果是桌面牌：检查是否可以匹配
    // 6. 如果可以匹配：执行匹配操作
    // 7. 保存操作到撤销管理器
    //
    // 使用场景：
    // - 用户点击卡牌时触发
    bool handleCardClick(views::CardView* cardView);
    
    // 处理撤销按钮点击
    //
    // 工作流程：
    // 1. 检查是否可以撤销
    // 2. 执行撤销操作
    // 3. 更新游戏视图
    // 4. 更新UI
    //
    // 使用场景：
    // - 用户点击撤销按钮时触发
    void handleUndoButtonClick();
    
    // 处理重做按钮点击
    //
    // 工作流程：
    // 1. 检查是否可以重做
    // 2. 执行重做操作
    // 3. 更新游戏视图
    // 4. 更新UI
    //
    // 使用场景：
    // - 用户点击重做按钮时触发
    void handleRedoButtonClick();
    
    // 处理重置按钮点击
    //
    // 工作流程：
    // 1. 清空撤销/重做历史
    // 2. 重新开始当前关卡
    //
    // 使用场景：
    // - 用户点击重置按钮时触发
    void handleResetButtonClick();
    
    // ========== 游戏逻辑方法 ==========
    
    // 检查卡牌是否可匹配
    // 参数：
    //   tableCardId - 桌面牌ID
    //   handCardId - 手牌ID
    // 返回：可以匹配返回true，不能匹配返回false
    //
    // 匹配规则：
    // - 两张牌的花色相同
    // - 两张牌的点数相同
    //
    // 使用场景：
    // - 检查卡牌是否可以匹配
    bool isCardsMatchable(int tableCardId, int handCardId);
    
    // ========== UI更新方法 ==========
    
    // 更新UI
    //
    // 功能说明：
    // - 更新顶手牌信息
    // - 更新撤销/重做按钮状态
    //
    // 使用场景：
    // - 游戏状态发生变化时更新UI
    void updateUI();
    
private:
    //
    
    // 初始化游戏视图
    //
    // 工作流程：
    // 1. 创建游戏视图对象
    // 2. 设置卡牌点击回调
    // 3. 设置按钮回调
    // 4. 设置初始布局
    //
    // 使用场景：
    // - startGame方法内部调用
    void initGameView();
    
    // 设置事件监听器
    //
    // 工作流程：
    // 1. 创建键盘监听器
    // 2. 设置Z键为撤销，Y键为重做
    // 3. 将监听器添加到事件分发器
    //
    // 使用场景：
    // - startGame方法内部调用
    void setupEventListeners();
    
    // ========== 私有游戏逻辑方法 ==========
    
    // 替换顶手牌
    // 参数：clickedCardId - 被点击的卡牌ID
    //
    // 工作流程：
    // 1. 获取当前顶手牌
    // 2. 将被点击的卡牌设置为新的顶手牌
    // 3. 播放替换动画
    //
    // 使用场景：
    // - 用户点击手牌时触发
    void replaceTopHandCard(int clickedCardId);
    
    // 匹配桌面卡牌
    // 参数：tableCardId - 桌面牌ID
    //
    // 工作流程：
    // 1. 获取顶手牌
    // 2. 检查是否可以匹配
    // 3. 如果可以匹配：
    //    a. 播放匹配动画
    //    b. 从游戏中移除两张卡牌
    //    c. 保存操作到撤销管理器
    //
    // 使用场景：
    // - 用户点击桌面牌时触发
    void matchTableCard(int tableCardId);
    
    // ========== 私有撤销/重做方法 ==========
    
    // 执行撤销操作
    //
    // 工作流程：
    // 1. 设置撤销/重做标志
    // 2. 从撤销管理器获取上一个状态
    // 3. 反序列化游戏状态
    // 4. 更新所有卡牌视图
    // 5. 清除撤销/重做标志
    //
    // 使用场景：
    // - handleUndoButtonClick方法内部调用
    void executeUndo();
    
    // 执行重做操作
    //
    // 工作流程：
    // 1. 设置撤销/重做标志
    // 2. 从撤销管理器获取下一个状态
    // 3. 反序列化游戏状态
    // 4. 更新所有卡牌视图
    // 5. 清除撤销/重做标志
    //
    // 使用场景：
    // - handleRedoButtonClick方法内部调用
    void executeRedo();
    
    // ========== 私有成员变量 ==========
    
    // 核心组件
    models::GameModel* _gameModel;           // 游戏模型 - 管理游戏状态
    views::GameView* _gameView;             // 游戏视图 - 显示游戏界面
    
    // 撤销/重做
    bool _isPerformingUndoRedo;             // 是否正在执行撤销/重做操作
    
    // 游戏状态
    int _currentLevelId;                     // 当前关卡ID
};

} // namespace controllers

#endif // __GAME_CONTROLLER_H__
