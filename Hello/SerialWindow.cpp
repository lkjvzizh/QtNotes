#include "SerialWindow.h"
#include "JellyButton.h"     // 复用你的漂亮按钮
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QDateTime>


SerialWindow::SerialWindow(QWidget* parent)
    : QWidget(parent)
{

    // ==========================================
    // 纯代码布局开始 (完全复用之前的逻辑)
    // ==========================================

    // 1. 初始化核心
    m_worker = new SerialWorker(this);

    // 2. 布局设置
    QVBoxLayout* mainLayout = new QVBoxLayout(this); // 直接给当前窗口设置布局

    // [顶部] 下拉框 + 按钮
    QHBoxLayout* topLayout = new QHBoxLayout();
    QComboBox* comboPorts = new QComboBox(this);
    comboPorts->addItems(SerialWorker::getAvailablePorts());

    JellyButton* btnOpen = new JellyButton(tr("打开串口"), this);
    btnOpen->setFixedSize(120, 40);

    m_chkHexDisplay = new QCheckBox(tr("HEX显示"), this);
    m_chkPacketMode = new QCheckBox(tr("开启AA55协议"), this);
    m_chkGBK = new QCheckBox(tr("GBK模式"), this);

    topLayout->addWidget(comboPorts);
    topLayout->addWidget(btnOpen);
    topLayout->addWidget(m_chkHexDisplay); // <--- 加在这里
    topLayout->addWidget(m_chkPacketMode); // <--- 加在这里
    topLayout->addWidget(m_chkGBK);

    // [中间] 接收区
    QTextEdit* textRecv = new QTextEdit(this);
    textRecv->setReadOnly(true);
    textRecv->setPlaceholderText(tr("串口独立窗口准备就绪..."));

    // ==========================================
        // 【新增】发送区布局
        // ==========================================
    QHBoxLayout* sendLayout = new QHBoxLayout();

    // 1. 输入框 (高度设小一点，不用太高)
    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setMaximumHeight(60);
    m_inputEdit->setPlaceholderText(tr("请输入要发送的数据..."));

    // 2. Hex发送开关
    m_chkHexSend = new QCheckBox(tr("Hex发送"), this);

    // 3. 发送按钮
    JellyButton* btnSend = new JellyButton(tr("发送"), this);
    btnSend->setFixedSize(100, 40);

    // 组装发送区
    sendLayout->addWidget(m_chkHexSend);
    sendLayout->addWidget(m_inputEdit);
    sendLayout->addWidget(btnSend);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(textRecv);
    mainLayout->addLayout(sendLayout);

    // ==========================================
    // 3. 信号连接 (逻辑搬运)
    // ==========================================

    // 开关串口
    connect(btnOpen, &QPushButton::clicked, this, [=]() {
        if (m_worker->isOpen()) {
            m_worker->closePort();
            btnOpen->setText(tr("打开串口"));
            btnOpen->setStyleSheet("");
            comboPorts->setEnabled(true);
        }
        else {
            if (m_worker->openPort(comboPorts->currentText(),9600)) {
                btnOpen->setText(tr("关闭串口"));
                btnOpen->setStyleSheet("QPushButton { background-color: #ff4b4b; color: white; border-radius: 20px; border: 2px solid white; font-weight: bold;}");
                comboPorts->setEnabled(false);
            }
        }
        });

    // 发送数据
    connect(btnSend, &QPushButton::clicked, this, [=]() {
        QString text = m_inputEdit->toPlainText();

        if (text.isEmpty()) {
            return; // 没内容就不发
        }

        QByteArray dataToSend;

        // 2. 判断发送模式
        if (m_chkHexSend->isChecked()) {
            // --- HEX 发送模式 ---
            // 用户可能输入 "AA 55 FF" 或者 "AA,55,FF"
            // QByteArray::fromHex 非常智能，它会自动忽略空格、逗号等非Hex字符
            // 只要你是 0-9, A-F 就能识别
            dataToSend = QByteArray::fromHex(text.toLatin1());
        }
        else {
            // --- 文本发送模式 ---

            // 既然你之前加了 GBK 接收，这里最好也联动一下
            // 如果你希望发送也支持 GBK (给单片机发中文)：
            if (m_chkGBK->isChecked()) {
                dataToSend = text.toLocal8Bit(); // 转为 GBK/Windows本地编码
            }
            else {
                dataToSend = text.toUtf8();      // 转为 UTF-8 (标准)
            }

            // 如果你想简单点，不看 GBK 开关，直接用 toUtf8() 也行
        }

        // 3. 调用 Worker 发送
        m_worker->sendData(dataToSend);
        });

    // 接收显示
    
    connect(m_worker, &SerialWorker::dataReceived, this, [=](const QByteArray& data) {
        QString time = QDateTime::currentDateTime().toString("[HH:mm:ss] ");
        QString str;

        // 检查复选框是否被勾选
        if (m_chkHexDisplay->isChecked()) {
            // 如果勾选了 HEX：转成 "AA BB CC" 格式
            str = data.toHex(' ').toUpper();
        }
        else if (m_chkGBK->isChecked()) {
            // B. 如果勾选了 GBK，用本地编码 (Windows下通常是GBK) 解码
            // fromLocal8Bit 是解决中文乱码的神器
            str = QString::fromLocal8Bit(data);
        }
        else {
            // 如果没勾选：按文本显示 (UTF-8)
            str = QString::fromUtf8(data);
        }

        textRecv->append(time + str);
        });
        

    connect(m_chkPacketMode, &QCheckBox::toggled, this, [=](bool checked) {
        m_worker->setPacketMode(checked);

        // 可选：切换时在接收框里提示一下
        if (checked) textRecv->append(tr("--- 已切换至协议解析模式 (AA 55 ... FF) ---"));
        else textRecv->append(tr("--- 已切换至原始数据模式 ---"));
        });


    // 错误处理
    connect(m_worker, &SerialWorker::errorOccurred, this, [=](const QString& msg) {
        QMessageBox::warning(this, tr("串口警告"), msg);
        });
}

SerialWindow::~SerialWindow()
{

}