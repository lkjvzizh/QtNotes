#pragma once
#include <QLabel>
#include <QTimer>
#include <QStringList>
#include <QDir>

class ImageCarousel : public QLabel
{
    Q_OBJECT

public:
    explicit ImageCarousel(QWidget* parent = nullptr);

    // ✅ 核心API：告诉它扫描哪个文件夹，并开始播放
    void startCarousel(const QString& folderPath);

    // 停止轮播
    void stopCarousel();

    // 设置轮播间隔(毫秒)
    void setInterval(int ms);

private slots:
    // 定时器到时间了，切到下一张
    void showNextImage();

private:
    QStringList m_imagePaths; // 存放找到的所有图片路径
    int m_currentIndex;       // 当前显示的是第几张
    QTimer* m_timer;          // 定时器
};