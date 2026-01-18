// LevelConfigLoader.cpp
// 关卡配置加载器 - 单例类，负责加载和缓存关卡配置
#include "LevelConfigLoader.h"

namespace loaders {

// 单例实例指针初始化
LevelConfigLoader* LevelConfigLoader::_instance = nullptr;

LevelConfigLoader::LevelConfigLoader() {
    // 构造函数，初始化缓存
}

LevelConfigLoader::~LevelConfigLoader() {
    // 析构函数，释放所有缓存的关卡配置对象
    for (auto& pair : _levelConfigCache) {
        CC_SAFE_DELETE(pair.second);
    }
    _levelConfigCache.clear();
}

LevelConfigLoader* LevelConfigLoader::getInstance() {
    // 获取单例实例
    if (!_instance) {
        _instance = new LevelConfigLoader();
    }
    return _instance;
}

void LevelConfigLoader::destroyInstance() {
    // 销毁单例实例
    CC_SAFE_DELETE(_instance);
    _instance = nullptr;
}

configs::LevelConfig* LevelConfigLoader::loadLevelConfig(int levelId) {
    // 检查缓存中是否已存在该关卡配置
    auto it = _levelConfigCache.find(levelId);
    if (it != _levelConfigCache.end()) {
        return it->second;
    }
    
    // 生成关卡配置文件路径
    std::string configPath = generateLevelConfigPath(levelId);
    
    // 创建新的关卡配置对象
    configs::LevelConfig* levelConfig = new configs::LevelConfig();
    
    // 加载关卡配置
    bool loaded = levelConfig->loadLevelConfig(configPath);
    
    if (!loaded) {
        CCLOG("Failed to load level config for level %d", levelId);
        CC_SAFE_DELETE(levelConfig);
        return nullptr;
    }
    
    // 将加载的配置存入缓存
    _levelConfigCache[levelId] = levelConfig;
    
    return levelConfig;
}

std::string LevelConfigLoader::generateLevelConfigPath(int levelId) {
    // 生成关卡配置文件路径，目前只支持单个配置文件
    return "level_config.json";
}

} // namespace loaders
