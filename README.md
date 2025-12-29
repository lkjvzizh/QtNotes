# Hello Project

这是一个基于 **Qt 6 (C++)** 开发的多功能桌面应用程序，集成了串口通信、蓝牙 BLE、USB 原始通信、网络请求、Web 浏览器及多媒体播放等多种功能。

## 🚀 功能特性

项目包含以下核心模块：

### 🔌 硬件交互
- **串口通信 (Serial)**: 支持串口设备的连接、自动重连及数据收发 (`SerialWindow`).
- **蓝牙 (BLE)**: 支持低功耗蓝牙设备的扫描、连接及服务特征值的读写 (`BluetoothWindow`).
- **USB 通信**: 使用 `libusb` 实现 USB 设备的原始指令交互 (`UsbRawWindow`).

### 🌐 网络与 Web
- **Web 浏览器**: 基于 `QWebEngineView` 的内嵌浏览器，支持与 HTML 页面交互 (`WebWindow`, `BrowserWindow`).
- **WebSocket**: 包含 WebSocket 服务端与客户端的实现 (`wsServerWindow`, `wsClientWindow`).
- **HTTP 请求**: 封装了常用的 GET/POST 网络请求功能 (`NetworkHelper`).

### 🎬 多媒体
- **视频播放**: 基于 `QMediaPlayer` 的视频播放功能 (`VideoWindow`).
- **语音播报**: 集成语音合成/播报功能 (`VoiceHelper`).
- **UI 组件**: 包含图片轮播 (`ImageCarousel`) 和跑马灯标签 (`MarqueeLabel`) 等自定义控件.

### 📊 其他
- **报表打印**: 支持生成和打印报表 (`ReportWindow`).
- **国际化**: 支持多语言切换 (中文/英文).

## 🛠️ 开发环境

- **操作系统**: Windows 10/11
- **编程语言**: C++17
- **GUI 框架**: Qt 6.x (Core, Gui, Widgets, Network, SerialPort, Multimedia, WebEngine, Bluetooth, WebSockets)
- **IDE**: Visual Studio 2019/2022 或 Qt Creator
- **构建工具**: CMake 或 MSVC (qmake)
- **依赖库**: 
  - `libusb-1.0` (用于 USB 通信，库文件位于 `libusb/` 目录)

## 🏗️ 构建说明

### 使用 CMake 构建 (推荐)

1. 确保已安装 Qt 6 和 CMake。
2. 在项目根目录下打开终端：
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
3. `CMakeLists.txt` 已配置自动将 `libusb-1.0.dll` 复制到生成目录，编译完成后可直接运行。

### 使用 Visual Studio 构建

1. 打开 `Hello.vcxproj` 项目文件。
2. 确保已安装 Qt Visual Studio Tools 插件并配置好 Qt 版本。
3. 点击“生成”即可。

## 📂 目录结构

```
Hello/
├── libusb/              # libusb 依赖库
├── translations/        # 国际化翻译文件 (.ts/.qm)
├── images/              # 资源图片
├── *.h/cpp              # 源代码文件
├── *.ui                 # 界面设计文件
├── CMakeLists.txt       # CMake 构建脚本
└── README.md            # 项目说明文档
```
