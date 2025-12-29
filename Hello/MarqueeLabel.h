#pragma once
#include <QWidget>
#include <QTimer>
#include <QPainter>

class MarqueeLabel : public QWidget
{
    Q_OBJECT

public:
    explicit MarqueeLabel(QWidget* parent = nullptr);

    // 设置要滚动的文字
    void setText(const QString& text);
    // 设置滚动速度 (毫秒，越小越快)
    void setSpeed(int ms);

protected:
    // 🎨 核心：在这个函数里画画
    void paintEvent(QPaintEvent* event) override;

    // 窗口大小改变时，重新计算一下逻辑
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // 定时器到时间执行的函数
    void onTimeout();

private:
    QString m_text;      // 保存文字
    int m_offset;        // 当前文字显示的 X 坐标
    int m_textWidth;     // 文字的总像素宽度
    QTimer* m_timer;     // 定时器
};