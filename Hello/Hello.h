#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Hello.h"
#include <QPaintEvent>
#include "VoiceHelper.h"

class Hello : public QMainWindow
{
    Q_OBJECT

public:
    Hello(QWidget *parent = nullptr);
    ~Hello();

private:
    Ui::HelloClass ui;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    // slots 关键字告诉 Qt 这是一个槽函数
    void changeBackgroundColor();

private slots:
    // 这个函数将会在语音播报结束时被自动调用
    void onSpeechFinished();

private:
    QPixmap m_bgPixmap;
    VoiceHelper* m_assistant;
};

