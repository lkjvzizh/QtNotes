#include "JellyButton.h"
#include <QGraphicsDropShadowEffect> // 别忘了引入阴影头文件

JellyButton::JellyButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent) // 显式调用父类构造函数
{
    // 1. 基础设置
    this->setFixedSize(200, 60);
    this->setCursor(Qt::PointingHandCursor);

    // 2. 设置 QSS (完全复用之前的代码)
    QString qss = R"(
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4facfe, stop:1 #00f2fe);
            border-radius: 20px;
            color: white;
            font-family: "Microsoft YaHei";
            font-size: 18px;
            font-weight: bold;
            border: 2px solid #ffffff;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #66b6ff, stop:1 #33f6ff);
            padding-bottom: 2px;
        }
        QPushButton:pressed {
            background-color: #007bb5;
            padding-top: 4px;
            border: none;
        }
    )";
    this->setStyleSheet(qss);

    // 3. 设置阴影特效
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(3, 5);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setBlurRadius(15);
    this->setGraphicsEffect(shadow);
}

JellyButton::~JellyButton()
{
}