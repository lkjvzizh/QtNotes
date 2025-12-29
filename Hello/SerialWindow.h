#pragma once
#include <QWidget>
#include <QCheckBox>
#include <QTextEdit>
#include "SerialWorker.h" // 引入你的串口工作类


class SerialWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWindow(QWidget* parent = nullptr);
    ~SerialWindow();

private:
    SerialWorker* m_worker; // 核心串口对象
    QCheckBox* m_chkHexDisplay; // <--- 新增一个成员变量
    QCheckBox* m_chkPacketMode; // 新增一个复选框指针
    QCheckBox* m_chkGBK;
    // 【新增】发送区控件
    QTextEdit* m_inputEdit;   // 输入框
    QCheckBox* m_chkHexSend;  // Hex发送开关
};