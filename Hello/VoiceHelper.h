#pragma once
#include <QObject>
#include <QTextToSpeech>

class VoiceHelper : public QObject
{
    Q_OBJECT // 这一行至关重要，必须有！
public:
    explicit VoiceHelper(QObject* parent = nullptr);
    void speak(const QString& text);

    // 1. 添加一个信号，告诉外界播放结束了
signals:
    void speechFinished();

    // 2. 添加一个私有槽函数，用来内部监听底层状态
private slots:
    void onStateChanged(QTextToSpeech::State state);

private:
    QTextToSpeech* m_tts;
};