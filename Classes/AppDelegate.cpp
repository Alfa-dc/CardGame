

#pragma execution_character_set("utf-8")

#include "AppDelegate.h"
#include "controllers/GameController.h"

// 音频引擎配置选项
// #define USE_AUDIO_ENGINE 1        // 使用高级音频引擎
// #define USE_SIMPLE_AUDIO_ENGINE 1 // 使用简单音频引擎

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

// Unicode文本转换宏 - 用于处理中文字符串
#define UTEXT(str) str

USING_NS_CC;

// 屏幕分辨率配置
// 设计分辨率：游戏设计的标准分辨率（1080x2080，竖屏）
static cocos2d::Size kDesignResolutionSize = cocos2d::Size(1080, 2080);
// 小分辨率配置
static cocos2d::Size kSmallResolutionSize = cocos2d::Size(1080, 2080);
// 中等分辨率配置
static cocos2d::Size kMediumResolutionSize = cocos2d::Size(1080, 2080);
// 大分辨率配置
static cocos2d::Size kLargeResolutionSize = cocos2d::Size(1080, 2080);

// AppDelegate构造函数
// 初始化应用程序代理
AppDelegate::AppDelegate()
{
}

// AppDelegate析构函数
// 清理应用程序资源
AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    // 停止高级音频引擎
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    // 停止简单音频引擎
    SimpleAudioEngine::end();
#endif
}

// 初始化OpenGL上下文属性
// 这个函数在应用程序启动时最先被调用，用于设置OpenGL渲染上下文的属性
// 属性包括：红色通道位数、绿色通道位数、蓝色通道位数、alpha通道位数、
//           深度缓冲区位数、模板缓冲区位数、多重采样数量
void AppDelegate::initGLContextAttrs()
{
    // 设置OpenGL上下文属性：red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

// 注册所有包管理器
// 这个函数用于Android平台的包管理，用于安装和管理游戏依赖的包
// 如果需要使用包管理器安装更多包，不要修改或删除这个函数
static int register_all_packages()
{
    return 0; // 返回0表示包管理器注册成功
}

// 应用程序启动完成回调函数
// 这是Cocos2d-x框架的核心入口函数，在应用程序启动完成时被调用
// 负责初始化游戏引擎、创建游戏窗口、设置游戏参数、启动游戏等
bool AppDelegate::applicationDidFinishLaunching() {
    // ========== 第一步：初始化导演 ==========
    // Director是Cocos2d-x的核心管理器，负责管理场景、调度器、动作等
    auto director = Director::getInstance();
    
    // ========== 第二步：创建OpenGL视图 ==========
    // OpenGL视图是游戏的渲染窗口，负责显示游戏画面
    auto glview = director->getOpenGLView();
    if(!glview) {
        // 如果视图不存在，则创建新的OpenGL视图
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        // 桌面平台（Windows/Mac/Linux）：创建带标题栏的窗口
        // 参数：窗口标题、窗口大小、缩放比例（0.5表示窗口大小为设计分辨率的一半）
        glview = GLViewImpl::createWithRect(UTEXT("卡牌游戏"), cocos2d::Rect(0, 0, kDesignResolutionSize.width, kDesignResolutionSize.height), 0.5);
#else
        // 移动平台：创建全屏视图
        glview = GLViewImpl::create("card");
#endif
        // 将创建的视图设置给导演
        director->setOpenGLView(glview);
    }

    // ========== 第三步：设置显示FPS ==========
    // FPS（Frames Per Second）表示每秒渲染的帧数，用于性能监控
    // 设置为false表示不显示FPS信息
    director->setDisplayStats(false);

    // ========== 第四步：设置动画帧率 ==========
    // 设置游戏的帧率为60FPS，即每秒刷新60次
    // 默认值就是1.0/60，这里显式设置以确保性能
    director->setAnimationInterval(1.0f / 144);

    // ========== 第五步：设置设计分辨率 ==========
    // 设计分辨率是游戏开发时使用的标准分辨率
    // FIXED_WIDTH策略表示固定宽度，高度会根据屏幕比例自动调整
    glview->setDesignResolutionSize(kDesignResolutionSize.width, kDesignResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);

    // ========== 第六步：注册包管理器 ==========
    // 用于Android平台的包管理，安装游戏依赖的包
    register_all_packages();

    // ========== 第七步：创建游戏控制器 ==========
    // GameController是游戏的主控制器，负责管理整个游戏的逻辑和流程
    auto gameController = new controllers::GameController();
    if (gameController && gameController->init()) {
        // 初始化成功，启动游戏
        // 参数：关卡ID，这里启动关卡1
        gameController->startGame(1);
        // 注意：gameController的生命周期由游戏视图管理，这里不需要手动释放
    } else {
        // 初始化失败，清理资源并返回错误
        CC_SAFE_DELETE(gameController);
        return false;
    }

    return true;
}

// 应用程序进入后台回调函数
// 当应用程序失去焦点时被调用，例如：
// - 接收电话
// - 按下Home键
// - 切换到其他应用
void AppDelegate::applicationDidEnterBackground() {
    // 停止动画渲染，节省CPU和电池资源
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    // 暂停所有音频播放（高级音频引擎）
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    // 暂停背景音乐和音效（简单音频引擎）
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// 应用程序进入前台回调函数
// 当应用程序重新获得焦点时被调用，例如：
// - 通话结束
// - 从后台切换回来
void AppDelegate::applicationWillEnterForeground() {
    // 恢复动画渲染
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    // 恢复所有音频播放（高级音频引擎）
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    // 恢复背景音乐和音效（简单音频引擎）
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
