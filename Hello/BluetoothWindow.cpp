#include "BluetoothWindow.h"

BluetoothWindow::BluetoothWindow(QWidget* parent)
	: QWidget(parent)
{
	setupUi();

	m_ble = new BleManager(this);

	// 连接信号槽
	connect(m_ble, &BleManager::statusChanged, this, &BluetoothWindow::updateStatus);
	connect(m_ble, &BleManager::deviceDiscovered, this, &BluetoothWindow::addDevice);
	connect(m_ble, &BleManager::dataReceived, this, &BluetoothWindow::appendLog);

	connect(m_btnScan, &QPushButton::clicked, this, &BluetoothWindow::onScanClicked);
	connect(m_btnSend, &QPushButton::clicked, this, &BluetoothWindow::onSendClicked);
	connect(m_listDevices, &QListWidget::itemClicked, this, &BluetoothWindow::onDeviceItemClicked);

	setWindowTitle("Qt 6 BLE Tool (v1.0)");
	resize(450, 700);
}

BluetoothWindow::~BluetoothWindow()
{
}

void BluetoothWindow::setupUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// --- 1. 配置区域 ---
	QGroupBox* groupConfig = new QGroupBox("UUID 配置", this);
	QVBoxLayout* layoutConfig = new QVBoxLayout(groupConfig);

	// Service UUID
	layoutConfig->addWidget(new QLabel("Service UUID:", groupConfig));
	m_editServiceUuid = new QLineEdit(groupConfig);
	m_editServiceUuid->setText("0000ffe0-0000-1000-8000-00805f9b34fb"); // 默认值
	layoutConfig->addWidget(m_editServiceUuid);

	// Write UUID (Tx)
	layoutConfig->addWidget(new QLabel("Write UUID (Tx):", groupConfig));
	m_editWriteUuid = new QLineEdit(groupConfig);
	m_editWriteUuid->setText("0000ffe1-0000-1000-8000-00805f9b34fb");
	layoutConfig->addWidget(m_editWriteUuid);

	// Notify UUID (Rx)
	layoutConfig->addWidget(new QLabel("Notify UUID (Rx):", groupConfig));
	m_editNotifyUuid = new QLineEdit(groupConfig);
	m_editNotifyUuid->setText("0000ffe1-0000-1000-8000-00805f9b34fb"); // 如果读写一样，填一样的即可
	layoutConfig->addWidget(m_editNotifyUuid);

	mainLayout->addWidget(groupConfig);

	// --- 2. 扫描区域 ---
	m_btnScan = new QPushButton("扫描设备", this);
	mainLayout->addWidget(m_btnScan);

	mainLayout->addWidget(new QLabel("设备列表 (点击连接):", this));
	m_listDevices = new QListWidget(this);
	mainLayout->addWidget(m_listDevices);

	// --- 3. 日志区域 ---
	mainLayout->addWidget(new QLabel("收发日志:", this));
	m_textLogs = new QTextBrowser(this);
	mainLayout->addWidget(m_textLogs);

	// --- 4. 发送区域 ---
	QHBoxLayout* layoutSend = new QHBoxLayout();

	m_checkHex = new QCheckBox("Hex发送", this);
	layoutSend->addWidget(m_checkHex);

	m_editSend = new QLineEdit(this);
	m_editSend->setPlaceholderText("输入内容...");
	layoutSend->addWidget(m_editSend);

	m_btnSend = new QPushButton("发送", this);
	layoutSend->addWidget(m_btnSend);

	mainLayout->addLayout(layoutSend);

	// --- 5. 状态栏 ---
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	mainLayout->addWidget(line);

	m_statusLabel = new QLabel("就绪", this);
	m_statusLabel->setStyleSheet("color: blue;");
	mainLayout->addWidget(m_statusLabel);
}

void BluetoothWindow::onScanClicked()
{
	m_listDevices->clear();
	m_ble->startScan();
}

void BluetoothWindow::addDevice(QString name, QString addr)
{
	QString label = name + " | " + addr;
	if (m_listDevices->findItems(addr, Qt::MatchContains).isEmpty()) {
		m_listDevices->addItem(label);
	}
}

void BluetoothWindow::onDeviceItemClicked(QListWidgetItem* item)
{
	QString text = item->text();
	QString addr = text.split("|").last().trimmed();

	QString sUuid = m_editServiceUuid->text();
	QString wUuid = m_editWriteUuid->text();
	QString nUuid = m_editNotifyUuid->text();

	if (sUuid.isEmpty() || wUuid.isEmpty() || nUuid.isEmpty()) {
		updateStatus("错误：UUID 不能为空");
		return;
	}

	m_ble->connectToDevice(addr, sUuid, wUuid, nUuid);
}

void BluetoothWindow::onSendClicked()
{
	QString text = m_editSend->text();
	if (text.isEmpty()) return;

	QByteArray dataToSend;

	// Hex 转换逻辑
	if (m_checkHex->isChecked()) {
		dataToSend = QByteArray::fromHex(text.toUtf8());
	}
	else {
		dataToSend = text.toUtf8();
	}

	m_ble->sendData(dataToSend);

	// 日志显示
	QString prefix = m_checkHex->isChecked() ? "Send(Hex): " : "Send(Str): ";
	QString displayData = m_checkHex->isChecked() ? dataToSend.toHex(' ').toUpper() : text;

	m_textLogs->append(prefix + displayData);
	m_editSend->clear();
}

void BluetoothWindow::updateStatus(QString msg)
{
	m_statusLabel->setText(msg);
}

void BluetoothWindow::appendLog(QByteArray data)
{
	// 收到的数据同时显示 Hex 和 文本，方便调试
	QString hex = data.toHex(' ').toUpper();
	QString str = QString::fromUtf8(data);

	m_textLogs->append("Recv: " + hex + " | " + str);
}