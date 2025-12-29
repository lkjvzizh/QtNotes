#include "SerialWorker.h"
#include <QDebug>

SerialWorker::SerialWorker(QObject* parent)
    : QObject(parent)
{
    m_serial = new QSerialPort(this);

    // 连接到总入口
    connect(m_serial, &QSerialPort::readyRead, this, &SerialWorker::onGlobalReadyRead);

    // ==========================================
    // 1. 初始化定时器
    // ==========================================
    m_rawTimer = new QTimer(this);
    m_rawTimer->setSingleShot(true); // 单次触发模式（只响一次）

    // 当定时器时间到了，就去执行发送函数
    connect(m_rawTimer, &QTimer::timeout, this, &SerialWorker::onRawTimerTimeout);
}

SerialWorker::~SerialWorker()
{
    closePort();
}

// 【这里是你需要的函数】
QStringList SerialWorker::getAvailablePorts()
{
    QStringList ports;
    // 遍历系统所有可用串口
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts()) {
        ports.append(info.portName());
    }

    return ports;
}

bool SerialWorker::openPort(const QString& portName, int baudRate)
{
    if (m_serial->isOpen()) closePort();

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        return true;
    }
    else {
        emit errorOccurred(tr("打开失败: ") + m_serial->errorString());
        return false;
    }
}

void SerialWorker::closePort()
{
    if (m_serial->isOpen()) {
        m_serial->clear();
        m_serial->close();
    }
}

bool SerialWorker::isOpen() const
{
    return m_serial->isOpen();
}

void SerialWorker::sendData(const QByteArray& data)
{
    if (!m_serial->isOpen()) {
        emit errorOccurred(tr("发送失败：串口未打开"));
        return;
    }
    m_serial->write(data);
}

void SerialWorker::setPacketMode(bool enable)
{
    m_isPacketMode = enable;
    m_buffer.clear(); // 切换模式时清空缓冲区，防止乱码

    // 切换模式时，也要把原始数据的缓存和定时器停掉，防止冲突
    m_rawBuffer.clear();
    m_rawTimer->stop();
}


// ==========================================
// 3. 新增：定时器超时处理
// ==========================================
void SerialWorker::onRawTimerTimeout()
{
    // 时间到了，说明对方发完了（或者暂时歇气了）
    if (!m_rawBuffer.isEmpty()) {
        // 把攒好的数据一次性发出去
        emit dataReceived(m_rawBuffer);

        // 清空缓冲区，准备接下一波
        m_rawBuffer.clear();
    }
}

// ==========================================
// 核心接收逻辑
// ==========================================

void SerialWorker::onGlobalReadyRead()
{
    if (m_isPacketMode) {
        processPacketData(); // 走协议解析
    }
    else {
        processRawData();    // 走原始透传
    }
}

void SerialWorker::processRawData()
{
    // 读取新来的数据
    QByteArray data = m_serial->readAll();

    // 存入原始数据缓冲区
    m_rawBuffer.append(data);

    // 【核心】启动(或重启)定时器
    // 如果定时器已经在跑了，start() 会让它重新开始计时
    // 50ms 是个经验值，对于 9600 波特率足够了，如果数据断得厉害可以设成 100
    m_rawTimer->start(m_rawDelayMs);
}


void SerialWorker::processPacketData()
{
    QByteArray temp = m_serial->readAll();
    m_buffer.append(temp);

    while (!m_buffer.isEmpty()) {
        // 1. 找头 (AA 55)
        int headerIndex = m_buffer.indexOf(QByteArray::fromHex("AA 55"));
        if (headerIndex < 0) {
            if (m_buffer.size() > 1000) m_buffer.clear();
            break;
        }

        // 2. 去掉头前面的垃圾
        if (headerIndex > 0) {
            m_buffer.remove(0, headerIndex);
            headerIndex = 0;
        }

        // 3. 找尾 (FF)
        int tailIndex = m_buffer.indexOf(0xFF);
        if (tailIndex < 0) {
            break; // 数据没收完，等下一次
        }

        // 4. 提取完整包
        int packetLen = tailIndex + 1;
        QByteArray packet = m_buffer.mid(0, packetLen);

        // 5. 发送
        emit dataReceived(packet);

        // 6. 清理
        m_buffer.remove(0, packetLen);
    }
}