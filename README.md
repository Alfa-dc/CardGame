# 卡牌游戏程序设计文档

## 文档信息
- **项目名称**: 卡牌游戏
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

### 3. 如何新增一个类型的回退功能

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



### 4. 文件结构

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

### 5. 关键数据流

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

