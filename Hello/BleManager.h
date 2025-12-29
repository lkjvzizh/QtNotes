#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothUuid>

class BleManager : public QObject
{
	Q_OBJECT
public:
	explicit BleManager(QObject* parent = nullptr);

	// 开始扫描
	void startScan();

	// 连接设备：支持分别指定 写UUID 和 收(Notify)UUID
	void connectToDevice(const QString& address,
		const QString& serviceUuid,
		const QString& writeUuid,
		const QString& notifyUuid);

	// 发送数据
	void sendData(const QByteArray& data);

	// 断开连接
	void disconnectDevice();

signals:
	void deviceDiscovered(QString name, QString address);
	void statusChanged(QString status);
	void dataReceived(QByteArray data);

private slots:
	void onDeviceDiscovered(const QBluetoothDeviceInfo& device);
	void onScanFinished();

	void onConnected();
	void onDisconnected();
	void onServiceDiscovered(const QBluetoothUuid& newService);
	void onServiceScanDone();

	void onServiceStateChanged(QLowEnergyService::ServiceState s);
	void onCharacteristicChanged(const QLowEnergyCharacteristic& c, const QByteArray& value);
	void onControllerError(QLowEnergyController::Error);

private:
	QBluetoothDeviceDiscoveryAgent* m_discoveryAgent;
	QLowEnergyController* m_controller = nullptr;
	QLowEnergyService* m_service = nullptr;

	// 保存关键的特征值对象用于写入
	QLowEnergyCharacteristic m_writeCharacteristic;

	// 目标 UUID 配置
	QBluetoothUuid m_targetServiceUuid;
	QBluetoothUuid m_targetWriteUuid;
	QBluetoothUuid m_targetNotifyUuid;
};

#endif // BLEMANAGER_H