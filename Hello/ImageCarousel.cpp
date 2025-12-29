#include "ImageCarousel.h"
#include <QPixmap>
#include <QDebug>

ImageCarousel::ImageCarousel(QWidget* parent)
    : QLabel(parent), m_currentIndex(-1)
{
    // 1. 初始化定时器
    m_timer = new QTimer(this);
    // 默认 2 秒切一张
    m_timer->setInterval(2000);
    connect(m_timer, &QTimer::timeout, this, &ImageCarousel::showNextImage);

    // 2. 设置 Label 属性
    this->setAlignment(Qt::AlignCenter); // 图片居中
    this->setScaledContents(true);       // ✅ 关键：让图片自动缩放以填满 Label 区域
    this->setStyleSheet("background-color: #333; border: 1px solid #555;"); // 加个深色背景和边框好看点
    // 设置一个最小高度，防止没图的时候缩没了
    this->setMinimumHeight(150);
    this->setText("等待加载图片...");
}

void ImageCarousel::startCarousel(const QString& folderPath)
{
    // 先停掉之前的
    stopCarousel();
    m_imagePaths.clear();

    QDir dir(folderPath);
    if (!dir.exists()) {
        this->setText("文件夹不存在: " + folderPath);
        return;
    }

    // ✅ 设置过滤器：只找图片文件
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
    dir.setNameFilters(filters);
    // 获取文件列表 (只要文件，不要隐藏的)
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    // 把绝对路径存起来
    for (const QFileInfo& fileInfo : fileList) {
        m_imagePaths.append(fileInfo.absoluteFilePath());
    }

    if (m_imagePaths.isEmpty()) {
        this->setText("该文件夹下没有找到图片");
        qDebug() << "目录中无图片:" << folderPath;
    }
    else {
        qDebug() << "找到" << m_imagePaths.size() << "张图片，开始轮播";
        m_currentIndex = 0;
        // 马上显示第一张
        showNextImage();
        // 启动定时器
        m_timer->start();
    }
}

void ImageCarousel::stopCarousel()
{
    m_timer->stop();
    this->clear(); // 清除显示的图片
    this->setText("轮播停止");
}

void ImageCarousel::setInterval(int ms)
{
    m_timer->setInterval(ms);
}

void ImageCarousel::showNextImage()
{
    if (m_imagePaths.isEmpty()) return;

    // 1. 加载当前索引的图片
    QPixmap pixmap(m_imagePaths[m_currentIndex]);

    // 2. 显示到 Label 上
    if (!pixmap.isNull()) {
        this->setPixmap(pixmap);
    }
    else {
        this->setText("图片加载失败");
    }

    // 3. 计算下一个索引 (核心算法：取余数实现循环)
    // 比如共5张图，当前是4，(4+1)%5 = 0，回到开头
    m_currentIndex = (m_currentIndex + 1) % m_imagePaths.size();
}