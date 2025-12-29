#ifndef BLUETOOTHWINDOW_H
#define BLUETOOTHWINDOW_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFrame>

#include "BleManager.h"

class BluetoothWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BluetoothWindow(QWidget* parent = nullptr);
	~BluetoothWindow();

private slots:
	// UI 事件
	void onScanClicked();
	void onSendClicked();
	void onDeviceItemClicked(QListWidgetItem* item);

	// 业务逻辑回调
	void updateStatus(QString msg);
	void addDevice(QString name, QString addr);
	void appendLog(QByteArray data);

private:
	void setupUi();

	// UI 控件
	QLineEdit* m_editServiceUuid;
	QLineEdit* m_editWriteUuid;  // 写 UUID
	QLineEdit* m_editNotifyUuid; // 收 UUID

	QPushButton* m_btnScan;
	QListWidget* m_listDevices;
	QTextBrowser* m_textLogs;

	QCheckBox* m_checkHex; // Hex发送复选框
	QLineEdit* m_editSend;
	QPushButton* m_btnSend;

	QLabel* m_statusLabel; // 底部状态栏

	BleManager* m_ble;
};

#endif // BLUETOOTHWINDOW_H