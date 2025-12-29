#include "wsClientWindow.h"
#include "WsClientManager.h" // 确保引用了管理器头文件

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>

wsClientWindow::wsClientWindow(QWidget* parent) : QWidget(parent) {
	setWindowTitle("Qt WebSocket Client (wsClientWindow)");
	resize(400, 300);

	// --- 1. UI 初始化 ---
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	m_logDisplay = new QTextEdit(this);
	m_logDisplay->setReadOnly(true);

	QHBoxLayout* inputLayout = new QHBoxLayout();
	m_inputEdit = new QLineEdit(this);
	m_btnSend = new QPushButton("发送", this);
	inputLayout->addWidget(m_inputEdit);
	inputLayout->addWidget(m_btnSend);

	mainLayout->addWidget(m_logDisplay);
	mainLayout->addLayout(inputLayout);

	// --- 2. 逻辑初始化 ---
	m_manager = new WsClientManager(this);

	// --- 3. 信号连接 ---
	connect(m_btnSend, &QPushButton::clicked, this, &wsClientWindow::onBtnSendClicked);

	connect(m_manager, &WsClientManager::sigConnected, this, &wsClientWindow::onServerConnected);
	connect(m_manager, &WsClientManager::sigDisconnected, this, &wsClientWindow::onServerDisconnected);
	connect(m_manager, &WsClientManager::sigMessageReceived, this, &wsClientWindow::onMsgReceived);

	// --- 4. 自动连接 ---
	log("正在尝试连接服务器...");
	m_manager->connectToServer("ws://localhost:12345");
}

wsClientWindow::~wsClientWindow() {
	// m_manager 会随父对象自动释放
}

void wsClientWindow::onBtnSendClicked() {
	QString text = m_inputEdit->text();
	if (text.isEmpty()) return;

	m_manager->sendMsg(text);
	m_inputEdit->clear();
}

void wsClientWindow::onServerConnected() {
	log(">>> 连接成功 (Ready)");
}

void wsClientWindow::onServerDisconnected() {
	log(">>> 连接断开 (Offline)");
}

void wsClientWindow::onMsgReceived(const QString& msg) {
	log("Server: " + msg);
}

void wsClientWindow::log(const QString& msg) {
	QString timeStr = QTime::currentTime().toString("HH:mm:ss");
	m_logDisplay->append(QString("[%1] %2").arg(timeStr, msg));
}