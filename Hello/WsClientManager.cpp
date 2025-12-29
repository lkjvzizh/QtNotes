#include "WsClientManager.h"
#include <QDebug>

WsClientManager::WsClientManager(QObject* parent) : QObject(parent) {
	m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

	// 内部信号绑定
	connect(m_socket, &QWebSocket::connected, this, &WsClientManager::onConnected);
	connect(m_socket, &QWebSocket::disconnected, this, &WsClientManager::onDisconnected);
	connect(m_socket, &QWebSocket::textMessageReceived, this, &WsClientManager::onTextMessageReceived);
}

WsClientManager::~WsClientManager() {
	// 1. 先断开 socket 的所有信号连接
	// 这样 socket 关闭时发出的 disconnected 信号就不会触发 onDisconnected 了
	m_socket->disconnect();
	// 2. 然后再安全关闭
	m_socket->close();
}

void WsClientManager::connectToServer(const QString& url) {
	m_socket->open(QUrl(url));
}

void WsClientManager::sendMsg(const QString& msg) {
	if (m_socket->isValid()) {
		m_socket->sendTextMessage(msg);
	}
	else {
		qDebug() << "Error: WebSocket is not connected.";
	}
}

void WsClientManager::closeConnection() {
	m_socket->close();
}

void WsClientManager::onConnected() {
	qDebug() << "WS Connected!";
	emit sigConnected();
}

void WsClientManager::onDisconnected() {
	qDebug() << "WS Disconnected!";
	emit sigDisconnected();
}

void WsClientManager::onTextMessageReceived(const QString& frame) {
	emit sigMessageReceived(frame);
}