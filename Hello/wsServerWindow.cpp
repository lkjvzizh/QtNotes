#include "wsServerWindow.h"
#include <QWebSocketServer>
#include <QWebSocket>
#include <QVBoxLayout>
#include <QHBoxLayout> // <--- 布局需要
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>   // <--- 控件需要
#include <QPushButton> // <--- 控件需要
#include <QTime>
#include <utility>     // std::as_const 需要

wsServerWindow::wsServerWindow(QWidget* parent) : QWidget(parent) {
	setWindowTitle("Qt WebSocket Server");
	resize(400, 400); //稍微调高一点窗口

	// --- 1. 主布局 ---
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// 日志区域
	m_logDisplay = new QTextEdit(this);
	m_logDisplay->setReadOnly(true);
	mainLayout->addWidget(new QLabel("服务器日志:", this));
	mainLayout->addWidget(m_logDisplay);

	// --- 2. 新增：发送区域布局 ---
	QHBoxLayout* inputLayout = new QHBoxLayout();
	m_inputEdit = new QLineEdit(this);
	m_inputEdit->setPlaceholderText("输入要广播给所有客户端的消息...");

	m_btnSend = new QPushButton("全员广播", this);

	inputLayout->addWidget(m_inputEdit);
	inputLayout->addWidget(m_btnSend);

	// 将发送区域加入主布局
	mainLayout->addLayout(inputLayout);

	// --- 3. 信号连接 ---
	// 按钮点击 -> 触发发送逻辑
	connect(m_btnSend, &QPushButton::clicked, this, &wsServerWindow::onBtnSendClicked);


	// --- 4. WebSocket 初始化 ---
	m_server = new QWebSocketServer("MyEchoServer", QWebSocketServer::NonSecureMode, this);

	if (m_server->listen(QHostAddress::Any, 12345)) {
		log("服务器监听中，端口: 12345...");
		connect(m_server, &QWebSocketServer::newConnection, this, &wsServerWindow::onNewConnection);
	}
	else {
		log("错误：监听失败！");
	}
}

wsServerWindow::~wsServerWindow() {
	m_server->close();
	qDeleteAll(m_clients.begin(), m_clients.end());
}

// --- 新增：实现发送逻辑 ---
void wsServerWindow::onBtnSendClicked() {
	QString text = m_inputEdit->text();
	if (text.isEmpty()) return;

	if (m_clients.isEmpty()) {
		log("提示：当前没有连接的客户端，无法发送。");
		return;
	}

	// 给每一个连接的客户端发送消息
	for (QWebSocket* pClient : std::as_const(m_clients)) {
		pClient->sendTextMessage("[管理员]: " + text);
	}

	log("已广播: " + text);
	m_inputEdit->clear();
}

void wsServerWindow::onNewConnection() {
	QWebSocket* pSocket = m_server->nextPendingConnection();

	connect(pSocket, &QWebSocket::textMessageReceived, this, &wsServerWindow::processTextMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &wsServerWindow::socketDisconnected);

	m_clients << pSocket;
	log("新客户端接入: " + pSocket->peerAddress().toString());
}

void wsServerWindow::processTextMessage(const QString& message) {
	// 收到消息后，依然执行“回声/转发”逻辑
	QString forwardMsg = "用户转发: " + message;

	for (QWebSocket* pClient : std::as_const(m_clients)) {
		pClient->sendTextMessage(forwardMsg);
	}

	log("收到并转发: " + message);
}

void wsServerWindow::socketDisconnected() {
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	if (pClient) {
		m_clients.removeAll(pClient);
		pClient->deleteLater();
		log("客户端断开连接");
	}
}

void wsServerWindow::log(const QString& msg) {
	QString timeStr = QTime::currentTime().toString("HH:mm:ss");
	m_logDisplay->append(QString("[%1] %2").arg(timeStr, msg));
}