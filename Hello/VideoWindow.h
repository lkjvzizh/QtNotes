#pragma once
#include <QWidget>
#include "MarqueeLabel.h"
#include "VideoHelper.h"
#include "ImageCarousel.h"

class VideoWindow : public QWidget
{
    Q_OBJECT

public:
    // 1. ✅ 构造函数变干净了，只负责初始化界面
    explicit VideoWindow(QWidget* parent = nullptr);
    ~VideoWindow();

    // 2. ✅ 新增函数：专门用来设置和播放视频
    void setVideoPath(const QString& path);

private:
    MarqueeLabel* m_marquee;
    VideoHelper* m_videoHelper;
    ImageCarousel* m_carousel;
};