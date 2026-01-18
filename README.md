# 卡牌游戏程序设计文档

## 文档信息
- **项目名称**: 卡牌游戏
- **文档版本**: v1.0
- **编写日期**: 2026-01-18
- **架构模式**: MVC + 命令模式 + 单例模式

---

## 一、整体架构设计

### 1.1 架构分层

本项目采用经典的 **MVC（Model-View-Controller）架构**，结合**命令模式**和**单例模式**，实现了高内聚、低耦合的设计目标。

```
┌─────────────────────────────────────────────────────────────┐
│                    应用程序层                          │
│  ┌──────────────────────────────────────────────────┐    │
│  │ AppDelegate (应用程序入口)                   │    │
│  │ - 生命周期管理                               │    │
│  │ - 场景启动                                   │    │
│  └──────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                        ↓
┌─────────────────────────────────────────────────────────────┐
│                    控制层 (Controller)               │
│  ┌──────────────────────────────────────────────────┐    │
│  │ GameController (游戏控制器)                  │    │
│  │ - 协调Model和View                        │    │
│  │ - 处理用户交互                              │    │
│  │ - 管理游戏流程                              │    │
│  └──────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
         ↕                    ↕                    ↕
┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
│  模型层 (Model)│  │  视图层 (View)  │  │  服务层 (Service)│
├──────────────────┤  ├──────────────────┤  ├──────────────────┤
│ GameModel      │  │ GameView        │  │ CommandService   │
│ - 游戏状态   │  │ - 主场景        │  │ - 命令执行    │
│ - 卡牌管理   │  │ - UI管理        │  │ - 撤销/重做    │
├──────────────────┤  ├──────────────────┤  ├──────────────────┤
│ CardModel      │  │ CardView        │  │ CardService     │
│ - 卡牌数据   │  │ - 卡牌视图      │  │ - 业务逻辑      │
│ - 属性管理   │  │ - 触摸处理      │  │ - 工具方法      │
├──────────────────┤  └──────────────────┘  ├──────────────────┤
│ UndoModel     │                       │ LevelConfigLoader│
│ - 历史记录   │                       │ - 配置加载      │
└──────────────────┘                       └──────────────────┘
         ↕                    ↕                    ↕
┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
│ 配置层 (Config)│  │ 加载层 (Loader)  │  │ 生成层 (Generator)│
├──────────────────┤  ├──────────────────┤  ├──────────────────┤
│ CardConfig     │  │ LevelConfigLoader│  │ GameModelFrom    │
│ - 卡牌配置   │  │ - JSON加载     │  │ LevelGenerator   │
│ - 常量定义   │  │ - 缓存管理      │  │ - 模型生成      │
├──────────────────┤  └──────────────────┘  └──────────────────┘
│ LevelConfig    │
│ - 关卡配置   │
└──────────────────┘
```

### 1.2 设计模式应用

| 设计模式 | 应用位置 | 说明 |
|---------|----------|------|
| **单例模式** | LevelConfigLoader, GameModelFromLevelGenerator, CommandService, CardService | 确保全局唯一实例，统一访问入口 |
| **命令模式** | CommandService, UndoModel | 封装操作为命令对象，支持撤销/重做 |
| **MVC模式** | 整体架构 | 分离数据、视图和逻辑，提高可维护性 |
| **工厂模式** | CardModel::createFromConfig(), GameModelFromLevelGenerator | 根据配置创建对象，降低耦合 |
| **观察者模式** | 回调函数机制 | View通过回调通知Controller事件 |

---

## 二、核心模块职责

### 2.1 AppDelegate（应用程序入口）

**职责**：应用程序生命周期管理

**主要功能**：
- 初始化OpenGL渲染环境
- 创建游戏窗口
- 设置屏幕分辨率和帧率
- 启动游戏控制器
- 处理应用前后台切换

**关键函数**：
```cpp
bool applicationDidFinishLaunching()  // 应用启动入口
void applicationDidEnterBackground()    // 进入后台（暂停）
void applicationWillEnterForeground()  // 进入前台（恢复）
```

### 2.2 GameController（游戏控制器）

**职责**：协调Model和View，管理游戏流程

**主要功能**：
- 初始化游戏视图和事件监听器
- 处理卡牌点击事件
- 处理按钮点击事件（撤销、重做、重置）
- 管理游戏状态（开始、重置）
- 协调命令服务的执行

**关键函数**：
```cpp
bool init()                          // 初始化控制器
void startGame(int levelId)          // 启动游戏
void initGameView()                  // 初始化游戏视图
void setupEventListeners()             // 设置事件监听器
bool handleCardClick(CardView*)       // 处理卡牌点击
void replaceTopHandCard(int)          // 替换顶手牌
void matchTableCard(int)             // 匹配桌面牌
void handleUndoButtonClick()          // 处理撤销
void handleRedoButtonClick()          // 处理重做
void handleResetButtonClick()         // 处理重置
void updateUI()                     // 更新UI显示
```

### 2.3 GameView（游戏主场景）

**职责**：显示游戏场景，管理所有UI元素和卡牌视图

**主要功能**：
- 创建和管理游戏区域（桌面区、手牌区）
- 管理所有卡牌视图的添加、删除、更新
- 显示和管理UI按钮（撤销、重做、重置）
- 播放卡牌动画效果（匹配、替换、撤销）
- 处理按钮点击事件

**关键函数**：
```cpp
bool init()                              // 初始化场景
void initButtons()                        // 初始化按钮
void addCardView(CardView*)               // 添加卡牌视图
void removeCardView(int)                 // 移除卡牌视图
CardView* getCardView(int) const        // 获取卡牌视图
void updateCardView(const CardModel*)      // 更新卡牌视图
void updateGameView(const GameModel*)      // 更新游戏视图
void playMatchAnimation(int, int)        // 播放匹配动画
void playReplaceAnimation(int, int)      // 播放替换动画
void playUndoAnimation(int, const Vec2&)  // 播放撤销动画
void playEntryAnimation()                 // 播放入场动画
void setupInitialLayout()                 // 设置初始布局
void updateTopCardInfo(const CardModel*)   // 更新顶手牌信息
```

### 2.4 CardView（卡牌视图）

**职责**：显示单张卡牌，处理触摸事件

**主要功能**：
- 显示卡牌外观（背景、数字、花色）
- 处理触摸事件（点击检测）
- 执行卡牌移动动画
- 管理卡牌状态（选中、顶手牌、可点击）

**关键函数**：
```cpp
bool initWithModel(const CardModel*)         // 使用模型初始化
void updateWithModel(const CardModel*)      // 使用模型更新
void moveToPosition(const Vec2&, float, callback)  // 移动到目标位置
void setSelected(bool)                   // 设置选中状态
void setAsTopCard(bool)                 // 设置为顶手牌
void playClickEffect()                   // 播放点击效果
bool onTouchBegan(Touch*, Event*)         // 触摸开始
void onTouchEnded(Touch*, Event*)         // 触摸结束
```

### 2.5 GameModel（游戏数据模型）

**职责**：管理游戏状态和所有卡牌数据

**主要功能**：
- 管理所有卡牌的增删改查
- 管理手牌顺序和顶手牌
- 序列化和反序列化游戏状态
- 提供卡牌查询接口

**关键函数**：
```cpp
void addCard(CardModel*)                // 添加卡牌
void removeCard(int)                   // 移除卡牌
CardModel* getCardById(int)            // 根据ID获取卡牌
const unordered_map<int, CardModel*>& getAllCards()  // 获取所有卡牌
vector<CardModel*> getHandCards() const  // 获取手牌列表
vector<CardModel*> getTableCards() const // 获取桌面牌列表
void setTopHandCardId(int)            // 设置顶手牌
int getTopHandCardId() const          // 获取顶手牌ID
ValueMap serialize() const             // 序列化游戏状态
void deserialize(const ValueMap&)        // 反序列化游戏状态
```

### 2.6 CardModel（卡牌数据模型）

**职责**：管理单张卡牌的数据

**主要功能**：
- 存储卡牌属性（ID、花色、点数、类型）
- 管理卡牌位置、可见性、层级
- 提供属性查询接口

**关键函数**：
```cpp
static CardModel* createFromConfig(const CardData&)  // 从配置创建
int getId() const                              // 获取卡牌ID
Suit getSuit() const                            // 获取花色
int getRank() const                             // 获取点数
CardType getType() const                         // 获取类型
Vec2 getPosition() const                       // 获取位置
void setPosition(const Vec2&)                     // 设置位置
bool isVisible() const                           // 是否可见
void setVisible(bool)                            // 设置可见性
int getZOrder() const                           // 获取层级
void setZOrder(int)                             // 设置层级
```

### 2.7 CommandService（命令服务）

**职责**：实现命令模式，支持撤销/重做功能

**主要功能**：
- 执行游戏操作命令
- 记录操作历史
- 提供撤销/重做接口

**关键函数**：
```cpp
void executeReplaceTopCardCommand(int, GameModel*)  // 执行替换命令
void executeMatchTableCardCommand(int, GameModel*)   // 执行匹配命令
void undo(GameModel*)                           // 撤销操作
void redo(GameModel*)                           // 重做操作
bool canUndo() const                           // 是否可以撤销
bool canRedo() const                           // 是否可以重做
```

### 2.8 UndoModel（撤销模型）

**职责**：管理撤销/重做历史栈

**主要功能**：
- 记录操作历史
- 管理撤销栈和重做栈
- 限制历史记录数量

**关键函数**：
```cpp
void recordAction(const Action&)   // 记录操作
void undo()                     // 撤销
void redo()                     // 重做
bool canUndo() const             // 是否可以撤销
bool canRedo() const             // 是否可以重做
void clear()                    // 清空历史
```

### 2.9 CardService（卡牌服务）

**职责**：提供卡牌相关的业务逻辑和工具方法

**主要功能**：
- 从配置创建卡牌模型
- 检查卡牌是否可以匹配
- 提供卡牌属性查询接口

**关键函数**：
```cpp
CardModel* createCardModelFromConfig(const CardData&)  // 从配置创建
bool isCardsMatchable(const CardModel*, const CardModel*)  // 检查是否可匹配
bool isCardsMatchableByRank(int, int)              // 检查点数是否可匹配
string getCardTextureName(const Suit&, int)         // 获取纹理名称
string getSuitString(const Suit&)                    // 获取花色字符串
string getRankString(int)                            // 获取点数字符串
```

### 2.10 LevelConfigLoader（关卡配置加载器）

**职责**：加载和缓存关卡配置

**主要功能**：
- 从JSON文件加载关卡配置
- 缓存已加载的配置
- 生成配置文件路径

**关键函数**：
```cpp
LevelConfigLoader* getInstance()          // 获取单例
LevelConfig* loadLevelConfig(int)      // 加载关卡配置
string generateLevelConfigPath(int)      // 生成配置路径
```

### 2.11 GameModelFromLevelGenerator（游戏模型生成器）

**职责**：根据关卡配置生成游戏模型

**主要功能**：
- 解析关卡配置数据
- 创建所有卡牌对象
- 设置初始游戏状态

**关键函数**：
```cpp
GameModelFromLevelGenerator* getInstance()  // 获取单例
GameModel* generateGameModel(const LevelConfig&)  // 生成游戏模型
CardModel* createCardModelFromConfig(const CardData&)  // 创建卡牌模型
```

---

## 三、扩展性设计

### 3.1 如何新增一个卡牌

#### 场景1：在关卡配置中新增卡牌

**步骤**：

1. **修改JSON配置文件** (`Resources/level_config.json`)
```json
{
  "level_1": {
    "table_cards": [
      {
        "id": 100,
        "suit": "HEART",
        "rank": 2,
        "position": {"x": 200, "y": 1600}
      }
    ],
    "stack_cards": [
      {
        "id": 1,
        "suit": "DIAMOND",
        "rank": 3,
        "position": {"x": 100, "y": 100}
      }
    ]
  }
}
```

2. **添加卡牌纹理资源**
- 将卡牌图片放入 `Resources/res/` 目录
- 确保文件名符合命名规范：
  - 数字：`small_red_2.png`, `big_red_2.png`
  - 花色：`heart.png`, `diamond.png`, `club.png`, `spade.png`

3. **无需修改代码**
- 系统会自动加载新卡牌
- `GameModelFromLevelGenerator` 会根据配置自动创建卡牌对象
- `CardView` 会自动显示新卡牌

#### 场景2：在游戏运行时动态添加卡牌

**步骤**：

1. **创建卡牌配置数据**
```cpp
configs::CardConfig::CardData cardData;
cardData.id = newCardId;
cardData.suit = configs::CardConfig::Suit::HEART;
cardData.rank = 5;
cardData.position = cocos2d::Vec2(x, y);
cardData.type = configs::CardConfig::CardType::TABLE_CARD;
```

2. **创建卡牌模型**
```cpp
models::CardModel* newCard = models::CardModel::createFromConfig(cardData);
```

3. **添加到游戏模型**
```cpp
_gameModel->addCard(newCard);
```

4. **创建卡牌视图**
```cpp
views::CardView* cardView = views::CardView::createWithModel(newCard);
```

5. **添加到游戏视图**
```cpp
_gameView->addCardView(cardView);
```

6. **更新UI**
```cpp
_gameView->updateGameView(_gameModel);
```

**代码示例**：
```cpp
// 在GameController中添加新卡牌的方法
void GameController::addNewCard(int suit, int rank, const cocos2d::Vec2& position) {
    // 步骤1：创建卡牌配置
    configs::CardConfig::CardData cardData;
    cardData.id = _gameModel->getAllCards().size() + 1;  // 生成新ID
    cardData.suit = static_cast<configs::CardConfig::Suit>(suit);
    cardData.rank = rank;
    cardData.position = position;
    cardData.type = configs::CardConfig::CardType::TABLE_CARD;
    
    // 步骤2：创建卡牌模型
    models::CardModel* newCard = models::CardModel::createFromConfig(cardData);
    
    // 步骤3：添加到游戏模型
    _gameModel->addCard(newCard);
    
    // 步骤4：创建卡牌视图
    views::CardView* cardView = views::CardView::createWithModel(newCard);
    
    // 步骤5：添加到游戏视图
    _gameView->addCardView(cardView);
    
    // 步骤6：更新UI
    updateUI();
}
```

### 3.2 如何新增一个类型的回退功能

当前系统已支持**操作级回退**（撤销/重做），以下是如何扩展为**多类型回退**的设计方案。

#### 方案1：回退类型枚举扩展

**步骤1：扩展回退类型枚举**

在 `UndoModel.h` 中扩展回退类型：
```cpp
// 回退类型枚举
enum class UndoType {
    // 现有类型
    OPERATION,        // 操作回退（撤销/重做）
    
    // 新增类型
    LEVEL,            // 关卡回退（返回上一关）
    STATE,            // 状态回退（返回上一个保存点）
    BOOKMARK,         // 书签回退（跳转到指定书签）
    TIMELINE          // 时间线回退（跳转到指定时间点）
};
```

**步骤2：扩展UndoModel**

```cpp
class UndoModel {
public:
    // 原有方法
    void recordAction(const Action& action);
    void undo();
    void redo();
    
    // 新增方法
    void recordLevel(int levelId, const ValueMap& state);
    void undoLevel();
    void recordBookmark(const string& name, const ValueMap& state);
    void restoreBookmark(const string& name);
    void recordTimelinePoint(int timestamp, const ValueMap& state);
    void restoreTimelinePoint(int timestamp);
    
private:
    // 原有数据结构
    std::stack<Action> _undoStack;
    std::stack<Action> _redoStack;
    
    // 新增数据结构
    struct LevelState {
        int levelId;
        ValueMap state;
    };
    std::vector<LevelState> _levelHistory;  // 关卡历史
    
    struct Bookmark {
        string name;
        ValueMap state;
    };
    std::unordered_map<string, Bookmark> _bookmarks;  // 书签
    
    struct TimelinePoint {
        int timestamp;
        ValueMap state;
    };
    std::vector<TimelinePoint> _timeline;  // 时间线
};
```

**步骤3：扩展CommandService**

```cpp
class CommandService {
public:
    // 原有方法
    void executeReplaceTopCardCommand(int, GameModel*);
    void executeMatchTableCardCommand(int, GameModel*);
    
    // 新增方法
    void executeLevelChangeCommand(int newLevelId, GameModel*);
    void executeBookmarkSaveCommand(const string& name, GameModel*);
    void executeBookmarkRestoreCommand(const string& name, GameModel*);
    void executeTimelineJumpCommand(int timestamp, GameModel*);
};
```

**步骤4：扩展GameController**

```cpp
class GameController {
public:
    // 原有方法
    void handleUndoButtonClick();
    void handleRedoButtonClick();
    
    // 新增方法
    void handleLevelBackButton();           // 返回上一关
    void handleBookmarkSaveButton();        // 保存书签
    void handleBookmarkRestoreButton();     // 恢复书签
    void handleTimelineJumpButton();       // 跳转到时间点
    
private:
    // 新增UI回调
    std::function<void()> _levelBackCallback;
    std::function<void()> _bookmarkSaveCallback;
    std::function<void()> _bookmarkRestoreCallback;
    std::function<void(int)> _timelineJumpCallback;
};
```

**步骤5：扩展GameView**

```cpp
class GameView {
public:
    // 新增UI元素
    void setLevelBackCallback(const std::function<void()>& callback);
    void setBookmarkSaveCallback(const std::function<void()>& callback);
    void setBookmarkRestoreCallback(const std::function<void()>& callback);
    void setTimelineJumpCallback(const std::function<void(int)>& callback);
    
    // 新增UI显示方法
    void showBookmarkList(const std::vector<string>& bookmarks);
    void showTimeline(const std::vector<int>& timestamps);
};
```

#### 方案2：多级回退栈设计

**设计思路**：将回退功能分为多个独立的栈，每个栈管理一种类型的回退。

```cpp
class MultiLevelUndoModel {
public:
    enum class UndoStackType {
        OPERATION,      // 操作栈（撤销/重做）
        LEVEL,          // 关卡栈
        BOOKMARK,       // 书签栈
        TIMELINE        // 时间线栈
    };
    
    // 记录操作到指定栈
    void recordAction(UndoStackType type, const Action& action);
    
    // 从指定栈执行撤销
    void undo(UndoStackType type);
    
    // 从指定栈执行重做
    void redo(UndoStackType type);
    
    // 检查指定栈是否可以撤销
    bool canUndo(UndoStackType type) const;
    
    // 检查指定栈是否可以重做
    bool canRedo(UndoStackType type) const;
    
    // 清空指定栈
    void clear(UndoStackType type);
    
    // 清空所有栈
    void clearAll();
    
private:
    std::unordered_map<UndoStackType, std::stack<Action>> _undoStacks;
    std::unordered_map<UndoStackType, std::stack<Action>> _redoStacks;
};
```

**使用示例**：
```cpp
// 在GameController中使用多级回退
void GameController::handleMultiLevelUndo(UndoStackType type) {
    switch (type) {
        case UndoStackType::OPERATION:
            // 撤销上一步操作
            _undoModel->undo(UndoStackType::OPERATION, _gameModel);
            break;
            
        case UndoStackType::LEVEL:
            // 返回上一关
            _undoModel->undo(UndoStackType::LEVEL, _gameModel);
            break;
            
        case UndoStackType::BOOKMARK:
            // 恢复上一个书签
            _undoModel->undo(UndoStackType::BOOKMARK, _gameModel);
            break;
    }
    
    updateUI();
}
```

---

## 四、代码结构优势

### 4.1 高内聚低耦合

- **Model层**：只负责数据管理，不涉及UI逻辑
- **View层**：只负责显示和用户交互，不涉及业务逻辑
- **Controller层**：负责协调和业务逻辑，不涉及具体实现
- **Service层**：提供通用服务，可被多个模块复用

### 4.2 可维护性强

- **清晰的职责划分**：每个模块职责明确，修改影响范围小
- **统一的设计模式**：单例、命令、工厂等模式应用一致
- **完善的注释**：每个函数都有详细的功能说明和使用场景

### 4.3 可扩展性好

- **配置驱动**：通过JSON配置文件控制游戏内容，无需修改代码
- **工厂模式**：新增卡牌类型只需扩展工厂方法
- **命令模式**：新增操作类型只需扩展命令对象
- **回调机制**：新增UI元素只需设置回调函数

### 4.4 可测试性强

- **依赖注入**：Controller通过构造函数或setter注入依赖
- **接口抽象**：Service层提供接口，便于mock测试
- **单一职责**：每个函数职责单一，易于单元测试

---

## 五、未来需求承接能力评估

### 5.1 新增卡牌类型

**难度**：⭐⭐ (简单)

**评估**：
- ✅ 配置文件驱动，无需修改代码
- ✅ 工厂模式支持动态创建
- ✅ 视图自动渲染，无需额外处理
- ✅ 数据模型自动管理，无需手动维护

**工作量**：0.5人日

**承接能力**：可快速响应新增卡牌需求

### 5.2 新增回退功能

**难度**：⭐⭐⭐ (中等)

**评估**：
- ✅ 命令模式已实现，扩展容易
- ✅ UndoModel架构清晰，易于扩展
- ⚠️ 需要扩展UI界面
- ⚠️ 需要扩展数据结构

**工作量**：2人日

**承接能力**：可快速响应多类型回退需求

### 5.3 新增游戏模式

**难度**：⭐⭐⭐⭐ (中等偏难)

**评估**：
- ✅ MVC架构支持模式切换
- ✅ 配置文件可定义不同模式
- ⚠️ 需要扩展GameModel
- ⚠️ 需要扩展GameController

**工作量**：3人日

**承接能力**：可响应游戏模式扩展需求

### 5.4 新增多人联机

**难度**：⭐⭐⭐⭐⭐ (困难)

**评估**：
- ✅ MVC架构支持网络层扩展
- ✅ 命令模式支持同步
- ⚠️ 需要新增Network层
- ⚠️ 需要重构部分逻辑

**工作量**：5人日

**承接能力**：需要较大架构调整

### 5.5 新增成就系统

**难度**：⭐⭐ (简单)

**评估**：
- ✅ 配置文件可定义成就
- ✅ Service层可新增AchievementService
- ✅ GameModel可扩展成就数据
- ✅ View层可显示成就UI

**工作量**：2人日

**承接能力**：可快速响应成就系统需求

### 5.6 新增道具系统

**难度**：⭐⭐⭐ (中等)

**评估**：
- ✅ 配置文件可定义道具
- ✅ 可新增ItemModel
- ✅ GameModel可管理道具
- ✅ CardView可显示道具效果

**工作量**：3人日

**承接能力**：可响应道具系统需求

---

## 六、最佳实践建议

### 6.1 代码规范

1. **命名规范**
   - 类名：大驼峰命名（GameController）
   - 函数名：小驼峰命名（handleCardClick）
   - 变量名：小驼峰命名（_gameModel）
   - 常量名：全大写下划线分隔（MAX_HISTORY_SIZE）

2. **注释规范**
   - 每个函数都要有功能说明
   - 复杂逻辑要有行内注释
   - 使用场景要有明确说明

3. **错误处理**
   - 关键操作要有错误检查
   - 使用日志记录异常情况
   - 提供友好的错误提示

### 6.2 扩展指南

1. **新增功能时**
   - 先评估是否需要修改核心架构
   - 优先使用配置文件驱动
   - 遵循现有的设计模式
   - 保持代码风格一致

2. **修改现有功能时**
   - 确保不影响其他功能
   - 添加充分的测试
   - 更新相关文档
   - 考虑向后兼容性

3. **性能优化**
   - 避免频繁的对象创建和销毁
   - 使用对象池管理临时对象
   - 合理使用缓存机制
   - 优化渲染性能

---

## 七、附录

### 7.1 文件结构

```
card/
├── Classes/
│   ├── AppDelegate.cpp/h              # 应用程序入口
│   ├── configs/
│   │   ├── models/
│   │   │   ├── CardConfig.h      # 卡牌配置
│   │   │   └── LevelConfig.h     # 关卡配置
│   │   └── loaders/
│   │       └── LevelConfigLoader.cpp/h  # 配置加载器
│   ├── controllers/
│   │   └── GameController.cpp/h  # 游戏控制器
│   ├── models/
│   │   ├── CardModel.cpp/h       # 卡牌模型
│   │   ├── GameModel.cpp/h      # 游戏模型
│   │   └── UndoModel.cpp/h      # 撤销模型
│   ├── services/
│   │   ├── CardService.cpp/h    # 卡牌服务
│   │   └── CommandService.cpp/h # 命令服务
│   ├── views/
│   │   ├── CardView.cpp/h       # 卡牌视图
│   │   └── GameView.cpp/h      # 游戏视图
│   └── utils/
│       ├── FileUtils.cpp/h       # 文件工具
│       └── LogUtils.cpp/h        # 日志工具
├── Resources/
│   ├── level_config.json          # 关卡配置文件
│   ├── res/                     # 资源文件
│   │   ├── number/            # 数字图片
│   │   └── suits/             # 花色图片
│   └── fonts/                    # 字体文件
└── proj.win32/               # 项目配置
```

### 7.2 关键数据流

```
用户操作
    ↓
View层 (CardView/GameView)
    ↓ (回调)
Controller层 (GameController)
    ↓ (调用)
Service层 (CommandService/CardService)
    ↓ (修改)
Model层 (GameModel/CardModel)
    ↓ (通知)
View层 (更新显示)
```

### 7.3 版本历史

| 版本 | 日期 | 修改内容 | 修改人 |
|------|------|---------|--------|
| v1.0 | 2026-01-18 | 初始版本，实现核心功能 | 开发团队 |

---

## 八、总结

本程序设计文档详细说明了卡牌游戏的代码架构、模块职责、扩展性设计和未来需求承接能力。

**核心优势**：
1. **清晰的架构分层**：MVC + 命令模式 + 单例模式
2. **高内聚低耦合**：各模块职责明确，相互独立
3. **优秀的扩展性**：配置驱动、工厂模式、回调机制
4. **良好的可维护性**：统一的设计模式、完善的注释
5. **强大的需求承接能力**：可快速响应多种新增需求

**未来展望**：
基于当前架构，可以快速实现以下功能：
- 新增卡牌类型和特效
- 多类型回退功能（关卡、书签、时间线）
- 游戏模式扩展（限时、挑战等）
- 成就系统和排行榜
- 道具和技能系统
- 多人联机对战

---

**文档结束**
