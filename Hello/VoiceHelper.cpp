#include "VoiceHelper.h"
#include <QDebug>

VoiceHelper::VoiceHelper(QObject* parent) : QObject(parent)
{
    m_tts = new QTextToSpeech(this);
    //m_tts = new QTextToSpeech("sapi", this);
    // 1. 获取系统里所有的声音列表
    const QList<QVoice> voices = m_tts->availableVoices();

    // 2. 遍历打印出来看看（调试用）
    for (const QVoice& voice : voices) {
        qDebug() << "发现语音包:" << voice.name();

     qDebug() << "当前使用的语音引擎:" << m_tts->engine();
        // 3. 如果名字里包含 "Huihui" (中文常用) 或者 "Xiaoxiao" (更自然的)，就选中它
        // 注意：Windows 里的名字通常叫 "Microsoft Huihui Desktop"

     if (voice.name().contains("Yaoyao")) {
         m_tts->setVoice(voice);
         qDebug() << "--> 已切换到:" << voice.name();
     }

     /*
        if (voice.name().contains("Alan") || voice.name().contains("BDL")) {
            m_tts->setVoice(voice);
            qDebug() << "--> 已切换到:" << voice.name();
        }
        */
        
    }
    /*发现语音包: "Microsoft Huihui" ，"Microsoft Yaoyao" ，"Microsoft Kangkang"*/

    // 初始化设置
    // m_tts->setVolume(1.0);

    // 1. 核心连接：连接底层的 stateChanged 信号到我们的私有槽函数
    connect(m_tts, &QTextToSpeech::stateChanged,
        this, &VoiceHelper::onStateChanged);
}

void VoiceHelper::speak(const QString& text)
{
    if (m_tts->state() == QTextToSpeech::Ready) {
        m_tts->say(text);
        qDebug() << "开始播报:" << text;
    }
}

// 2. 实现状态处理
void VoiceHelper::onStateChanged(QTextToSpeech::State state)
{
    // 当状态变为 "Ready" 时，说明上一句话说完了
    if (state == QTextToSpeech::Ready) {
        qDebug() << "播报结束 (Internal)";

        // 发送我们自定义的信号通知主窗口
        emit speechFinished();
    }
}