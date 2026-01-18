# Cocos2d-x项目安卓移植指南

本指南将详细介绍如何将您的卡牌游戏项目从Windows移植到Android平台。

## 1. 环境配置

### 1.1 安装必要工具

1. **Java JDK**：下载并安装JDK 1.8或以上版本
   - 下载地址：[Oracle JDK](https://www.oracle.com/java/technologies/javase-jdk8-downloads.html)

2. **Android Studio**：下载并安装最新版本
   - 下载地址：[Android Studio](https://developer.android.com/studio)

3. **NDK**：通过Android Studio安装NDK
   - 打开Android Studio → Settings → Appearance & Behavior → System Settings → Android SDK → SDK Tools
   - 勾选NDK (Side by side) 和 CMake，点击Apply安装

4. **SDK**：确保安装了Android SDK Platform和Android SDK Build-Tools
   - 打开Android Studio → Settings → Appearance & Behavior → System Settings → Android SDK
   - 选择合适的SDK版本（建议Android 10.0或以上）
   - 勾选相应的SDK Platform和Build-Tools

### 1.2 配置环境变量

1. **JAVA_HOME**：指向JDK安装目录
2. **ANDROID_HOME**：指向Android SDK安装目录
3. **NDK_ROOT**：指向NDK安装目录
4. 将JDK的bin目录、Android SDK的tools和platform-tools目录添加到PATH环境变量

## 2. 项目结构分析

您的项目中已经包含了`proj.android`目录，这是Cocos2d-x为Android平台生成的项目目录。其结构如下：

```
proj.android/
├── app/
│   ├── jni/              # C++代码编译配置
│   │   ├── Android.mk     # NDK构建脚本
│   │   └── Application.mk # 应用配置
│   ├── src/              # Java代码
│   ├── AndroidManifest.xml # Android清单文件
│   ├── build.gradle      # 项目构建配置
│   └── proguard-rules.pro # ProGuard配置
├── build.gradle          # 顶级构建配置
├── gradle.properties     # Gradle属性
├── gradlew               # Gradle wrapper脚本（Linux/Mac）
└── gradlew.bat           # Gradle wrapper脚本（Windows）
```

## 3. 代码适配

### 3.1 平台特定代码处理

在Cocos2d-x中，可以使用宏来区分不同平台：

```cpp
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Android特定代码
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // Windows特定代码
#endif
```

### 3.2 输入处理

Android平台的输入处理与Windows有所不同，主要使用触摸事件而非鼠标事件。确保您的代码正确处理触摸事件：

```cpp
// 在CardView.cpp中，触摸事件处理已经实现
// 如果您的代码中使用了鼠标事件，需要修改为触摸事件
```

### 3.3 文件路径处理

Android平台的文件系统结构与Windows不同，确保使用Cocos2d-x提供的文件工具类来加载资源：

```cpp
// 正确：使用FileUtils获取文件路径
std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename("res/card_general.png");

// 错误：使用硬编码路径
// std::string filePath = "Resources/res/card_general.png";
```

## 4. 资源处理

### 4.1 资源目录结构

确保所有资源文件都放在`Resources`目录下，Cocos2d-x会自动将其打包到APK中。

### 4.2 屏幕适配

Android设备有多种分辨率，建议使用Cocos2d-x的多分辨率适配机制：

```cpp
// 在AppDelegate.cpp中，已经设置了设计分辨率
// 确保使用正确的ResolutionPolicy
GLView* glview = director->getOpenGLView();
glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
```

## 5. 构建项目

### 5.1 使用Android Studio构建

1. **打开项目**：
   - 启动Android Studio
   - 选择"Open an existing project"
   - 选择`proj.android`目录

2. **配置NDK路径**：
   - 打开File → Project Structure
   - 在SDK Location标签页中，设置NDK路径

3. **同步Gradle**：
   - 点击Sync Now按钮，同步Gradle配置

4. **构建APK**：
   - 点击Build → Build Bundle(s) / APK(s) → Build APK(s)
   - 构建完成后，APK文件会生成在`proj.android/app/build/outputs/apk/debug/`目录下

### 5.2 使用命令行构建

1. **进入项目目录**：
   ```bash
   cd d:\Code\cocos_work\mycard\proj.android
   ```

2. **清理项目**：
   ```bash
   gradlew clean
   ```

3. **构建APK**：
   ```bash
   gradlew assembleDebug
   ```

4. **构建发布版本**：
   ```bash
   gradlew assembleRelease
   ```

## 6. 测试项目

### 6.1 使用模拟器

1. **创建模拟器**：
   - 打开Android Studio → AVD Manager
   - 点击Create Virtual Device
   - 选择设备类型，点击Next
   - 选择系统镜像，点击Next
   - 配置模拟器属性，点击Finish

2. **运行APK**：
   - 点击Run → Run 'app'
   - 选择创建的模拟器，点击OK

### 6.2 使用真机测试

1. **启用开发者选项**：
   - 在Android设备上，进入设置 → 关于手机
   - 连续点击版本号7次，启用开发者选项
   - 进入设置 → 开发者选项，启用USB调试

2. **连接设备**：
   - 使用USB数据线连接设备到电脑
   - 在Android Studio中，点击Run → Run 'app'
   - 选择连接的设备，点击OK

## 7. 常见问题及解决方案

### 7.1 编译错误

**问题**：找不到头文件或库文件
**解决方案**：
- 检查`Android.mk`文件中的`LOCAL_C_INCLUDES`是否包含所有必要的头文件目录
- 确保`LOCAL_LDLIBS`中包含所有必要的库

**问题**：undefined reference to...
**解决方案**：
- 检查是否缺少库文件
- 确保所有源文件都已添加到`LOCAL_SRC_FILES`中

### 7.2 运行时崩溃

**问题**：资源文件加载失败
**解决方案**：
- 确保资源文件放在`Resources`目录下
- 检查资源文件名的大小写（Android区分大小写）
- 使用`FileUtils::getInstance()->fullPathForFilename()`获取正确路径

**问题**：屏幕显示异常
**解决方案**：
- 检查设计分辨率和实际分辨率的适配
- 确保使用了正确的`ResolutionPolicy`

## 8. 优化建议

1. **使用NDK Release构建**：
   - 发布版本使用Release构建，可以提高性能
   - 在`Application.mk`中设置`APP_OPTIM := release`

2. **启用ProGuard混淆**：
   - 在`build.gradle`中启用ProGuard，可以减小APK体积
   - 注意：需要配置ProGuard规则，避免混淆Cocos2d-x的关键类

3. **优化资源**：
   - 压缩图片资源，使用WebP格式
   - 移除未使用的资源
   - 为不同分辨率提供不同版本的资源

## 9. 发布到Google Play

1. **生成签名APK**：
   - 点击Build → Generate Signed Bundle / APK
   - 选择APK，点击Next
   - 选择或创建密钥库，填写密钥信息
   - 选择Build Type（Release），点击Finish

2. **上传到Google Play**：
   - 登录Google Play Console
   - 创建应用或选择现有应用
   - 上传生成的签名APK
   - 填写应用信息，提交审核

## 10. 参考资源

- [Cocos2d-x官方文档](https://docs.cocos.com/cocos2d-x/manual/zh/)
- [Android开发者文档](https://developer.android.com/docs)
- [NDK开发指南](https://developer.android.com/ndk/guides)

## 11. 注意事项

1. 确保您使用的Cocos2d-x版本支持Android平台
2. 定期更新NDK和SDK到最新版本
3. 测试不同Android版本和设备，确保兼容性
4. 关注Cocos2d-x社区的最新动态和最佳实践

按照以上步骤，您应该能够成功将项目移植到Android平台。如果遇到问题，可以查看Cocos2d-x官方文档或在社区寻求帮助。