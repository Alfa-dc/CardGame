// GameView.h
// 游戏视图类 - 负责显示游戏场景和所有UI元素
// 
// 这个类继承自cocos2d::Scene，是游戏的主场景
// 它负责：
// 1. 显示所有卡牌视图
// 2. 显示游戏UI按钮（撤销、重做、重置）
// 3. 处理卡牌匹配和替换的动画
// 4. 管理卡牌视图的添加、删除和更新
// 5. 显示顶手牌信息
//
// 核心概念：
// - 场景管理：GameView是游戏的主场景，包含所有游戏元素
// - 卡牌视图管理：使用映射表管理所有卡牌视图
// - 动画效果：使用Cocos2d-x的Action系统实现动画
// - 回调机制：通过回调函数通知控制器按钮被点击
//
// 使用示例：
// GameView* gameView = GameView::create();
// gameView->setCardClickCallback([this](CardView* view) {
//     this->handleCardClick(view);
// });
// gameView->setUndoCallback([this]() {
//     this->handleUndo();
// });
// cocos2d::Director::getInstance()->replaceScene(gameView);
//
#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CardView.h"
#include "models/GameModel.h"

namespace views {

class GameView : public cocos2d::Scene {
public:
    // Cocos2d-x的宏，用于创建场景对象
    CREATE_FUNC(GameView);
    
    // 初始化游戏场景
    // 返回：成功返回true，失败返回false
    //
    // 功能说明：
    // - 调用父类Scene的init方法
    // - 初始化UI按钮
    // - 设置初始布局
    virtual bool init() override;
    
    // ========== 卡牌视图管理方法 ==========
    
    // 更新游戏视图
    // 参数：gameModel - 游戏数据模型
    //
    // 功能说明：
    // - 遍历所有卡牌，更新对应的卡牌视图
    // - 确保视图与模型保持同步
    //
    // 使用场景：
    // - 撤销/重做操作后更新视图
    // - 游戏状态发生变化时更新视图
    void updateGameView(const models::GameModel* gameModel);
    
    // 添加卡牌视图
    // 参数：cardView - 要添加的卡牌视图
    //
    // 功能说明：
    // - 将卡牌视图添加到映射表中
    // - 将卡牌视图添加到场景中
    // - 将zOrder保存到对应的CardModel中（用于撤销/重做）
    //
    // 使用场景：
    // - 初始化游戏时添加所有卡牌视图
    void addCardView(CardView* cardView);
    
    // 设置游戏模型引用
    // 参数：gameModel - 游戏数据模型
    //
    // 功能说明：
    // - 保存游戏模型的引用，用于更新CardModel的zOrder
    //
    // 使用场景：
    // - 初始化游戏时设置游戏模型
    void setGameModel(const models::GameModel* gameModel) { _gameModel = gameModel; }
    
    // 移除卡牌视图
    // 参数：cardId - 要移除的卡牌ID
    //
    // 功能说明：
    // - 从映射表中查找并移除卡牌视图
    // - 从场景中移除卡牌视图
    //
    // 使用场景：
    // - 卡牌被匹配后移除视图
    void removeCardView(int cardId);
    
    // 获取卡牌视图
    // 参数：cardId - 卡牌ID
    // 返回：找到的卡牌视图，未找到返回nullptr
    //
    // 使用场景：
    // - 查找特定卡牌视图进行操作
    CardView* getCardView(int cardId) const;
    
    // 更新卡牌视图
    // 参数：cardModel - 卡牌数据模型
    //
    // 功能说明：
    // - 查找对应的卡牌视图
    // - 使用模型更新卡牌视图
    //
    // 使用场景：
    // - 卡牌数据发生变化时更新视图
    void updateCardView(const models::CardModel* cardModel);
    
    // ========== 动画方法 ==========
    
    // 播放卡牌匹配动画
    // 参数：
    //   topCardId - 顶手牌ID
    //   tableCardId - 桌面牌ID
    //
    // 功能说明：
    // - 两张卡牌移动到屏幕中央
    // - 播放匹配特效
    // - 两张卡牌消失
    //
    // 使用场景：
    // - 卡牌匹配成功时播放动画
    void playMatchAnimation(int topCardId, int tableCardId);
    
    // 播放卡牌替换动画
    // 参数：
    //   clickedCardId - 被点击的卡牌ID
    //   topCardId - 当前顶手牌ID
    //
    // 功能说明：
    // - 被点击的卡牌移动到顶手牌位置
    // - 当前顶手牌移动到被点击卡牌的位置
    // - 交换两张卡牌的位置
    //
    // 使用场景：
    // - 玩家点击手牌时播放替换动画
    void playReplaceAnimation(int clickedCardId, int topCardId);
    
    // 播放撤销动画
    // 参数：
    //   cardId - 卡牌ID
    //   originalPos - 原始位置
    //
    // 功能说明：
    // - 卡牌从当前位置移动到原始位置
    // - 恢复卡牌的可见性
    //
    // 使用场景：
    // - 撤销操作时恢复卡牌位置
    void playUndoAnimation(int cardId, const cocos2d::Vec2& originalPos);
    
    // ========== 信息显示方法 ==========
    
    // 更新顶手牌信息
    // 参数：topCard - 顶手牌数据模型
    //
    // 功能说明：
    // - 显示顶手牌的花色和点数
    // - 更新顶手牌信息面板
    //
    // 使用场景：
    // - 顶手牌发生变化时更新信息
    void updateTopCardInfo(const models::CardModel* topCard);
    
    // ========== 回调函数设置方法 ==========
    
    // 定义按钮回调函数类型
    typedef std::function<void()> UndoCallback;
    typedef std::function<void()> RedoCallback;
    typedef std::function<void()> ResetCallback;
    
    // 设置按钮回调函数
    // 参数：callback - 按钮点击时调用的函数
    //
    // 使用场景：
    // - 在GameController中设置回调，处理按钮点击逻辑
    void setUndoCallback(const UndoCallback& callback) { _undoCallback = callback; }
    void setRedoCallback(const RedoCallback& callback) { _redoCallback = callback; }
    void setResetCallback(const ResetCallback& callback) { _resetCallback = callback; }
    
    // 设置卡牌点击回调函数
    // 参数：callback - 卡牌点击时调用的函数
    //
    // 使用场景：
    // - 在GameController中设置回调，处理卡牌点击逻辑
    void setCardClickCallback(const CardView::CardClickCallback& callback) { _cardClickCallback = callback; }
    
    // ========== 布局和初始化方法 ==========
    
    // 设置初始布局
    //
    // 功能说明：
    // - 设置背景颜色
    // - 设置UI按钮的位置
    // - 设置卡牌显示区域
    //
    // 使用场景：
    // - 初始化游戏时设置布局
    void setupInitialLayout();
    
    // 播放入场动画
    //
    // 功能说明：
    // - 卡牌从屏幕外飞入
    // - UI按钮淡入显示
    //
    // 使用场景：
    // - 游戏开始时播放入场动画
    void playEntryAnimation();
    
private:
    // ========== 私有方法 ==========
    
    // 初始化UI按钮
    //
    // 功能说明：
    // - 创建撤销、重做、重置按钮
    // - 设置按钮的位置和样式
    // - 设置按钮的点击回调
    //
    // 使用场景：
    // - init方法内部调用
    void initButtons();
    
    // ========== 按钮点击事件处理 ==========
    
    // 撤销按钮点击事件
    // 参数：sender - 发送事件的对象
    //
    // 功能说明：
    // - 调用撤销回调函数
    //
    // 使用场景：
    // - 用户点击撤销按钮时触发
    void onUndoButtonClicked(cocos2d::Ref* sender);
    
    // 重做按钮点击事件
    // 参数：sender - 发送事件的对象
    //
    // 功能说明：
    // - 调用重做回调函数
    //
    // 使用场景：
    // - 用户点击重做按钮时触发
    void onRedoButtonClicked(cocos2d::Ref* sender);
    
    // 重置按钮点击事件
    // 参数：sender - 发送事件的对象
    //
    // 功能说明：
    // - 调用重置回调函数
    //
    // 使用场景：
    // - 用户点击重置按钮时触发
    void onResetButtonClicked(cocos2d::Ref* sender);
    
    // ========== 私有成员变量 ==========
    
    // UI按钮
    cocos2d::ui::Button* _undoButton;      // 撤销按钮
    cocos2d::ui::Button* _redoButton;      // 重做按钮
    cocos2d::ui::Button* _resetButton;     // 重置按钮
    
    // 卡牌视图映射表
    std::unordered_map<int, CardView*> _cardViews;  // 卡牌ID -> 卡牌视图
    
    // 游戏模型引用
    const models::GameModel* _gameModel;  // 游戏数据模型，用于更新CardModel的zOrder
    
    // 回调函数
    UndoCallback _undoCallback;             // 撤销回调
    RedoCallback _redoCallback;             // 重做回调
    ResetCallback _resetCallback;           // 重置回调
    CardView::CardClickCallback _cardClickCallback;  // 卡牌点击回调
};

} // namespace views

#endif // __GAME_VIEW_H__
