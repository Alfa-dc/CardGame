#pragma execution_character_set("utf-8")

// GameController.cpp
// 游戏控制器实现文件 - 负责协调游戏各组件，管理游戏流程
//
// 这个类是整个游戏的核心控制器，负责：
// 1. 协调模型、视图和其他控制器
// 2. 管理游戏的整体流程（开始、重置、撤销、重做）
// 3. 处理用户交互（卡牌点击、按钮点击）
// 4. 实现游戏的核心逻辑（卡牌匹配、替换）
// 5. 管理撤销/重做功能

#include "GameController.h"
#include "configs/models/CardConfig.h"
#include "configs/models/LevelConfig.h"



namespace controllers {

// GameController构造函数
// 初始化所有成员变量为默认值
GameController::GameController() : 
    _gameModel(nullptr),           // 游戏模型 - 初始化为空
    _gameView(nullptr),            // 游戏视图 - 初始化为空
    _isPerformingUndoRedo(false),  // 是否正在执行撤销/重做 - 初始化为false
    _currentLevelId(1) {           // 当前关卡ID - 默认为1
}

// GameController析构函数
// 清理所有动态分配的资源，防止内存泄漏
GameController::~GameController() {
    // 安全释放游戏视图（使用CC_SAFE_RELEASE_NULL确保指针安全）
    CC_SAFE_RELEASE_NULL(_gameView);
    
    // 安全删除游戏模型
    CC_SAFE_DELETE(_gameModel);
    
}

// 初始化游戏控制器
// 返回：成功返回true，失败返回false
//
// 工作流程：
// 1. 初始化游戏控制器
// 2. 返回初始化结果
bool GameController::init() {
    return true;
}

// 初始化游戏视图
// 创建游戏视图并设置各种回调函数
void GameController:: initGameView() {
    // ========== 第一步：创建游戏视图 ==========
    // GameView是游戏的主场景，包含所有UI元素和卡牌
    _gameView = views::GameView::create();
    _gameView->retain(); // 保留引用，防止被自动释放
    
    // ========== 第二步：设置游戏模型引用 ==========
    // 重要：设置游戏模型引用，用于更新CardModel的zOrder
    _gameView->setGameModel(_gameModel);
    
    // ========== 第三步：设置卡牌点击回调 ==========
    // 当用户点击卡牌时，调用handleCardClick函数处理
    _gameView->setCardClickCallback([this](views::CardView* cardView) {
        this->handleCardClick(cardView);
    });
    
    // ========== 第四步：设置按钮点击回调 ==========
    // 撤销按钮回调
    _gameView->setUndoCallback([this]() {
        this->handleUndoButtonClick();
    });
    
    // 重做按钮回调
    _gameView->setRedoCallback([this]() {
        this->handleRedoButtonClick();
    });
    
    // 重置按钮回调
    _gameView->setResetCallback([this]() {
        this->handleResetButtonClick();
    });
    
    // ========== 第五步：设置初始布局 ==========
    // 设置游戏视图的初始UI布局
    _gameView->setupInitialLayout();
}

// 设置事件监听器
// 注册键盘事件监听器，用于快捷键操作
void GameController::setupEventListeners() {
    // ========== 创建键盘事件监听器 ==========
    auto keyboardListener = cocos2d::EventListenerKeyboard::create();
    
    // 设置按键按下回调
    keyboardListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        // 按Z键执行撤销操作
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_Z) {
            handleUndoButtonClick();
        }
        // 按Y键执行重做操作
        else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_Y) {
            handleRedoButtonClick();
        }
    };
    
    // 将监听器添加到事件分发器
    // 使用场景图优先级，确保监听器在游戏视图上生效
    cocos2d::Director::getInstance()->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(keyboardListener, _gameView);
}

// 开始游戏
// 参数：levelId - 关卡ID
//
// 工作流程：
// 1. 加载关卡配置
// 2. 生成游戏模型
// 3. 初始化控制器
// 4. 初始化游戏视图
// 5. 初始化视图
// 6. 设置事件监听器
// 7. 创建所有卡牌视图
// 8. 更新UI
// 9. 播放入场动画
// 10. 运行场景
void GameController::startGame(int levelId) {
    // ========== 第一步：保存当前关卡ID ==========
    _currentLevelId = levelId;
    
    // ========== 第二步：加载关卡配置 ==========
    // 使用关卡配置加载器加载指定关卡的配置
    loaders::LevelConfigLoader* levelConfigLoader = loaders::LevelConfigLoader::getInstance();
    configs::LevelConfig* levelConfig = levelConfigLoader->loadLevelConfig(levelId);
    
    if (!levelConfig) {
        CCLOG("Failed to load level config for level %d", levelId);
        return;
    }
    
    // ========== 第三步：生成游戏模型 ==========
    // 使用游戏模型生成器根据关卡配置生成游戏模型
    generators::GameModelFromLevelGenerator* generator = generators::GameModelFromLevelGenerator::getInstance();
    _gameModel = generator->generateGameModel(*levelConfig);
    
    if (!_gameModel) {
        CCLOG("Failed to generate game model for level %d", levelId);
        return;
    }
    
    // ========== 第四步：初始化游戏视图 ==========
    initGameView();
    
    // ========== 第七步：设置事件监听器 ==========
    setupEventListeners();
    
    // ========== 第八步：创建所有卡牌视图 ==========
    // 遍历游戏模型中的所有卡牌，为每张卡牌创建对应的视图
    for (const auto& pair : _gameModel->getAllCards()) {
        views::CardView* cardView = views::CardView::createWithModel(pair.second);
        if (cardView) {
            _gameView->addCardView(cardView);
            
            // 重要：将zOrder保存到CardModel中，用于撤销/重做时恢复层级
            models::CardModel* cardModel = const_cast<models::CardModel*>(pair.second);
            if (cardModel) {
                cardModel->setZOrder(cardView->getLocalZOrder());
            }
        }
    }
    
    // ========== 第九步：更新UI ==========
    updateUI();
    
    // ========== 第十步：播放入场动画 ==========
    _gameView->playEntryAnimation();
    
    // ========== 第十一步：运行场景 ==========
    // 将游戏视图作为主场景运行
    cocos2d::Director::getInstance()->runWithScene(_gameView);
}

// 处理卡牌点击事件
// 参数：cardView - 被点击的卡牌视图
// 返回：成功处理返回true，失败返回false
//
// 工作流程：
// 1. 检查是否正在执行撤销/重做
// 2. 获取被点击的卡牌模型
// 3. 根据卡牌类型执行不同操作：
//    - 手牌：替换顶手牌
//    - 桌面牌：匹配卡牌
bool GameController::handleCardClick(views::CardView* cardView) {
    // 检查卡牌视图是否有效，以及是否正在执行撤销/重做
    if (!cardView || _isPerformingUndoRedo) {
        return false;
    }
    
    // 获取被点击的卡牌ID和模型
    int cardId = cardView->getCardId();
    models::CardModel* cardModel = _gameModel->getCardById(cardId);
    
    // 检查卡牌模型是否存在
    if (!cardModel) {
        return false;
    }
    
    // 获取当前顶手牌ID
    int topCardId = _gameModel->getTopHandCardId();
    
    // 根据卡牌类型执行不同操作
    if (cardModel->getType() == configs::CardConfig::CardType::HAND_CARD) {
        // 情况1：点击的是手牌 - 执行替换顶手牌操作
        replaceTopHandCard(cardId);
        return true;
    }
    else if (cardModel->getType() == configs::CardConfig::CardType::TABLE_CARD) {
        // 情况2：点击的是桌面牌 - 执行匹配卡牌操作
        matchTableCard(cardId);
        return true;
    }
    
    return false;
}

// 替换顶手牌
// 参数：clickedCardId - 被点击的手牌ID
//
// 功能说明：
// 1. 检查是否点击的是当前顶手牌
// 2. 播放替换动画
// 3. 执行替换命令（记录到撤销历史）
// 4. 更新UI
void GameController::replaceTopHandCard(int clickedCardId) {
    // 获取当前顶手牌ID
    int topCardId = _gameModel->getTopHandCardId();
    
    // 如果点击的就是当前顶手牌，则不需要替换
    if (clickedCardId == topCardId) {
        return;
    }
    
    // 播放替换动画（将点击的牌移到顶部，原顶手牌移到下面）
    _gameView->playReplaceAnimation(clickedCardId, topCardId);
    
    // 执行替换顶手牌命令（使用命令服务，支持撤销/重做）
    services::CommandService::getInstance()->executeReplaceTopCardCommand(clickedCardId, _gameModel);
    
    // 更新UI显示
    updateUI();
}

// 匹配桌面卡牌
// 参数：tableCardId - 桌面卡牌ID
//
// 功能说明：
// 1. 检查顶手牌和桌面牌是否可以匹配
// 2. 播放匹配动画
// 3. 执行匹配命令（记录到撤销历史）
// 4. 更新UI
void GameController::matchTableCard(int tableCardId) {
    // 获取顶手牌ID
    int topCardId = _gameModel->getTopHandCardId();
    
    // 检查顶手牌和桌面牌是否可以匹配
    // 匹配规则：点数相差1（例如：3和4可以匹配）
    if (!isCardsMatchable(topCardId, tableCardId)) {
        // 不能匹配，输出日志并返回
        CCLOG("Cards not matchable!");
        return;
    }
    
    // 播放匹配动画（顶手牌飞向桌面牌，两张牌消失）
    _gameView->playMatchAnimation(topCardId, tableCardId);
    
    // 执行匹配桌面卡牌命令（使用命令服务，支持撤销/重做）
    services::CommandService::getInstance()->executeMatchTableCardCommand(tableCardId, _gameModel);
    
    // 更新UI显示
    updateUI();
}

// 检查两张卡牌是否可以匹配
// 参数：
//   tableCardId - 桌面卡牌ID
//   handCardId - 手牌ID
// 返回：可以匹配返回true，不能匹配返回false
//
// 匹配规则：两张牌的点数相差1
// 例如：3和4可以匹配，5和5不能匹配
bool GameController::isCardsMatchable(int tableCardId, int handCardId) {
    // 获取两张卡牌的模型
    models::CardModel* tableCard = _gameModel->getCardById(tableCardId);
    models::CardModel* handCard = _gameModel->getCardById(handCardId);
    
    // 检查卡牌是否存在
    if (!tableCard || !handCard) {
        return false;
    }
    
    // 计算点数差值
    int rankDiff = abs(tableCard->getRank() - handCard->getRank());
    
    // 点数差值为1则可以匹配
    return rankDiff == 1;
}

// 更新UI显示
// 根据当前游戏模型更新所有UI元素的状态
void GameController::updateUI() {
    // 更新游戏视图（包括卡牌位置、选中状态等）
    _gameView->updateGameView(_gameModel);
    
    // UI控制器会根据游戏模型的状态更新按钮的可用状态
    // 例如：如果没有可撤销的操作，撤销按钮会被禁用
}

// 处理撤销按钮点击
void GameController::handleUndoButtonClick() {
    executeUndo();
}

// 处理重做按钮点击
void GameController::handleRedoButtonClick() {
    executeRedo();
}

// 处理重置按钮点击
// 功能说明：
// 1. 重新开始当前关卡
// 2. 清空撤销/重做历史
// 3. 更新UI
void GameController::handleResetButtonClick() {
    // 重新开始当前关卡（使用保存的关卡ID）
    startGame(_currentLevelId);
    
    // 清空命令服务的历史记录
    services::CommandService::getInstance()->clearHistory();
    
    // 更新UI显示
    updateUI();
}

// 执行撤销操作
// 功能说明：
// 1. 检查是否正在执行撤销/重做
// 2. 执行撤销命令
// 3. 更新UI
// 4. 延迟一段时间后允许再次操作（防止快速连续操作）
void GameController::executeUndo() {
    // 如果正在执行撤销/重做，则不允许再次执行
    if (_isPerformingUndoRedo) {
        return;
    }
    
    // 设置标志位，表示正在执行撤销
    _isPerformingUndoRedo = true;
    
    // 执行撤销命令（从命令服务获取上一个操作记录）
    models::ActionRecord* record = services::CommandService::getInstance()->undo(_gameModel);
    
    if (record) {
        // 更新UI显示
        updateUI();
        
        // 删除操作记录（已经不需要了）
        delete record;
    }
    
    // 延迟0.3秒后重置标志位，防止用户快速连续点击
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(0.3f);
    cocos2d::CallFunc* callFunc = cocos2d::CallFunc::create([this]() {
        _isPerformingUndoRedo = false;
    });
    
    // 运行延迟动作
    _gameView->runAction(cocos2d::Sequence::create(delay, callFunc, nullptr));
}

// 执行重做操作
// 功能说明：
// 1. 检查是否正在执行撤销/重做
// 2. 执行重做命令
// 3. 更新UI
// 4. 延迟一段时间后允许再次操作（防止快速连续操作）
void GameController::executeRedo() {
    // 如果正在执行撤销/重做，则不允许再次执行
    if (_isPerformingUndoRedo) {
        return;
    }
    
    // 设置标志位，表示正在执行重做
    _isPerformingUndoRedo = true;
    
    // 执行重做命令（从命令服务获取下一个操作记录）
    models::ActionRecord* record = services::CommandService::getInstance()->redo(_gameModel);
    
    if (record) {
        // 更新UI显示
        updateUI();
        
        // 删除操作记录（已经不需要了）
        delete record;
    }
    
    // 延迟0.3秒后重置标志位，防止用户快速连续点击
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(0.3f);
    cocos2d::CallFunc* callFunc = cocos2d::CallFunc::create([this]() {
        _isPerformingUndoRedo = false;
    });
    
    // 运行延迟动作
    _gameView->runAction(cocos2d::Sequence::create(delay, callFunc, nullptr));
}

} // namespace controllers
