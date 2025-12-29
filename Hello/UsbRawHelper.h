#pragma once
#include <QObject>
#include <QTimer>
#include "libusb.h" // 确保你配置好了 libusb 的 include 路径

class UsbRawHelper : public QObject
{
	Q_OBJECT

public:
	explicit UsbRawHelper(QObject* parent = nullptr);
	~UsbRawHelper();

	// --- 基础控制 ---
	bool initUsb();
	QStringList scanDevices();
	bool openDevice(int vid, int pid);
	void closeDevice();

	// --- 数据收发 ---
	// 发送数据
	void writeData(const QByteArray& data);

	// 手动读取一次 (超时时间较长，用于测试)
	void readOnce(int timeout = 1000);

	// --- ✅ 自动循环读取控制 ---
	void startLoopRead();
	void stopLoopRead();

signals:
	// 发送日志消息
	void logMessage(const QString& msg);
	// 发送收到的原始数据 (方便外部处理)
	void dataReceived(const QByteArray& data);

private slots:
	// 内部定时器槽函数
	void onTimerTick();

private:
	libusb_context* m_ctx = nullptr;
	libusb_device_handle* m_handle = nullptr;

	// 端点地址
	unsigned char m_endpointIn = 0;
	unsigned char m_endpointOut = 0;

	// 内部定时器
	QTimer* m_timer;
};