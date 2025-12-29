#include "VideoHelper.h"
#include <QUrl>

VideoHelper::VideoHelper(QObject* parent)
    : QObject(parent), m_isLoop(false) // 默认不循环
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_videoWidget = new QVideoWidget;

    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoWidget);
    m_audioOutput->setVolume(0.5);

    // ✅ 关键连接：当媒体状态改变时（比如播放结束），通知我们
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
        this, &VideoHelper::onMediaStatusChanged);
}

void VideoHelper::playUrl(const QString& path)
{
    m_player->setSource(QUrl::fromLocalFile(path));
    m_player->play();
}

QVideoWidget* VideoHelper::getVideoWidget()
{
    return m_videoWidget;
}

// ✅ 实现静音控制
void VideoHelper::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

// ✅ 实现循环开关
void VideoHelper::setLoop(bool loop)
{
    m_isLoop = loop;
}

// ✅ 核心逻辑：自动重播
void VideoHelper::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    // 如果状态变成了 "EndOfMedia" (播完了) 且 用户开启了循环
    if (status == QMediaPlayer::EndOfMedia && m_isLoop) {
        m_player->play(); // 马上重新开始播放
    }
}