// CardView.h
// 卡牌视图类 - 负责显示卡牌和处理用户交互
// 
// 这个类继承自cocos2d::Node，是卡牌在屏幕上的视觉表现
// 它负责：
// 1. 显示卡牌的外观（背景、数字、花色）
// 2. 处理用户的点击和触摸事件
// 3. 执行卡牌的移动和动画效果
// 4. 管理卡牌的选中状态和顶手牌状态
//
// 核心概念：
// - 视图与模型分离：CardView只负责显示，CardModel负责数据
// - 事件回调：通过回调函数通知控制器卡牌被点击
// - 动画效果：使用Cocos2d-x的Action系统实现平滑动画
// - 触摸事件：使用EventListenerTouchOneByOne处理单点触摸
//
// 使用示例：
// CardView* cardView = CardView::createWithModel(cardModel);
// cardView->setClickCallback([this](CardView* view) {
//     this->handleCardClick(view);
// });
// this->addChild(cardView);
//
#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "models/CardModel.h"

namespace views {

class CardView : public cocos2d::Node {
public:
    // Cocos2d-x的宏，用于创建对象
    CREATE_FUNC(CardView);
    
    // 使用模型创建卡牌视图
    // 参数：cardModel - 卡牌数据模型
    // 返回：创建的CardView对象，失败返回nullptr
    //
    // 功能说明：
    // - 创建一个新的CardView对象
    // - 根据卡牌模型初始化视图
    // - 设置卡牌的外观（纹理、数字、花色）
    //
    // 使用场景：
    // - 初始化游戏时创建所有卡牌视图
    // - 从序列化数据恢复时创建卡牌视图
    static CardView* createWithModel(const models::CardModel* cardModel);
    
    // 使用模型初始化卡牌视图
    // 参数：cardModel - 卡牌数据模型
    // 返回：成功返回true，失败返回false
    //
    // 功能说明：
    // - 保存卡牌ID
    // - 初始化卡牌的视觉组件（背景、数字、花色）
    // - 设置卡牌的初始位置和缩放比例
    // - 初始化触摸事件监听器
    //
    // 使用场景：
    // - createWithModel内部调用此方法
    virtual bool initWithModel(const models::CardModel* cardModel);
    
    // 使用模型更新卡牌视图
    // 参数：cardModel - 卡牌数据模型
    //
    // 功能说明：
    // - 更新卡牌的位置
    // - 更新卡牌的可见性
    // - 更新卡牌的选中状态
    // - 更新卡牌的顶手牌状态
    //
    // 使用场景：
    // - 卡牌数据发生变化时更新视图
    // - 撤销/重做操作后恢复卡牌状态
    void updateWithModel(const models::CardModel* cardModel);
    
    // 获取卡牌ID
    // 返回：卡牌的唯一标识符
    int getCardId() const { return _cardId; }
    
    // 获取/设置卡牌是否可点击
    // 可点击性控制用户是否可以与卡牌交互
    bool isClickable() const { return _isClickable; }
    void setClickable(bool clickable) { _isClickable = clickable; }
    
    // 定义卡牌点击回调函数类型
    // 参数：CardView* - 被点击的卡牌视图
    typedef std::function<void(CardView*)> CardClickCallback;
    
    // 设置卡牌点击回调函数
    // 参数：callback - 点击事件发生时调用的函数
    //
    // 使用场景：
    // - 在GameController中设置回调，处理卡牌点击逻辑
    void setClickCallback(const CardClickCallback& callback) { _clickCallback = callback; }
    
    // 移动卡牌到指定位置
    // 参数：
    //   target - 目标位置
    //   duration - 移动持续时间（秒），默认0.3秒
    //   callback - 移动完成后的回调函数，可选
    //
    // 功能说明：
    // - 使用MoveTo动作实现平滑移动
    // - 设置_isMoving标志表示正在移动
    // - 移动完成后调用回调函数
    //
    // 使用场景：
    // - 卡牌匹配成功后移动到目标位置
    // - 卡牌从手牌堆移动到桌面
    void moveToPosition(const cocos2d::Vec2& target, 
                       float duration = 0.3f, 
                       const std::function<void()>& callback = nullptr);
    
    // 获取卡牌是否正在移动
    // 返回：true表示正在移动，false表示静止
    bool isMoving() const { return _isMoving; }
    
    // 设置卡牌的选中状态
    // 参数：selected - true表示选中，false表示未选中
    //
    // 功能说明：
    // - 选中时卡牌放大1.2倍
    // - 未选中时恢复原始大小
    // - 使用ScaleTo动作实现平滑缩放
    //
    // 使用场景：
    // - 用户点击卡牌时设置为选中状态
    // - 卡牌匹配成功后取消选中状态
    void setSelected(bool selected);
    
    // 设置卡牌为顶手牌状态
    // 参数：isTop - true表示是顶手牌，false表示不是
    //
    // 功能说明：
    // - 顶手牌会稍微放大并向前移动
    // - 非顶手牌恢复原始大小和位置
    // - 使用Z轴调整实现前后层次效果
    //
    // 使用场景：
    // - 手牌堆最上面的牌设置为顶手牌
    // - 用户点击手牌时将其设置为顶手牌
    void setAsTopCard(bool isTop);
    
    // 播放卡牌点击效果
    //
    // 功能说明：
    // - 卡牌放大20%
    // - 保持2秒钟
    // - 恢复原始大小
    // - 使用Sequence组合多个动作
    //
    // 使用场景：
    // - 用户点击卡牌时提供视觉反馈
    void playClickEffect();
    
protected:
    // 构造函数
    CardView();
    
    // 析构函数
    virtual ~CardView();
    
    // ========== 触摸事件处理 ==========
    
    // 触摸开始事件
    // 参数：
    //   touch - 触摸对象
    //   event - 事件对象
    // 返回：true表示接受触摸事件，false表示忽略
    //
    // 功能说明：
    // - 检查卡牌是否可点击
    // - 检查触摸点是否在卡牌范围内
    // - 播放点击效果
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    
    // 触摸结束事件
    // 参数：
    //   touch - 触摸对象
    //   event - 事件对象
    //
    // 功能说明：
    // - 调用点击回调函数
    // - 通知控制器卡牌被点击
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    // ========== 初始化方法 ==========
    
    // 初始化触摸事件监听器
    //
    // 功能说明：
    // - 创建单点触摸监听器
    // - 设置触摸开始和结束的回调
    // - 将监听器添加到事件分发器
    //
    // 使用场景：
    // - initWithModel内部调用此方法
    void initTouchEvents();
    
    // 初始化卡牌的视觉组件
    //
    // 功能说明：
    // - 创建背景精灵
    // - 创建数字精灵（左上角和右下角）
    // - 创建花色精灵（右下角）
    // - 根据卡牌花色和点数设置对应的纹理
    //
    // 使用场景：
    // - initWithModel内部调用此方法
    void initCardComponents();
    
private:
    // ========== 私有成员变量 ==========
    
    int _cardId;                          // 卡牌ID - 唯一标识符
    bool _isClickable;                    // 是否可点击 - 控制用户交互
    bool _isMoving;                       // 是否正在移动 - 防止重复操作
    cocos2d::EventListenerTouchOneByOne* _touchListener; // 触摸事件监听器
    CardClickCallback _clickCallback;     // 点击回调函数
    float _scaleFactor;                   // 缩放因子 - 控制卡牌大小
    
    // ========== 视觉组件 ==========
    
    cocos2d::Sprite* _background;         // 背景精灵 - 卡牌的白色背景
    cocos2d::Sprite* _numberSprite;       // 左上角数字精灵 - 显示卡牌点数
    cocos2d::Sprite* _bigNumberSprite;    // 右下角数字精灵 - 大号显示点数
    cocos2d::Sprite* _topRightSuitSprite; // 右下角花色精灵 - 显示卡牌花色
};

} // namespace views

#endif // __CARD_VIEW_H__
