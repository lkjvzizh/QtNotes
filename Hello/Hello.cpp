#include "Hello.h"
#include <QMessageBox>
#include "JellyButton.h" // <--- 引入你刚才封装的类
#include <QVBoxLayout>
#include <QSerialPort>
#include <QPainter>
#include <QRandomGenerator> // 别忘了引入这个头文件，用于生成随机数

Hello::Hello(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_assistant = new VoiceHelper(this);
    connect(m_assistant, &VoiceHelper::speechFinished, this, &Hello::onSpeechFinished);
    //测试播放
    //m_assistant->speak("Hello World, welcome to Qt.");
    m_assistant->speak("欢迎使用乐佳健康自助一体机系统");

    m_bgPixmap.load(":/images/bg.png");
    // 创建第一个按钮
    JellyButton* btn1 = new JellyButton(tr("登录"), this);

    // 创建第二个按钮 (你看，复用变得非常简单！)
    JellyButton* btn2 = new JellyButton(tr("注册"), this);

    // 放到布局里
    QWidget* centerWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centerWidget);

    layout->addWidget(btn1, 0, Qt::AlignCenter);
    layout->addSpacing(20); // 增加一点间距
    layout->addWidget(btn2, 0, Qt::AlignCenter);

    this->setCentralWidget(centerWidget);

    connect(btn1, &QPushButton::clicked, this, [=]() {

        // 这里写点击后的逻辑
        QMessageBox::information(this, tr("标题"), tr("你好！这是我的第一个 Qt 程序！"));

        });

    connect(btn2, &QPushButton::clicked, this, &Hello::changeBackgroundColor);

}

Hello::~Hello()
{}



void Hello::changeBackgroundColor()
{
    // 生成三个 0-255 的随机数
    int r = QRandomGenerator::global()->bounded(256);
    int g = QRandomGenerator::global()->bounded(256);
    int b = QRandomGenerator::global()->bounded(256);

    // 拼接样式表字符串，比如 "background-color: rgb(100, 20, 50);"
    QString style = QString("background-color: rgb(%1, %2, %3);").arg(r).arg(g).arg(b);

    // 设置给主窗口 (this)
    // 注意：因为我们之前给 centralWidget 设置过布局，背景色最好设置在 centralWidget 上
    if (centralWidget()) {
        centralWidget()->setStyleSheet(style);
    }
}

void Hello::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    // 3. 绘制时直接使用内存里的对象，速度极快
    if (!m_bgPixmap.isNull()) {
        painter.drawPixmap(this->rect(), m_bgPixmap);
    }
}


void Hello::onSpeechFinished()
{
    qDebug() << "Hello窗口收到通知：刚才那句话说完了！";

    // --- 在这里写你想做的任何事 ---

    // 例子 A：连续对话
    // m_assistant->speak("这是第二句话，紧接着上一句。");

    // 例子 B：播完自动关闭窗口 (很像游戏启动画面)
    // this->close();

    // 例子 C：弹出一个提示
    // QMessageBox::information(this, "提示", "语音播报已完成！");
}