#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class NetworkHelperWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkHelperWindow(QWidget* parent = nullptr);

private:
    // UI 控件成员变量
    QLineEdit* m_urlEdit;       // 输入网址的地方
    QTextEdit* m_logArea;       // 显示结果的地方
    QPushButton* m_btnGet;      // GET 按钮
    QPushButton* m_btnPost;     // POST 按钮

    // 辅助函数：向日志区追加文本
    void log(const QString& text);
};