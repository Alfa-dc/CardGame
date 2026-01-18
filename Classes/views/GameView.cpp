// GameView.cpp
// 游戏视图类实现
// 
// 这个文件实现了GameView类的所有方法
// GameView是游戏的主场景，负责显示所有游戏元素
//
// 主要实现内容：
// 1. 初始化游戏场景
// 2. 创建游戏区域（桌面区和手牌区）
// 3. 初始化UI按钮
// 4. 管理卡牌视图的添加、删除和更新
// 5. 实现卡牌动画效果
//
// 核心概念：
// - 场景管理：GameView是游戏的主场景
// - 区域划分：分为桌面区（棕色）和手牌区（紫色）
// - 卡牌视图管理：使用映射表管理所有卡牌视图
// - 动画效果：使用Cocos2d-x的Action系统实现动画
#pragma execution_character_set("utf-8")
#include "GameView.h"
#include "configs/models/CardConfig.h"

namespace views {

// ========== 初始化方法 ==========

// 初始化游戏场景
// 返回：成功返回true，失败返回false
//
// 工作流程：
// 1. 调用父类Scene的init方法
// 2. 获取屏幕可见大小
// 3. 创建桌面区域（棕色）
// 4. 创建手牌区域（紫色）
// 5. 初始化UI按钮
//
// 使用场景：
// - 游戏启动时调用
bool GameView::init() {
    if (!Scene::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    
    // 桌面区域高度: 1080*1500
    const float mainAreaHeight = 1500.0f;
    // 手牌区域高度: 1080*580
    const float stackAreaHeight = 580.0f;
    
    // 创建桌面区域 - 棕色背景
    auto mainArea = cocos2d::LayerColor::create(cocos2d::Color4B(139, 69, 19, 255), visibleSize.width, mainAreaHeight);
    mainArea->setPosition(cocos2d::Vec2(0, stackAreaHeight));
    this->addChild(mainArea);
    
    // 创建手牌区域 - 紫色背景
    auto stackArea = cocos2d::LayerColor::create(cocos2d::Color4B(102, 51, 153, 255), visibleSize.width, stackAreaHeight);
    stackArea->setPosition(cocos2d::Vec2(0, 0));
    this->addChild(stackArea);
    
    // 初始化UI按钮
    initButtons();
    
    return true;
}

// 初始化UI按钮
//
// 工作流程：
// 1. 设置场景内容大小
// 2. 计算按钮位置和大小
// 3. 创建撤销按钮背景
// 4. 创建撤销按钮文字
// 5. 初始化触摸事件监听器
// 6. 设置触摸回调
//
// 使用场景：
// - init方法内部调用
void GameView::initButtons() {
    // 设置场景内容大小为屏幕可见大小
    this->setContentSize(cocos2d::Director::getInstance()->getVisibleSize());
    
    // 按钮尺寸和位置配置
    const float buttonWidth = 150.0f;
    const float buttonHeight = 100.0f;
    float xPos = this->getContentSize().width - 100.0f; // 右侧边缘内移100px
    float yPos = 270.0f; // 底部边缘上移100px
    
    // 按钮背景颜色 - 深紫色
    cocos2d::Color4B purpleColor = cocos2d::Color4B(102, 51, 153, 255);
    
    // 创建按钮背景
    auto buttonBg = cocos2d::LayerColor::create(purpleColor, buttonWidth, buttonHeight);
    buttonBg->setPosition(cocos2d::Vec2(xPos - buttonWidth/2, yPos - buttonHeight/2));
    this->addChild(buttonBg, 1000); // 设置高Z轴值，确保显示在顶层
    
    // 创建按钮文字，使用微软雅黑字体
    auto label = cocos2d::Label::createWithSystemFont("回 退", "Microsoft YaHei", 35);
    if (label) {
        label->setPosition(cocos2d::Vec2(xPos, yPos));
        label->setColor(cocos2d::Color3B::WHITE);
        label->setAlignment(cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
        this->addChild(label, 1001); // Z轴值比背景高，确保文字显示在背景上面
    }
    
    // 初始化触摸事件监听器
    auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    
    // 触摸开始事件处理
    touchListener->onTouchBegan = [xPos, yPos, buttonWidth, buttonHeight](cocos2d::Touch* touch, cocos2d::Event* event) {
        auto touchPos = touch->getLocation();
        float halfWidth = buttonWidth / 2;
        float halfHeight = buttonHeight / 2;
        
        // 检查触摸点是否在按钮范围内
        return touchPos.x >= xPos - halfWidth && touchPos.x <= xPos + halfWidth &&
               touchPos.y >= yPos - halfHeight && touchPos.y <= yPos + halfHeight;
    };
    
    // 触摸结束事件处理
    touchListener->onTouchEnded = [this](cocos2d::Touch* touch, cocos2d::Event* event) {
        this->onUndoButtonClicked(nullptr);
    };
    
    // 将监听器添加到事件分发器
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// ========== 游戏视图更新方法 ==========

// 更新游戏视图
// 参数：gameModel - 游戏数据模型
//
// 工作流程：
// 1. 检查游戏模型是否有效
// 2. 遍历所有卡牌
// 3. 查找对应的卡牌视图
// 4. 更新卡牌视图的位置、可见性、顶手牌状态和可点击性
//
// 使用场景：
// - 撤销/重做操作后更新视图
// - 游戏状态发生变化时更新视图
void GameView::updateGameView(const models::GameModel* gameModel) {
    if (!gameModel) {
        return;
    }
    
    // 获取顶手牌ID
    int topCardId = gameModel->getTopHandCardId();
    
    // 遍历所有卡牌
    for (const auto& pair : gameModel->getAllCards()) {
        const models::CardModel* cardModel = pair.second;
        CardView* cardView = getCardView(cardModel->getId());
        
        if (cardView) {
            // 根据卡牌类型和是否为顶手牌设置位置
            cocos2d::Vec2 position;
            int cardId = cardModel->getId();
            
            // 桌面牌（卡牌ID <= 6）
            if (cardId <= 6) {
                // 使用模型中的位置
                position = cardModel->getPosition();
            }
            // 手牌（卡牌ID > 6）
            else {
                // 顶手牌放在最右边
                if (cardId == topCardId) {
                    // 顶手牌 - 紫色区域最右边
                    position.x = 800.0f;
                    position.y = 290.0f;
                }
                // 普通手牌放在左边，y轴相同，x轴错开
                else {
                    // 根据卡牌ID设置不同的x坐标，y坐标相同
                    if (cardId == 7) {
                        // 手牌1 - 左边第一张
                        position.x = 300.0f;
                        position.y = 290.0f;
                    } else if (cardId == 8) {
                        // 手牌2 - 左边第二张
                        position.x = 350.0f;
                        position.y = 290.0f;
                    } else if (cardId == 9) {
                        // 手牌3 - 左边第三张
                        position.x = 400.0f;
                        position.y = 290.0f;
                    } else {
                        // 其他手牌，默认放在左边
                        position.x = 0.0f + (cardId - 7) * 100.0f;
                        position.y = 290.0f;
                    }
                }
            }
            
            // 更新卡牌位置
            cardView->setPosition(position);
            
            // 更新卡牌可见性
            cardView->setVisible(cardModel->isVisible());
            
            // 更新卡牌顶手牌状态
            cardView->setAsTopCard(cardModel->isTopCard());
            
            // 更新卡牌Z轴值（层级）- 重要：撤销/重做时需要恢复层级
            this->reorderChild(cardView, cardModel->getZOrder());
            
            // 设置卡牌可点击
            cardView->setClickable(true);
        }
    }
}

// ========== 卡牌视图管理方法 ==========

// 添加卡牌视图
// 参数：cardView - 要添加的卡牌视图
//
// 工作流程：
// 1. 检查卡牌视图是否有效
// 2. 获取卡牌ID
// 3. 移除卡牌视图的现有父节点
// 4. 设置卡牌点击回调
// 5. 根据卡牌ID计算Z轴值
// 6. 设置卡牌Z轴值
// 7. 将卡牌视图添加到场景
// 8. 将卡牌视图添加到映射表
//
// 使用场景：
// - 初始化游戏时添加卡牌视图
// - 从序列化数据恢复时添加卡牌视图
void GameView::addCardView(CardView* cardView) {
    if (!cardView) {
        return;
    }
    
    int cardId = cardView->getCardId();
    
    // 如果卡牌视图已有父节点，先移除
    if (cardView->getParent()) {
        cardView->removeFromParent();
    }
    
    // 设置卡牌点击回调
    cardView->setClickCallback(_cardClickCallback);
    
    // 计算卡牌Z轴值
    int zOrder = 0;
    
    // 卡牌ID <= 6 - 桌面卡牌
    if (cardId <= 6) {
        // 分为两列，每列3张牌
        int columnIndex = (cardId - 1) % 3;
        // 根据卡牌在列中的位置设置Z轴值
        zOrder = 100 + columnIndex;
    } 
    // 手牌（卡牌ID > 6）
    else {
        // 根据卡牌ID设置Z轴值
        zOrder = 50 + (cardId - 7);
    }
    
    // 将卡牌视图添加到场景，位置将在updateGameView中设置
    this->addChild(cardView, zOrder);
    
    // 将卡牌视图添加到映射表
    _cardViews[cardId] = cardView;
    
    // 重要：将zOrder保存到CardModel中，用于撤销/重做时恢复层级
    if (_gameModel) {
        models::CardModel* cardModel = const_cast<models::CardModel*>(_gameModel->getCardById(cardId));
        if (cardModel) {
            cardModel->setZOrder(zOrder);
        }
    }
}

// 移除卡牌视图
// 参数：cardId - 要移除的卡牌ID
//
// 工作流程：
// 1. 在映射表中查找卡牌视图
// 2. 如果找到：
//    a. 从场景中移除卡牌视图
//    b. 从映射表中移除卡牌视图
//
// 使用场景：
// - 卡牌被匹配后移除视图
// - 重置游戏时移除所有卡牌视图
void GameView::removeCardView(int cardId) {
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        it->second->removeFromParent();
        _cardViews.erase(it);
    }
}

// 获取卡牌视图
// 参数：cardId - 卡牌ID
// 返回：找到的卡牌视图，未找到返回nullptr
//
// 工作流程：
// 1. 在映射表中查找卡牌视图
// 2. 找到返回卡牌视图，否则返回nullptr
//
// 使用场景：
// - 更新卡牌视图时
// - 播放动画时
CardView* GameView::getCardView(int cardId) const {
    auto it = _cardViews.find(cardId);
    if (it != _cardViews.end()) {
        return it->second;
    }
    return nullptr;
}

// 更新卡牌视图
// 参数：cardModel - 卡牌数据模型
//
// 工作流程：
// 1. 检查卡牌模型是否有效
// 2. 查找对应的卡牌视图
// 3. 使用模型更新卡牌视图
//
// 使用场景：
// - 卡牌数据发生变化时更新视图
// - 从序列化数据恢复时更新视图
void GameView::updateCardView(const models::CardModel* cardModel) {
    if (!cardModel) {
        return;
    }
    
    CardView* cardView = getCardView(cardModel->getId());
    if (cardView) {
        cardView->updateWithModel(cardModel);
    }
}

// ========== 动画方法 ==========

// 播放卡牌匹配动画
// 参数：
//   topCardId - 顶手牌ID
//   tableCardId - 桌面牌ID
//
// 工作流程：
// 1. 获取顶手牌和桌面牌的视图
// 2. 检查视图是否有效
// 3. 获取顶手牌的位置
// 4. 桌面牌移动到顶手牌位置
// 5. 隐藏顶手牌
//
// 使用场景：
// - 卡牌匹配成功时播放
void GameView::playMatchAnimation(int topCardId, int tableCardId) {
    CardView* topCardView = getCardView(topCardId);
    CardView* tableCardView = getCardView(tableCardId);
    
    if (!topCardView || !tableCardView) {
        return;
    }
    
    // 获取顶手牌的位置
    cocos2d::Vec2 topPosition = topCardView->getPosition();
    
    // 桌面牌移动到顶手牌位置
    tableCardView->moveToPosition(topPosition, 0.3f);
    // 隐藏顶手牌
    topCardView->setVisible(false);
}

// 播放卡牌替换动画
// 参数：
//   clickedCardId - 被点击的卡牌ID
//   topCardId - 当前顶手牌ID
//
// 工作流程：
// 1. 获取被点击卡牌和顶手牌的视图
// 2. 检查视图是否有效
// 3. 获取顶手牌的位置
// 4. 被点击卡牌移动到顶手牌位置
// 5. 隐藏当前顶手牌
//
// 使用场景：
// - 用户点击手牌时播放
void GameView::playReplaceAnimation(int clickedCardId, int topCardId) {
    CardView* clickedCardView = getCardView(clickedCardId);
    CardView* topCardView = getCardView(topCardId);
    
    if (!clickedCardView || !topCardView) {
        return;
    }
    
    // 获取顶手牌的位置
    cocos2d::Vec2 topPosition = topCardView->getPosition();
    
    // 被点击卡牌移动到顶手牌位置
    clickedCardView->moveToPosition(topPosition, 0.5f);
    // 隐藏当前顶手牌
    topCardView->setVisible(false);
}

// 播放撤销动画
// 参数：
//   cardId - 卡牌ID
//   originalPos - 原始位置
//
// 工作流程：
// 1. 获取卡牌视图
// 2. 检查视图是否有效
// 3. 卡牌移动到原始位置
//
// 使用场景：
// - 撤销操作时播放
void GameView::playUndoAnimation(int cardId, const cocos2d::Vec2& originalPos) {
    CardView* cardView = getCardView(cardId);
    if (!cardView) {
        return;
    }
    
    // 卡牌移动到原始位置
    cardView->moveToPosition(originalPos, 0.3f);
}

// ========== 其他方法 ==========

// 更新顶手牌信息
// 参数：topCard - 顶手牌数据模型
//
// 功能说明：
// - 预留方法，用于更新顶手牌信息显示
void GameView::updateTopCardInfo(const models::CardModel* topCard) {
    // 预留方法，用于更新顶手牌信息显示
}

// ========== 按钮点击事件处理 ==========

// 撤销按钮点击事件
// 参数：sender - 发送事件的对象
//
// 工作流程：
// 1. 检查撤销回调是否有效
// 2. 如果有效，调用撤销回调
//
// 使用场景：
// - 用户点击撤销按钮时触发
void GameView::onUndoButtonClicked(cocos2d::Ref* sender) {
    if (_undoCallback) {
        _undoCallback();
    }
}

// 重做按钮点击事件
// 参数：sender - 发送事件的对象
//
// 工作流程：
// 1. 检查重做回调是否有效
// 2. 如果有效，调用重做回调
//
// 使用场景：
// - 用户点击重做按钮时触发
void GameView::onRedoButtonClicked(cocos2d::Ref* sender) {
    if (_redoCallback) {
        _redoCallback();
    }
}

// 重置按钮点击事件
// 参数：sender - 发送事件的对象
//
// 工作流程：
// 1. 检查重置回调是否有效
// 2. 如果有效，调用重置回调
//
// 使用场景：
// - 用户点击重置按钮时触发
void GameView::onResetButtonClicked(cocos2d::Ref* sender) {
    if (_resetCallback) {
        _resetCallback();
    }
}

// 设置初始布局
//
// 工作流程：
// 1. 记录日志
// 2. 预留方法，用于设置初始布局
//
// 使用场景：
// - 初始化游戏时调用
void GameView::setupInitialLayout() {
    // 记录日志
    CCLOG("GameView::setupInitialLayout - Setting up initial layout");
    
    // 预留方法，用于设置初始布局
}

// 播放入场动画
//
// 工作流程：
// 1. 记录日志
// 2. 遍历所有卡牌视图
// 3. 设置卡牌初始状态（位置、透明度、缩放）
// 4. 创建动画动作（移动、淡入、缩放）
// 5. 添加缓动效果
// 6. 根据卡牌ID设置延迟时间
// 7. 执行动画序列
//
// 使用场景：
// - 游戏开始时播放入场动画
void GameView::playEntryAnimation() {
    // 记录日志
    CCLOG("GameView::playEntryAnimation - Playing entry animation");
    
    // 遍历所有卡牌视图
    for (const auto& pair : _cardViews) {
        CardView* cardView = pair.second;
        if (cardView) {
            // 获取卡牌当前位置
            cocos2d::Vec2 currentPos = cardView->getPosition();
            
            // 设置卡牌初始状态
            cardView->setPosition(currentPos.x, -200.0f); // 屏幕下方
            cardView->setOpacity(0); // 完全透明
            cardView->setScale(0.5f); // 缩小一半
            
            // 创建动画动作
            auto moveTo = cocos2d::MoveTo::create(0.8f, currentPos); // 移动到目标位置
            auto fadeIn = cocos2d::FadeIn::create(0.8f); // 淡入
            auto scaleTo = cocos2d::ScaleTo::create(0.8f, 0.6f); // 恢复原始大小
            
            // 添加缓动效果
            auto easeMove = cocos2d::EaseBackOut::create(moveTo); // 回弹效果
            auto easeScale = cocos2d::EaseBackOut::create(scaleTo); // 回弹效果
            
            // 组合动画
            auto spawn = cocos2d::Spawn::createWithTwoActions(fadeIn, cocos2d::Spawn::createWithTwoActions(easeMove, easeScale));
            
            // 根据卡牌ID设置延迟时间
            float delay = pair.first * 0.15f; // 每张牌延迟0.15秒
            auto delayAction = cocos2d::DelayTime::create(delay);
            auto sequence = cocos2d::Sequence::create(delayAction, spawn, nullptr);
            
            // 执行动画
            cardView->runAction(sequence);
        }
    }
}

} // namespace views
