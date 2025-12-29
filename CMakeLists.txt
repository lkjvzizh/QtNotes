cmake_minimum_required(VERSION 3.16)

project(Hello VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt 包
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort LinguistTools)

# 源文件
set(PROJECT_SOURCES
    Hello/main.cpp
    Hello/Hello.cpp
    Hello/Hello.h
    Hello/JellyButton.cpp
    Hello/JellyButton.h
    Hello/SerialWindow.cpp
    Hello/SerialWindow.h
    Hello/SerialWorker.cpp
    Hello/SerialWorker.h
    Hello/Hello.ui
    Hello/SerialWindow.ui
    Hello/Hello.qrc
)

# 翻译文件
set(TS_FILES
    translations/Hello_zh_CN.ts
    translations/Hello_en_US.ts
)

# 创建翻译文件
qt_add_lupdate(
    TS_FILES ${TS_FILES}
    SOURCES ${PROJECT_SOURCES}
    NO_GLOBAL_TARGET
)

qt_add_lrelease(
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

# 创建可执行文件
if(WIN32)
    add_executable(Hello WIN32 ${PROJECT_SOURCES})
else()
    add_executable(Hello ${PROJECT_SOURCES})
endif()

# 链接 Qt 库
target_link_libraries(Hello PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
)

# 设置输出目录
set_target_properties(Hello PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Windows 特定配置
if(WIN32)
    set_target_properties(Hello PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 添加 UTF-8 编码支持（MSVC）
if(MSVC)
    target_compile_options(Hello PRIVATE /utf-8)
endif()
