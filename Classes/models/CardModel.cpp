#pragma execution_character_set("utf-8")
// CardModel.cpp
// 卡牌模型类实现
// 
// 这个文件实现了CardModel类的所有方法
// CardModel是游戏中最基础的数据结构，代表一张扑克牌
//
// 主要实现内容：
// 1. 构造函数和析构函数 - 创建和销毁卡牌对象
// 2. 工厂方法 - 从配置数据创建卡牌
// 3. 序列化方法 - 将卡牌状态转换为可存储格式
// 4. 反序列化方法 - 从存储格式恢复卡牌状态
//
// 序列化/反序列化是撤销/重做功能的核心
// 它们允许我们保存和恢复卡牌的完整状态
#include "CardModel.h"

namespace models {

// ========== 构造函数和析构函数 ==========

// 默认构造函数
// 创建一个默认的卡牌对象，所有属性都设置为默认值
// 
// 默认值说明：
// - id: 0 (无效ID，实际使用时会被替换)
// - suit: HEART (红心，默认花色)
// - rank:1 (A，默认点数)
// - type: HAND_CARD (手牌，默认类型)
// - position: ZERO (原点，默认位置)
// - isTopCard: false (不是顶手牌)
// - isVisible: true (可见)
// - zOrder: 0 (默认层级)
CardModel::CardModel() :
    _id(0),
    _suit(configs::CardConfig::Suit::HEART),
    _rank(1),
    _type(configs::CardConfig::CardType::HAND_CARD),
    _position(cocos2d::Vec2::ZERO),
    _originalPosition(cocos2d::Vec2::ZERO),
    _isTopCard(false),
    _isVisible(true),
    _zOrder(0) {
}

// 完整构造函数
// 根据指定的参数创建一个卡牌对象
// 
// 参数说明：
// id: 卡牌的唯一标识符
// suit: 卡牌花色（红心、方块、梅花、黑桃）
// rank: 卡牌点数（1-13）
// type: 卡牌类型（手牌或桌面牌）
// position: 卡牌在屏幕上的初始位置
//
// 注意：纹理名称会根据花色和点数自动生成
CardModel::CardModel(int id, const configs::CardConfig::Suit& suit, int rank,
                   const configs::CardConfig::CardType& type, const cocos2d::Vec2& position) :
    _id(id),
    _suit(suit),
    _rank(rank),
    _type(type),
    _position(position),
    _originalPosition(position),
    _textureName(configs::CardConfig::getTextureName(suit, rank)),
    _isTopCard(false),
    _isVisible(true) {
}

// 析构函数
// 清理卡牌对象的资源
// 
// 注意：这个类不管理动态分配的资源，所以析构函数为空
CardModel::~CardModel() {
}

// ========== 工厂方法 ==========

// 从配置创建卡牌模型
// 这是一个静态工厂方法，用于从配置数据创建卡牌对象
//
// 参数：configData - 包含卡牌配置数据的结构体
// 返回：创建的CardModel指针，失败返回nullptr
//
// 工作流程：
// 1. 创建一个新的CardModel对象
// 2. 从配置数据中复制所有属性
// 3. 设置默认状态（不是顶手牌，可见）
// 4. 返回创建的对象
//
// 使用场景：
// - 从JSON配置文件加载关卡时创建卡牌
// - 从序列化数据恢复卡牌时使用
CardModel* CardModel::createFromConfig(const configs::CardConfig::CardData& configData) {
    CardModel* model = new CardModel();
    if (model) {
        model->_id = configData.id;
        model->_suit = configData.suit;
        model->_rank = configData.rank;
        model->_type = configData.type;
        model->_position = configData.position;
        model->_originalPosition = configData.originalPos;
        model->_textureName = configData.textureName;
        model->_isTopCard = false;
        model->_isVisible = true; // 默认可见
        return model;
    }
    delete model;
    return nullptr;
}

// ========== 序列化方法 ==========

// 序列化卡牌数据
// 将卡牌的当前状态转换为可存储的ValueMap格式
//
// 返回：包含卡牌所有属性的ValueMap对象
//
// 序列化的数据包括：
// - id: 卡牌ID
// - suit: 卡牌花色（转换为整数）
// - rank: 卡牌点数
// - type: 卡牌类型（转换为整数）
// - positionX/Y: 卡牌当前位置的X和Y坐标
// - originalX/Y: 卡牌原始位置的X和Y坐标
// - textureName: 卡牌纹理名称
// - isTopCard: 是否为顶手牌
// - isVisible: 是否可见
// - zOrder: 卡牌Z轴值（显示层级）
//
// 使用场景：
// - 保存游戏状态时
// - 执行操作前保存当前状态（用于撤销）
cocos2d::ValueMap CardModel::serialize() const {
    cocos2d::ValueMap data;
    data["id"] = cocos2d::Value(_id);
    data["suit"] = cocos2d::Value(static_cast<int>(_suit));
    data["rank"] = cocos2d::Value(_rank);
    data["type"] = cocos2d::Value(static_cast<int>(_type));
    data["positionX"] = cocos2d::Value(_position.x);
    data["positionY"] = cocos2d::Value(_position.y);
    data["originalX"] = cocos2d::Value(_originalPosition.x);
    data["originalY"] = cocos2d::Value(_originalPosition.y);
    data["textureName"] = cocos2d::Value(_textureName);
    data["isTopCard"] = cocos2d::Value(_isTopCard);
    data["isVisible"] = cocos2d::Value(_isVisible);
    data["zOrder"] = cocos2d::Value(_zOrder);  // 保存Z轴值
    return data;
}

// 反序列化卡牌数据
// 从存储的ValueMap格式恢复卡牌的状态
//
// 参数：data - 包含卡牌数据的ValueMap对象
// 返回：成功返回true，失败返回false
//
// 工作流程：
// 1. 检查必要的数据字段是否存在
// 2. 从ValueMap中读取并设置基本属性
// 3. 处理可选字段（原始位置、纹理名称、状态标志）
// 4. 如果缺少可选字段，使用合理的默认值
//
// 错误处理：
// - 如果缺少必要字段，返回false
// - 如果数据格式不正确，可能导致运行时错误
//
// 使用场景：
// - 加载保存的游戏状态时
// - 撤销操作时恢复之前的状态
bool CardModel::deserialize(const cocos2d::ValueMap& data) {
    // 检查必要字段是否存在
    if (!data.count("id") || !data.count("suit") || !data.count("rank") ||
        !data.count("type") || !data.count("positionX") || !data.count("positionY")) {
        return false;
    }
    
    // 读取并设置基本属性
    _id = data.at("id").asInt();
    _suit = static_cast<configs::CardConfig::Suit>(data.at("suit").asInt());
    _rank = data.at("rank").asInt();
    _type = static_cast<configs::CardConfig::CardType>(data.at("type").asInt());
    _position.x = data.at("positionX").asFloat();
    _position.y = data.at("positionY").asFloat();
    
    // 处理原始位置（可选字段）
    if (data.count("originalX") && data.count("originalY")) {
        _originalPosition.x = data.at("originalX").asFloat();
        _originalPosition.y = data.at("originalY").asFloat();
    } else {
        // 如果没有原始位置，使用当前位置作为原始位置
        _originalPosition = _position;
    }
    
    // 处理纹理名称（可选字段）
    if (data.count("textureName")) {
        _textureName = data.at("textureName").asString();
    } else {
        // 如果没有纹理名称，根据花色和点数生成默认纹理名称
        _textureName = configs::CardConfig::getTextureName(_suit, _rank);
    }
    
    // 处理顶手牌状态（可选字段）
    if (data.count("isTopCard")) {
        _isTopCard = data.at("isTopCard").asBool();
    } else {
        _isTopCard = false;
    }
    
    // 处理可见性（可选字段）
    if (data.count("isVisible")) {
        _isVisible = data.at("isVisible").asBool();
    } else {
        _isVisible = true;
    }
    
    // 处理Z轴值（可选字段）
    if (data.count("zOrder")) {
        _zOrder = data.at("zOrder").asInt();
    } else {
        _zOrder = 0;  // 默认层级为0
    }
    
    return true;
}

} // namespace models
