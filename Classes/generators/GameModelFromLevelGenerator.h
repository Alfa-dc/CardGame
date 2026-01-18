// GameModelFromLevelGenerator.h
// 游戏模型生成器 - 根据关卡配置生成游戏模型
//
// 这个类是游戏模型的生成器，负责：
// 1. 根据关卡配置创建游戏模型
// 2. 将配置中的卡牌数据转换为卡牌模型
// 3. 设置初始顶手牌
// 4. 管理生成器的生命周期
//
// 核心概念：
// - 单例模式：确保全局只有一个生成器实例
// - 工厂模式：根据配置数据创建游戏模型
// - 配置驱动：通过关卡配置生成游戏数据
//
// 使用示例：
// GameModelFromLevelGenerator* generator = GameModelFromLevelGenerator::getInstance();
// GameModel* model = generator->generateGameModel(levelConfig);
#ifndef __GAME_MODEL_FROM_LEVEL_GENERATOR_H__
#define __GAME_MODEL_FROM_LEVEL_GENERATOR_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "configs/models/LevelConfig.h"

namespace generators {

// 游戏模型生成器 - 根据关卡配置生成游戏模型
class GameModelFromLevelGenerator {
private:
    // 构造函数 - 私有构造函数，确保单例模式
    GameModelFromLevelGenerator();
    
    // 析构函数 - 清理资源
    ~GameModelFromLevelGenerator();
    
    // 禁止拷贝构造和赋值
    GameModelFromLevelGenerator(const GameModelFromLevelGenerator&) = delete;
    GameModelFromLevelGenerator& operator=(const GameModelFromLevelGenerator&) = delete;
    
public:
    // 获取单例实例
    // 返回：生成器的单例实例
    static GameModelFromLevelGenerator* getInstance();
    
    // 销毁单例实例
    static void destroyInstance();
    
    // 根据关卡配置生成游戏模型
    // 参数：levelConfig - 关卡配置对象
    // 返回：生成的游戏模型，失败返回nullptr
    //
    // 工作流程：
    // 1. 创建新的游戏模型
    // 2. 从关卡配置中获取桌面卡牌和手牌数据
    // 3. 为每张卡牌创建卡牌模型并添加到游戏模型中
    // 4. 设置初始顶手牌
    // 5. 返回生成的游戏模型
    models::GameModel* generateGameModel(const configs::LevelConfig& levelConfig);
    
private:
    // 根据卡牌配置创建卡牌模型
    // 参数：cardData - 卡牌配置数据
    // 返回：创建的卡牌模型，失败返回nullptr
    models::CardModel* createCardModelFromConfig(const configs::CardConfig::CardData& cardData);
    
    // 单例实例
    static GameModelFromLevelGenerator* _instance;
};

} // namespace generators

#endif // __GAME_MODEL_FROM_LEVEL_GENERATOR_H__
