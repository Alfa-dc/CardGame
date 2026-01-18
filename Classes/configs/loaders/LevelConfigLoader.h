// LevelConfigLoader.h
// 关卡配置加载器 - 负责根据关卡ID加载关卡配置
//
// 这个类是关卡配置的加载器，负责：
// 1. 根据关卡ID加载对应的配置文件
// 2. 解析配置文件内容，生成LevelConfig对象
// 3. 缓存已加载的配置，提高性能
// 4. 管理配置加载器的生命周期
//
// 核心概念：
// - 单例模式：LevelConfigLoader是单例，全局唯一
// - 配置缓存：缓存已加载的配置，避免重复加载
// - 文件路径生成：根据关卡ID生成配置文件路径
// - 配置解析：将JSON配置转换为LevelConfig对象
//
// 使用示例：
// LevelConfigLoader* loader = LevelConfigLoader::getInstance();
// LevelConfig* config = loader->loadLevelConfig(1);
// if (config) {
//     // 使用配置生成游戏模型
// }
#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include "cocos2d.h"
#include "configs/models/LevelConfig.h"

namespace loaders {

// 关卡配置加载器 - 负责根据关卡ID加载关卡配置
class LevelConfigLoader {
private:
    // 构造函数 - 私有构造函数，实现单例模式
    LevelConfigLoader();
    
    // 析构函数 - 清理资源，释放缓存的配置
    ~LevelConfigLoader();
    
    // 禁止拷贝构造和赋值操作，确保单例模式
    LevelConfigLoader(const LevelConfigLoader&) = delete;
    LevelConfigLoader& operator=(const LevelConfigLoader&) = delete;
    
public:
    // 获取单例实例
    // 返回：LevelConfigLoader的单例对象
    static LevelConfigLoader* getInstance();
    
    // 销毁单例实例
    // 清理所有资源，释放缓存的配置
    static void destroyInstance();
    
    // 根据关卡ID加载关卡配置
    // 参数：levelId - 关卡ID
    // 返回：生成的LevelConfig对象，失败返回nullptr
    configs::LevelConfig* loadLevelConfig(int levelId);
    
private:
    // 根据关卡ID生成配置文件路径
    // 参数：levelId - 关卡ID
    // 返回：配置文件的完整路径
    std::string generateLevelConfigPath(int levelId);
    
    // 关卡配置缓存 - 键为关卡ID，值为对应的配置对象
    std::map<int, configs::LevelConfig*> _levelConfigCache;
    
    // 单例实例
    static LevelConfigLoader* _instance;
};

} // namespace loaders

#endif // __LEVEL_CONFIG_LOADER_H__
