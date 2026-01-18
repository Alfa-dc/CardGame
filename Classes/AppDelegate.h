
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
@brief    Cocos2d-x应用程序代理类

这个类是Cocos2d-x游戏框架的入口点，负责管理应用程序的生命周期
通过私有继承cocos2d::Application，隐藏了部分Director的接口

主要职责：
1. 初始化OpenGL渲染上下文
2. 创建游戏窗口和视图
3. 管理应用程序的前台/后台切换
4. 处理音频资源的暂停和恢复
5. 启动游戏主控制器

应用程序生命周期：
- initGLContextAttrs() → applicationDidFinishLaunching() → 游戏运行
- applicationDidEnterBackground() → applicationWillEnterForeground() → 游戏恢复
*/
class  AppDelegate : private cocos2d::Application
{
public:
    // 构造函数 - 初始化应用程序代理
    AppDelegate();
    
    // 析构函数 - 清理应用程序资源
    virtual ~AppDelegate();

    // 初始化OpenGL上下文属性
    // 这个函数在应用程序启动时最先被调用，用于设置OpenGL渲染上下文的属性
    // 属性包括：颜色通道位数（RGBA）、深度缓冲区、模板缓冲区、多重采样等
    virtual void initGLContextAttrs();

    /**
    @brief    实现Director和Scene的初始化代码
    
    这是Cocos2d-x框架的核心入口函数，在应用程序启动完成时被调用
    负责初始化游戏引擎、创建游戏窗口、设置游戏参数、启动游戏等
    
    @return true    初始化成功，应用程序继续运行
    @return false   初始化失败，应用程序终止
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  当应用程序进入后台时调用
    
    当应用程序失去焦点时被调用，例如：
    - 接收电话
    - 按下Home键
    - 切换到其他应用
    
    在这个函数中应该暂停游戏逻辑、停止动画渲染、暂停音频播放等，
    以节省CPU和电池资源
    
    @param  the pointer of the application（应用程序指针，未使用）
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  当应用程序重新进入前台时调用
    
    当应用程序重新获得焦点时被调用，例如：
    - 通话结束
    - 从后台切换回来
    
    在这个函数中应该恢复游戏逻辑、恢复动画渲染、恢复音频播放等
    
    @param  the pointer of the application（应用程序指针，未使用）
    */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_

