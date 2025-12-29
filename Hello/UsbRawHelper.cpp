#include "UsbRawHelper.h"
#include <QDebug>

UsbRawHelper::UsbRawHelper(QObject* parent)
	: QObject(parent)
{
	m_ctx = nullptr;
	m_handle = nullptr;
	m_endpointIn = 0;
	m_endpointOut = 0;

	// ✅ 初始化内部定时器
	m_timer = new QTimer(this);
	m_timer->setInterval(20); // 每 20ms 尝试读取一次
	connect(m_timer, &QTimer::timeout, this, &UsbRawHelper::onTimerTick);
}

UsbRawHelper::~UsbRawHelper()
{
	stopLoopRead();
	closeDevice();
	if (m_ctx) {
		libusb_exit(m_ctx);
		m_ctx = nullptr;
	}
}

bool UsbRawHelper::initUsb()
{
	int r = libusb_init(&m_ctx);
	if (r < 0) {
		emit logMessage("❌ libusb 初始化失败");
		return false;
	}
	// 设置日志级别
	libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
	return true;
}

QStringList UsbRawHelper::scanDevices()
{
	QStringList list;
	libusb_device** devs;

	ssize_t cnt = libusb_get_device_list(m_ctx, &devs);
	if (cnt < 0) return list;

	for (ssize_t i = 0; i < cnt; i++) {
		libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(devs[i], &desc);
		if (r < 0) continue;

		QString info = QString("%1:%2")
			.arg(desc.idVendor, 4, 16, QChar('0'))
			.arg(desc.idProduct, 4, 16, QChar('0'))
			.toUpper();
		list << info;
	}
	libusb_free_device_list(devs, 1);
	return list;
}

bool UsbRawHelper::openDevice(int vid, int pid)
{
	if (m_handle) closeDevice();

	m_handle = libusb_open_device_with_vid_pid(m_ctx, vid, pid);
	if (m_handle == nullptr) {
		emit logMessage(QString("❌ 无法打开设备 %1:%2 (请检查驱动是否为 WinUSB)").arg(vid, 16).arg(pid, 16));
		return false;
	}

	m_endpointIn = 0;
	m_endpointOut = 0;

	// --- 自动寻找端点 ---
	libusb_device* dev = libusb_get_device(m_handle);
	libusb_config_descriptor* config;
	libusb_get_active_config_descriptor(dev, &config);

	const libusb_interface_descriptor* inter = &config->interface[0].altsetting[0];

	emit logMessage(QString("设备已打开，接口包含 %1 个端点").arg(inter->bNumEndpoints));

	for (int j = 0; j < inter->bNumEndpoints; j++) {
		const libusb_endpoint_descriptor* ep = &inter->endpoint[j];
		if ((ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN) {
			if (m_endpointIn == 0) {
				m_endpointIn = ep->bEndpointAddress;
				emit logMessage(QString("  -> ✅ 锁定 输入端点(IN): 0x%1").arg(m_endpointIn, 2, 16, QChar('0')));
			}
		}
		else {
			if (m_endpointOut == 0) {
				m_endpointOut = ep->bEndpointAddress;
				emit logMessage(QString("  -> ✅ 锁定 输出端点(OUT): 0x%1").arg(m_endpointOut, 2, 16, QChar('0')));
			}
		}
	}
	libusb_free_config_descriptor(config);

	if (libusb_kernel_driver_active(m_handle, 0) == 1) {
		libusb_detach_kernel_driver(m_handle, 0);
	}

	int r = libusb_claim_interface(m_handle, 0);
	if (r < 0) {
		emit logMessage("❌ 无法认领接口 0");
		closeDevice();
		return false;
	}

	return true;
}

void UsbRawHelper::closeDevice()
{
	stopLoopRead(); // 关设备前必须停定时器

	if (m_handle) {
		libusb_release_interface(m_handle, 0);
		libusb_close(m_handle);
		m_handle = nullptr;
		m_endpointIn = 0;
		m_endpointOut = 0;
		emit logMessage("设备已关闭");
	}
}

void UsbRawHelper::writeData(const QByteArray& data)
{
	if (!m_handle || m_endpointOut == 0) {
		emit logMessage("❌ 无法发送：设备未就绪");
		return;
	}

	int actual_length = 0;
	QByteArray temp = data;

	int r = libusb_bulk_transfer(m_handle, m_endpointOut,
		(unsigned char*)temp.data(),
		temp.size(),
		&actual_length, 1000);

	if (r == 0) {
		emit logMessage(QString("📤 发送成功 (%1 字节)").arg(actual_length));
	}
	else {
		emit logMessage("❌ 发送失败: " + QString::fromUtf8(libusb_error_name(r)));
	}
}

void UsbRawHelper::readOnce(int timeout)
{
	if (!m_handle || m_endpointIn == 0) return;

	unsigned char data[1024];
	int actual_length = 0;

	int r = libusb_bulk_transfer(m_handle, m_endpointIn, data, sizeof(data), &actual_length, timeout);

	if (r == 0 && actual_length > 0) {
		QByteArray received((char*)data, actual_length);
		emit logMessage("📥 手动收到: " + received.toHex().toUpper());
		emit dataReceived(received);
	}
	else if (r == LIBUSB_ERROR_TIMEOUT) {
		emit logMessage("读取超时 (无数据)");
	}
	else {
		emit logMessage("❌ 读取出错: " + QString::fromUtf8(libusb_error_name(r)));
	}
}

// ✅ 启动自动读取
void UsbRawHelper::startLoopRead()
{
	if (!m_handle || m_endpointIn == 0) {
		emit logMessage("⚠️ 请先打开设备并确保有输入端点");
		return;
	}
	if (!m_timer->isActive()) {
		m_timer->start();
		emit logMessage("🔄 已启动自动接收循环...");
	}
}

// ✅ 停止自动读取
void UsbRawHelper::stopLoopRead()
{
	if (m_timer->isActive()) {
		m_timer->stop();
		emit logMessage("⏹ 已停止自动接收");
	}
}

// ✅ 定时器逻辑：快速轮询
void UsbRawHelper::onTimerTick()
{
	if (!m_handle || m_endpointIn == 0) {
		stopLoopRead();
		return;
	}

	unsigned char data[1024];
	int actual_length = 0;

	// ⚡ 关键：超时仅设为 1ms，避免阻塞 UI 线程
	int r = libusb_bulk_transfer(m_handle, m_endpointIn, data, sizeof(data), &actual_length, 1);

	if (r == 0 && actual_length > 0) {
		QByteArray received((char*)data, actual_length);
		// 发送信号
		emit logMessage("📥 自动收到: " + received.toHex().toUpper());
		emit dataReceived(received);
	}
	// 忽略超时错误，因为大部分时间都是超时的
	else if (r != LIBUSB_ERROR_TIMEOUT && r != LIBUSB_SUCCESS) {
		emit logMessage("❌ 循环读取异常: " + QString::fromUtf8(libusb_error_name(r)));
		stopLoopRead(); // 出错就停下来
	}
}