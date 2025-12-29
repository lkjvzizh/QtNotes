#include "BleManager.h"
#include <QDebug>

BleManager::BleManager(QObject* parent) : QObject(parent)
{
	m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
	m_discoveryAgent->setLowEnergyDiscoveryTimeout(5000); // 5秒扫描超时

	connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
		this, &BleManager::onDeviceDiscovered);
	connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
		this, &BleManager::onScanFinished);
}

void BleManager::startScan()
{
	emit statusChanged("正在扫描...");
	// Qt6: LowEnergyMethod 是推荐的扫描方式
	m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BleManager::connectToDevice(const QString& address,
	const QString& serviceUuid,
	const QString& writeUuid,
	const QString& notifyUuid)
{
	if (m_controller) {
		m_controller->disconnectFromDevice();
		delete m_controller;
		m_controller = nullptr;
	}

	// 1. 保存 UUID 配置
	m_targetServiceUuid = QBluetoothUuid(serviceUuid);
	m_targetWriteUuid = QBluetoothUuid(writeUuid);
	m_targetNotifyUuid = QBluetoothUuid(notifyUuid);

	emit statusChanged("正在连接: " + address);

	// 2. [Qt6修正] 必须使用 QBluetoothDeviceInfo 来创建控制器
	QBluetoothAddress btAddress(address);
	QBluetoothDeviceInfo deviceInfo(btAddress, "", 0); // 临时构建一个对象

	m_controller = QLowEnergyController::createCentral(deviceInfo, this);

	// 3. 连接控制器的信号
	connect(m_controller, &QLowEnergyController::connected, this, &BleManager::onConnected);
	connect(m_controller, &QLowEnergyController::disconnected, this, &BleManager::onDisconnected);
	connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BleManager::onServiceDiscovered);
	connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BleManager::onServiceScanDone);
	connect(m_controller, &QLowEnergyController::errorOccurred, this, &BleManager::onControllerError);

	// 4. 发起连接
	m_controller->connectToDevice();
}

void BleManager::onDeviceDiscovered(const QBluetoothDeviceInfo& device)
{
	// 可以在这里加过滤逻辑，比如 if(device.name() == "MyBle")
	emit deviceDiscovered(device.name(), device.address().toString());
}

void BleManager::onScanFinished()
{
	emit statusChanged("扫描结束");
}

void BleManager::onConnected()
{
	emit statusChanged("连接成功，正在发现服务...");
	m_controller->discoverServices();
}

void BleManager::onDisconnected()
{
	emit statusChanged("已断开连接");
}

void BleManager::onControllerError(QLowEnergyController::Error)
{
	emit statusChanged("蓝牙错误: " + m_controller->errorString());
}

void BleManager::onServiceDiscovered(const QBluetoothUuid& newService)
{
	if (newService == m_targetServiceUuid) {
		m_service = m_controller->createServiceObject(newService, this);
		emit statusChanged("目标服务已找到，准备解析...");
	}
}

void BleManager::onServiceScanDone()
{
	if (m_service) {
		// 连接服务对象的信号
		connect(m_service, &QLowEnergyService::stateChanged, this, &BleManager::onServiceStateChanged);
		connect(m_service, &QLowEnergyService::characteristicChanged, this, &BleManager::onCharacteristicChanged);

		// 开始解析服务详情（特征值）
		m_service->discoverDetails();
	}
	else {
		emit statusChanged("错误：未找到指定的服务 UUID");
	}
}

void BleManager::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
	// [Qt6修正] 必须使用 RemoteServiceDiscovered
	if (s == QLowEnergyService::RemoteServiceDiscovered) {

		emit statusChanged("服务解析完成，正在配置特征值...");

		// 1. 寻找写特征值
		QLowEnergyCharacteristic writeChar = m_service->characteristic(m_targetWriteUuid);
		if (writeChar.isValid()) {
			m_writeCharacteristic = writeChar;
		}
		else {
			emit statusChanged("警告: Write UUID 未找到");
		}

		// 2. 寻找通知特征值并开启 Notify
		QLowEnergyCharacteristic notifyChar = m_service->characteristic(m_targetNotifyUuid);
		if (notifyChar.isValid()) {
			// [Qt6修正] 使用 (quint16)0x2902 避免版本兼容性问题
			QLowEnergyDescriptor cccd = notifyChar.descriptor(QBluetoothUuid((quint16)0x2902));
			if (cccd.isValid()) {
				// 写 0100 (小端序) 开启 Notify
				m_service->writeDescriptor(cccd, QByteArray::fromHex("0100"));
				emit statusChanged("Notify 已开启 (CCCD写成功)");
			}
		}
		else {
			emit statusChanged("警告: Notify UUID 未找到");
		}

		if (m_writeCharacteristic.isValid()) {
			emit statusChanged("就绪：可以发送数据");
		}
	}
}

void BleManager::sendData(const QByteArray& data)
{
	if (m_service && m_writeCharacteristic.isValid()) {
		// WriteWithoutResponse 通常用于透传，速度快
		// 如果需要可靠传输，改为 WriteWithResponse
		m_service->writeCharacteristic(m_writeCharacteristic, data, QLowEnergyService::WriteWithoutResponse);
	}
	else {
		emit statusChanged("错误：写特征值无效，无法发送");
	}
}

void BleManager::onCharacteristicChanged(const QLowEnergyCharacteristic& c, const QByteArray& value)
{
	Q_UNUSED(c);
	emit dataReceived(value);
}

void BleManager::disconnectDevice()
{
	if (m_controller) m_controller->disconnectFromDevice();
}