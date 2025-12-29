#pragma once
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer> // <--- 1. 引入定时器头文件

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject* parent = nullptr);
    ~SerialWorker();

    static QStringList getAvailablePorts();
    bool openPort(const QString& portName, int baudRate);
    void closePort();
    bool isOpen() const;
    void sendData(const QByteArray& data);

    // 【新增】设置是否开启 AA55 粘包处理模式
    void setPacketMode(bool enable);

signals:
    // 【统一信号】无论是不是粘包，最终解析好的数据都通过这个信号发出去
    // 这样 UI 只需要连接这一个信号就行了，不用管底层怎么处理的
    void dataReceived(const QByteArray& data);

    void errorOccurred(const QString& msg);

private slots:
    // 【修改】这是唯一的入口槽函数
    void onGlobalReadyRead();
    void onRawTimerTimeout(); // <--- 2. 新增：定时器时间到了执行的函数
private:
    QSerialPort* m_serial;
    QByteArray m_buffer;

    // 【新增】模式标记：true=粘包处理，false=原始数据
    bool m_isPacketMode = false;

    // <--- 3. 新增：原始数据模式用的变量
    QTimer* m_rawTimer;       // 延时定时器
    QByteArray m_rawBuffer;   // 原始数据暂存区
    int m_rawDelayMs = 50;    // 延时时间(毫秒)，可调整

    // 把之前的逻辑变成私有辅助函数
    void processRawData();   // 处理原始数据
    void processPacketData();// 处理粘包 AA 55
};