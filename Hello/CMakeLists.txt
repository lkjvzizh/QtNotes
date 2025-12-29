cmake_minimum_required(VERSION 3.16)

# 项目名称和版本
project(Hello VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 开启 Qt 自动工具 (MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 组件 (如果构建环境是 Qt5，请修改为 Qt5)
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    SerialPort
    Network
    Multimedia
    MultimediaWidgets
    WebEngineWidgets
    Bluetooth
    PrintSupport
    WebSockets
    REQUIRED
)

# --- 第三方库配置: LibUSB ---
set(LIBUSB_DIR "${CMAKE_SOURCE_DIR}/libusb")
set(LIBUSB_INCLUDE_DIR "${LIBUSB_DIR}")
# 根据文件名直接指定库文件
set(LIBUSB_LIBRARY "${LIBUSB_DIR}/libusb-1.0.lib")
set(LIBUSB_DLL "${LIBUSB_DIR}/libusb-1.0.dll")

# 添加头文件路径
include_directories(${LIBUSB_INCLUDE_DIR})

# --- 源文件管理 ---
# 包含当前目录下的所有源文件
file(GLOB PROJECT_SOURCES
    "*.cpp"
    "*.h"
    "*.ui"
    "*.qrc"
)

# 生成可执行文件
# qt_add_executable 是 Qt6 的新命令，兼容性更好
if(COMMAND qt_add_executable)
    qt_add_executable(Hello MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(Hello ${PROJECT_SOURCES})
endif()

# --- 链接库 ---
target_link_libraries(Hello PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::WebEngineWidgets
    Qt6::Bluetooth
    Qt6::PrintSupport
    Qt6::WebSockets
    ${LIBUSB_LIBRARY}
)

# --- 构建后操作: 复制 DLL ---
if(WIN32)
    # 将 libusb dll 复制到构建目录，确保程序能运行
    add_custom_command(TARGET Hello POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBUSB_DLL}"
        "$<TARGET_FILE_DIR:Hello>"
        COMMENT "Copying libusb-1.0.dll to output directory..."
    )
endif()

# --- 安装/部署配置 (可选) ---
install(TARGETS Hello
    BUNDLE DESTINATION .
    LIBRARY DESTINATION .
    RUNTIME DESTINATION .
)
