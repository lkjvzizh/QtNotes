#pragma once
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>

class VideoHelper : public QObject
{
    Q_OBJECT

public:
    explicit VideoHelper(QObject* parent = nullptr);

    // 基础功能
    void playUrl(const QString& path);
    QVideoWidget* getVideoWidget();

    // ✅ 新增：设置静音 (true=静音, false=有声)
    void setMuted(bool muted);

    // ✅ 新增：设置循环播放 (true=循环, false=播完即止)
    void setLoop(bool loop);

private slots:
    // ✅ 新增：内部槽函数，用来检测“是不是播完了？”
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QVideoWidget* m_videoWidget;

    // 记录循环状态
    bool m_isLoop;
};