#include "VideoWindow.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDir>

// ✅ 构造函数：只负责搭建界面 (UI)，不处理具体视频
VideoWindow::VideoWindow(QWidget* parent)
    : QWidget(parent)
{
    // 1. 设置默认标题
    this->setWindowTitle("简易播放器 - 准备就绪");
    this->resize(800, 1200);

    // 2. 初始化助手
    m_videoHelper = new VideoHelper(this);
    m_marquee = new MarqueeLabel(this);
    m_carousel = new ImageCarousel(this);

    // 3. 跑马灯设个默认值
    m_marquee->setFixedHeight(60);
    m_marquee->setText("等待播放...");

    // 4. 布局 (和之前一样)
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(m_marquee);
    layout->addWidget(m_videoHelper->getVideoWidget(), 3);

    layout->addWidget(m_carousel, 4);

    // ⚠️ 注意：这里不再调用 playUrl 了
}

VideoWindow::~VideoWindow()
{
}

// ✅ 新增：在窗口创建后的任何时间，都可以调用这个函数来播视频
void VideoWindow::setVideoPath(const QString& path)
{
    QFileInfo fileInfo(path);

    // 1. 更新窗口标题
    this->setWindowTitle("正在播放 - " + fileInfo.fileName());

    // 2. 更新跑马灯内容
    m_marquee->setText("🔥 正在播放: " + fileInfo.fileName() + " 🔥");

    // 3. 让助手开始干活
    m_videoHelper->playUrl(path);

    // --- ✅ 启动轮播图 ---
        // 获取视频文件所在的目录路径
    QString videoFolder = fileInfo.absolutePath();

    // 让轮播图去扫描这个目录
    m_carousel->startCarousel(videoFolder);

}