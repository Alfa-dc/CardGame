// CardView.cpp
// 卡牌视图类实现
// 
// 这个文件实现了CardView类的所有方法
// CardView是卡牌在屏幕上的视觉表现，负责显示和用户交互
//
// 主要实现内容：
// 1. 构造函数和析构函数 - 创建和销毁卡牌视图
// 2. 工厂方法 - 从模型创建卡牌视图
// 3. 初始化方法 - 初始化卡牌的视觉组件和触摸事件
// 4. 更新方法 - 根据模型更新卡牌的外观
// 5. 触摸事件处理 - 处理用户的点击和触摸
// 6. 动画方法 - 实现卡牌的移动、缩放等动画效果
//
// 核心概念：
// - 视觉组件：使用Sprite显示卡牌的背景、数字和花色
// - 触摸事件：使用EventListenerTouchOneByOne处理单点触摸
// - 动画系统：使用Cocos2d-x的Action系统实现平滑动画
// - 回调机制：通过回调函数通知控制器卡牌被点击
#include "CardView.h"
#include "configs/models/CardConfig.h"

namespace views {

// ========== 构造函数和析构函数 ==========

// 构造函数
// 初始化所有成员变量为默认值
//
// 初始化状态：
// - _cardId: -1 (无效ID)
// - _isClickable: true (可点击)
// - _isMoving: false (静止)
// - _touchListener: nullptr (未初始化)
// - _scaleFactor: 0.6f (默认缩放比例)
// - 所有视觉组件: nullptr (未创建)
CardView::CardView() : 
    _cardId(-1), 
    _isClickable(true), 
    _isMoving(false), 
    _touchListener(nullptr),
    _scaleFactor(0.6f), 
    _background(nullptr),
    _numberSprite(nullptr),
    _bigNumberSprite(nullptr),
    _topRightSuitSprite(nullptr) {
}

// 析构函数
// 清理卡牌视图的所有资源
//
// 功能说明：
// - 移除触摸事件监听器
// - Cocos2d-x会自动管理子节点的内存，所以不需要手动删除Sprite
CardView::~CardView() {
    // 移除触摸事件监听器
    if (_touchListener) {
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);// 移除触摸事件监听器
    }
}

// ========== 工厂方法 ==========

// 使用模型创建卡牌视图
// 参数：cardModel - 卡牌数据模型
// 返回：创建的CardView对象，失败返回nullptr
//
// 工作流程：
// 1. 创建一个新的CardView对象
// 2. 使用模型初始化视图
// 3. 如果成功，设置自动释放并返回对象
// 4. 如果失败，删除对象并返回nullptr
//
// 使用场景：
// - 初始化游戏时创建所有卡牌视图
// - 从序列化数据恢复时创建卡牌视图
CardView* CardView::createWithModel(const models::CardModel* cardModel) {
    CardView* cardView = new (std::nothrow) CardView();
    if (cardView && cardView->initWithModel(cardModel)) {
        cardView->autorelease();
        return cardView;
    }
    CC_SAFE_DELETE(cardView);
    return nullptr;
}

// ========== 初始化方法 ==========

// 使用模型初始化卡牌视图
// 参数：cardModel - 卡牌数据模型
// 返回：成功返回true，失败返回false
//
// 工作流程：
// 1. 检查模型指针是否有效
// 2. 保存卡牌ID
// 3. 调用父类Node的init方法
// 4. 初始化卡牌的视觉组件
// 5. 设置卡牌的初始位置
// 6. 设置卡牌的缩放比例
// 7. 初始化触摸事件监听器
// 8. 使用模型更新卡牌视图
//
// 使用场景：
// - createWithModel内部调用此方法
bool CardView::initWithModel(const models::CardModel* cardModel) {
    // 检查模型指针是否有效
    if (!cardModel) {
        return false;
    }
    
    // 保存卡牌ID
    _cardId = cardModel->getId();
    
    // 调用父类Node的init方法
    if (!Node::init()) {
        return false;
    }
    
    // 初始化卡牌的视觉组件
    initCardComponents();
    
    // 设置卡牌的初始位置
    setPosition(cardModel->getPosition());
    
    // 设置卡牌的缩放比例
    float baseScale = 0.6f; 
    _scaleFactor = baseScale;
    setScale(_scaleFactor);
    
    // 初始化触摸事件监听器
    initTouchEvents();
    
    // 使用模型更新卡牌视图
    updateWithModel(cardModel);
    
    return true;
}

// 初始化卡牌的视觉组件
//
// 工作流程：
// 1. 创建背景精灵并设置锚点
// 2. 创建左上角数字精灵
// 3. 创建右下角大号数字精灵
// 4. 创建右下角花色精灵
// 5. 设置每个精灵的位置和锚点
//
// 使用场景：
// - initWithModel内部调用此方法
void CardView::initCardComponents() {
    // 创建背景精灵
    _background = cocos2d::Sprite::create("res/card_general.png");
    if (_background) {
        _background->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
        this->addChild(_background);
        // 设置节点内容大小为背景精灵的大小
        this->setContentSize(_background->getContentSize());//182*282的像素
        
    } else {
        // 如果背景创建失败，使用默认大小
        this->setContentSize(cocos2d::Size(100, 150));
    }
    
    // 创建左上角数字精灵
    _numberSprite = cocos2d::Sprite::create();
    if (_numberSprite) {
        _numberSprite->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));//
        _numberSprite->setPosition(cocos2d::Vec2(-60, 100));
        this->addChild(_numberSprite);

    }
    
    // 创建右下角大号数字精灵
    _bigNumberSprite = cocos2d::Sprite::create();
    if (_bigNumberSprite) {
        _bigNumberSprite->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));//
        _bigNumberSprite->setPosition(cocos2d::Vec2(0, 0));
        this->addChild(_bigNumberSprite);
    }
    
    // 创建右下角花色精灵
    _topRightSuitSprite = cocos2d::Sprite::create();
    if (_topRightSuitSprite) {
        _topRightSuitSprite->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
        _topRightSuitSprite->setPosition(cocos2d::Vec2(60, 100));
        this->addChild(_topRightSuitSprite);
    }
}

// ========== 更新方法 ==========

// 使用模型更新卡牌视图
// 参数：cardModel - 卡牌数据模型
//
// 工作流程：
// 1. 检查模型指针是否有效
// 2. 更新卡牌位置
// 3. 更新顶手牌状态
// 4. 更新可见性
// 5. 更新背景纹理
// 6. 更新数字和花色纹理
//
// 使用场景：
// - 卡牌数据发生变化时更新视图
// - 撤销/重做操作后恢复卡牌状态
void CardView::updateWithModel(const models::CardModel* cardModel) {
    // 检查模型指针是否有效
    if (!cardModel) {
        return;
    }
    
    // 更新卡牌位置
    setPosition(cardModel->getPosition());
    
    // 更新顶手牌状态
    setAsTopCard(cardModel->isTopCard());
    
    // 更新可见性
    this->setVisible(cardModel->isVisible());
    
    // 更新背景纹理
    if (_background) {
        _background->setTexture(cardModel->getTextureName());
    }
    
    // 获取花色和数字的纹理路径
    std::string suitTexture = configs::CardConfig::getSuitTexturePath(cardModel->getSuit());
    std::string smallNumberTexture = configs::CardConfig::getSmallNumberTexturePath(cardModel->getSuit(), cardModel->getRank());
    std::string bigNumberTexture = configs::CardConfig::getBigNumberTexturePath(cardModel->getSuit(), cardModel->getRank());
    
    // 更新左上角数字精灵
    if (_numberSprite) {
        _numberSprite->setTexture(smallNumberTexture);
        _numberSprite->setScale(1.0f); 
    }
    
    // 更新右下角大号数字精灵
    if (_bigNumberSprite) {
        _bigNumberSprite->setTexture(bigNumberTexture);
        _bigNumberSprite->setScale(1.0f); 
    }
    
    // 更新右下角花色精灵
    if (_topRightSuitSprite) {
        _topRightSuitSprite->setTexture(suitTexture);
        _topRightSuitSprite->setScale(1.0f); 
        _topRightSuitSprite->setRotation(0.0f); // 不旋转
    }
}

// ========== 触摸事件处理 ==========

// 初始化触摸事件监听器
//
// 工作流程：
// 1. 创建单点触摸监听器
// 2. 设置吞噬触摸事件（防止事件传递到下层）
// 3. 设置触摸开始和结束的回调函数
// 4. 将监听器添加到事件分发器
//
// 使用场景：
// - initWithModel内部调用此方法
void CardView::initTouchEvents() {
    if (!_touchListener) {
        _touchListener = cocos2d::EventListenerTouchOneByOne::create();
        _touchListener->setSwallowTouches(true);
        
        // 设置触摸开始和结束的回调
        _touchListener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
        
        // 将监听器添加到事件分发器
        cocos2d::Director::getInstance()->getEventDispatcher()
            ->addEventListenerWithSceneGraphPriority(_touchListener, this);
    }
}

// 触摸开始事件
// 参数：
//   touch - 触摸对象
//   event - 事件对象
// 返回：true表示接受触摸事件，false表示忽略
//
// 工作流程：
// 1. 检查卡牌是否可点击且不在移动中
// 2. 获取触摸点的屏幕坐标
// 3. 以卡牌中心点为基础，建立方形点击范围
// 4. 检查触摸点是否在范围内
// 5. 如果在范围内，播放点击效果并接受触摸事件
bool CardView::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
    // 检查卡牌是否可点击且不在移动中
    if (!_isClickable || _isMoving) {
        return false;
    }
    
    // 获取触摸点的屏幕坐标
    cocos2d::Vec2 location = touch->getLocation();
    
    // 获取卡牌中心点的屏幕坐标
    cocos2d::Vec2 cardCenter = this->getPosition();
    
    // 设置点击范围大小（以卡牌实际大小为点击范围，不放大）
    float clickRangeWidth = this->getContentSize().width * _scaleFactor * 0.3f;
    float clickRangeHeight = this->getContentSize().height * _scaleFactor * 0.3f;
    
    // 检查触摸点是否在以卡牌中心点为基础的方形范围内
    if (abs(location.x - cardCenter.x) <= clickRangeWidth && 
        abs(location.y - cardCenter.y) <= clickRangeHeight) {
        // 播放点击效果
        playClickEffect();
        return true;
    }
    
    return false;
}

// 触摸结束事件
// 参数：
//   touch - 触摸对象
//   event - 事件对象
//
// 工作流程：
// 1. 恢复卡牌的原始大小
// 2. 调用点击回调函数
//
// 使用场景：
// - 用户点击卡牌时通知控制器
void CardView::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
    // 恢复卡牌的原始大小
    setScale(_scaleFactor);
    
    // 调用点击回调函数
    if (_clickCallback) {
        _clickCallback(this);
    }
}

// ========== 动画方法 ==========

// 移动卡牌到指定位置
// 参数：
//   target - 目标位置
//   duration - 移动持续时间（秒）
//   callback - 移动完成后的回调函数
//
// 工作流程：
// 1. 检查卡牌是否正在移动，如果是则直接返回
// 2. 设置移动标志为true
// 3. 创建MoveTo动作
// 4. 创建CallFunc动作，在移动完成后执行
// 5. 使用Sequence组合两个动作并执行
//
// 使用场景：
// - 卡牌匹配成功后移动到目标位置
// - 卡牌从手牌堆移动到桌面
void CardView::moveToPosition(const cocos2d::Vec2& target, float duration, const std::function<void()>& callback) {
    // 检查卡牌是否正在移动
    if (_isMoving) {
        return;
    }
    
    // 设置移动标志
    _isMoving = true;
    
    // 创建移动动作
    auto move = cocos2d::MoveTo::create(duration, target);
    
    // 创建完成回调动作
    auto finish = cocos2d::CallFunc::create([this, target, callback]() {
        // 确保卡牌位置准确
        this->setPosition(target);
        _isMoving = false;
        if (callback) {
            callback();
        }
    });
    
    // 执行动作序列
    this->runAction(cocos2d::Sequence::create(move, finish, nullptr));
}

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
void CardView::setSelected(bool selected) {
    // 设置缩放比例
    float scale = selected ? 1.2f : _scaleFactor;
    this->runAction(cocos2d::ScaleTo::create(0.1f, scale));
}

// 设置卡牌为顶手牌状态
// 参数：isTop - true表示是顶手牌，false表示不是
//
// 功能说明：
// - 顶手牌设置更高的Z轴值，显示在前面
// - 非顶手牌恢复原始Z轴值
// - 恢复原始缩放比例
//
// 使用场景：
// - 手牌堆最上面的牌设置为顶手牌
// - 用户点击手牌时将其设置为顶手牌
void CardView::setAsTopCard(bool isTop) {
    // 恢复原始缩放比例
    float baseScale = 0.6f; // 基础缩放比例
    _scaleFactor = baseScale; // 重置缩放因子
    setLocalZOrder(isTop ? 100 : 0); // 顶手牌显示在前面
    setScale(_scaleFactor);
}

// 播放卡牌点击效果
//
// 功能说明：
// - 卡牌放大20%
// - 保持0.5秒钟
// - 恢复原始大小
// - 使用Sequence组合多个动作
//
// 使用场景：
// - 用户点击卡牌时提供视觉反馈
void CardView::playClickEffect() {
    // 播放点击放大效果动画
    float clickScale = _scaleFactor * 1.2f; // 放大20%
    auto scaleTo = cocos2d::ScaleTo::create(0.1f, clickScale); // 0.1秒放大
    //auto delay = cocos2d::DelayTime::create(0.2f); // 保持0.5秒
    auto scaleBack = cocos2d::ScaleTo::create(0.05f, _scaleFactor); // 0.1秒恢复
    auto sequence = cocos2d::Sequence::create(scaleTo,scaleBack, nullptr);
    this->runAction(sequence);
}

} // namespace views
