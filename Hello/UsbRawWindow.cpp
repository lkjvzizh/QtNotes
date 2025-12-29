#include "UsbRawWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>

UsbRawWindow::UsbRawWindow(QWidget* parent) : QWidget(parent)
{
	this->setWindowTitle("原生 USB (LibUSB) 调试终端");
	this->resize(700, 600);

	m_helper = new UsbRawHelper(this);

	if (!m_helper->initUsb()) {
		QMessageBox::critical(this, "错误", "LibUSB 初始化失败！");
	}

	// --- 布局设计 ---
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// 1. 设备控制区
	QGroupBox* grpDevice = new QGroupBox("设备操作", this);
	QHBoxLayout* layoutDevice = new QHBoxLayout(grpDevice);

	m_btnScan = new QPushButton("1. 扫描设备", this);
	m_btnOpen = new QPushButton("2. 打开设备", this);
	m_btnOpen->setCheckable(true);

	m_btnRead = new QPushButton("手动读取一次", this);
	m_checkAuto = new QCheckBox("开启自动接收", this);

	layoutDevice->addWidget(m_btnScan);
	layoutDevice->addWidget(m_btnOpen);
	layoutDevice->addWidget(m_btnRead);
	layoutDevice->addWidget(m_checkAuto);

	// 2. 列表与日志
	QHBoxLayout* layoutCenter = new QHBoxLayout;

	QVBoxLayout* layoutList = new QVBoxLayout;
	layoutList->addWidget(new QLabel("设备列表 (VID:PID):"));
	m_listDevices = new QListWidget(this);
	layoutList->addWidget(m_listDevices);

	QVBoxLayout* layoutLog = new QVBoxLayout;

	// ✅ 在日志上方增加一个计数显示栏
	QHBoxLayout* layoutInfo = new QHBoxLayout;
	layoutInfo->addWidget(new QLabel("运行日志 / 数据接收:"));
	layoutInfo->addStretch(); // 弹簧
	m_lblCount = new QLabel("接收总量: 0 Bytes", this);
	// 给它加个样式，醒目一点
	m_lblCount->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
	layoutInfo->addWidget(m_lblCount);

	layoutLog->addLayout(layoutInfo); // 加入带计数器的标题栏

	m_textLog = new QTextEdit(this);
	m_textLog->setReadOnly(true);
	m_textLog->setStyleSheet("background-color: #1e1e1e; color: #00ff00; font-family: Consolas;");
	layoutLog->addWidget(m_textLog);

	layoutCenter->addLayout(layoutList, 1);
	layoutCenter->addLayout(layoutLog, 2);

	// 3. 发送区
	QGroupBox* grpSend = new QGroupBox("数据发送 (Bulk Out)", this);
	QHBoxLayout* layoutSend = new QHBoxLayout(grpSend);
	m_lineSend = new QLineEdit(this);
	m_lineSend->setPlaceholderText("请输入 Hex (如: AA BB 01)");
	m_btnSend = new QPushButton("发送 Hex", this);
	layoutSend->addWidget(m_lineSend);
	layoutSend->addWidget(m_btnSend);

	mainLayout->addWidget(grpDevice);
	mainLayout->addLayout(layoutCenter);
	mainLayout->addWidget(grpSend);

	// --- 信号连接 ---
	connect(m_btnScan, &QPushButton::clicked, this, &UsbRawWindow::onScan);
	connect(m_btnOpen, &QPushButton::clicked, this, &UsbRawWindow::onOpen);
	connect(m_btnRead, &QPushButton::clicked, this, &UsbRawWindow::onReadOnce);
	connect(m_btnSend, &QPushButton::clicked, this, &UsbRawWindow::onSend);

	// 连接信号
	connect(m_helper, &UsbRawHelper::logMessage, this, &UsbRawWindow::onLog);
	// ✅ 连接原始数据信号
	// 只要 Helper 收到了数据（无论是手动还是自动），这个槽函数都会被调用
	connect(m_helper, &UsbRawHelper::dataReceived, this, &UsbRawWindow::onDataReceived);


	connect(m_checkAuto, &QCheckBox::checkStateChanged, this, [=](int state) {
		if (state == Qt::Checked) {
			m_helper->startLoopRead();
			m_btnRead->setEnabled(false);
		}
		else {
			m_helper->stopLoopRead();
			m_btnRead->setEnabled(true);
		}
		});

	onScan();
}

// ✅ 核心功能：处理实时数据
void UsbRawWindow::onDataReceived(const QByteArray& data)
{
	// 1. 累加字节数
	m_totalBytes += data.size();

	// 2. 刷新界面显示
	m_lblCount->setText(QString("接收总量: %1 Bytes").arg(m_totalBytes));

	// 💡 可以在这里做更多事，例如：
	// - 解析协议头 (if data[0] == 0xAA) ...
	// - 保存到文件 ...
	// - 绘制波形图 ...
}

void UsbRawWindow::onScan()
{
	m_listDevices->clear();
	QStringList devs = m_helper->scanDevices();
	m_listDevices->addItems(devs);
	onLog("扫描结束，设备列表已更新。");
}

void UsbRawWindow::onOpen()
{
	if (m_btnOpen->isChecked()) {
		QListWidgetItem* item = m_listDevices->currentItem();
		if (!item) {
			onLog("⚠️ 请先在列表选择一个设备！");
			m_btnOpen->setChecked(false);
			return;
		}

		QStringList parts = item->text().split(":");
		if (parts.size() != 2) return;

		bool ok;
		int vid = parts[0].toInt(&ok, 16);
		int pid = parts[1].toInt(&ok, 16);

		if (m_helper->openDevice(vid, pid)) {
			m_btnOpen->setText("关闭设备");
			m_listDevices->setEnabled(false);
			m_btnScan->setEnabled(false);

			// 打开设备时清零计数
			m_totalBytes = 0;
			m_lblCount->setText("接收总量: 0 Bytes");
		}
		else {
			m_btnOpen->setChecked(false);
		}
	}
	else {
		m_checkAuto->setChecked(false);
		m_helper->stopLoopRead();
		m_helper->closeDevice();

		m_btnOpen->setText("打开设备");
		m_listDevices->setEnabled(true);
		m_btnScan->setEnabled(true);
	}
}

void UsbRawWindow::onSend()
{
	QString text = m_lineSend->text().remove(" ");
	if (text.isEmpty()) return;
	QByteArray data = QByteArray::fromHex(text.toUtf8());
	m_helper->writeData(data);
}

void UsbRawWindow::onReadOnce()
{
	m_helper->readOnce();
}

void UsbRawWindow::onLog(const QString& msg)
{
	m_textLog->append(msg);
	m_textLog->moveCursor(QTextCursor::End);
}