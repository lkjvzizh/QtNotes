#include "MarqueeLabel.h"
#include <QFontMetrics> // 用来计算文字宽度的工具

MarqueeLabel::MarqueeLabel(QWidget* parent)
    : QWidget(parent), m_offset(0), m_textWidth(0)
{
    // 1. 初始化定时器
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MarqueeLabel::onTimeout);

    // 2. 设置默认速度 (30ms 刷新一次，大约 30fps)
    m_timer->start(30);

    // 3. 设置默认背景色 (黑色) 和字体 (绿色 LED 风格)
    // 这一步设置会让它看起来像个黑色背景的控件
    this->setStyleSheet("background-color: black;");

    // 设置个大点的字体
    QFont font = this->font();
    font.setPixelSize(40); // 40像素高
    font.setBold(true);
    this->setFont(font);
}

void MarqueeLabel::setText(const QString& text)
{
    m_text = text;

    // 计算这段文字在当前字体下，到底占多少像素宽
    QFontMetrics metrics(this->font());
    m_textWidth = metrics.horizontalAdvance(m_text);

    // 每次设文字，从最右边开始滚
    m_offset = this->width();

    // 触发重绘
    update();
}

void MarqueeLabel::setSpeed(int ms)
{
    m_timer->setInterval(ms);
}

// 🎨 每一帧都会自动调用这个函数
void MarqueeLabel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // 1. 抗锯齿，让文字边缘平滑
    painter.setRenderHint(QPainter::Antialiasing);

    // 2. 设置画笔颜色 (亮绿色)
    painter.setPen(QColor(0, 255, 0));

    // 3. 在指定位置画文字
    // (x, y, width, height, 对齐方式, 文字内容)
    // 注意：m_offset 是动态变化的，所以文字会动
    int y = (this->height() - painter.fontMetrics().height()) / 2; // 垂直居中计算

    // 修正：drawText 的 y 是基线位置，或者用 rect 画更简单
    QRect rect(m_offset, 0, m_textWidth, this->height());
    painter.drawText(rect, Qt::AlignVCenter, m_text);
}

// ⏰ 定时器逻辑
void MarqueeLabel::onTimeout()
{
    // 每次向左移动 2 像素
    m_offset -= 2;

    // 边界判断：如果文字已经完全跑出了左边屏幕
    // ( offset + textWidth < 0 说明尾巴都看不见了)
    if (m_offset + m_textWidth < 0) {
        // 重置到最右边
        m_offset = this->width();
    }

    // 呼叫 paintEvent 重绘
    update();
}

void MarqueeLabel::resizeEvent(QResizeEvent* event)
{
    // 窗口大小变了，简单处理：不做特殊操作，继续滚
    QWidget::resizeEvent(event);
}