#pragma once

#include <QPushButton> // 必须引入基类头文件

class JellyButton : public QPushButton
{
    Q_OBJECT // Qt 的宏，建议加上，为了支持信号槽等高级特性

public:
    // 构造函数：允许传入按钮文字，和父窗口
    JellyButton(const QString& text, QWidget* parent = nullptr);

    // 你可以在这里声明更多函数，比如 setColor()...
    ~JellyButton();
};