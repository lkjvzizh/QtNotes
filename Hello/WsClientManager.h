#pragma once

#include <QObject>
#include <QWebSocket>
#include <QUrl>

class WsClientManager : public QObject {
	Q_OBJECT

public:
	explicit WsClientManager(QObject* parent = nullptr);
	~WsClientManager();

	// 接口：连接服务器
	void connectToServer(const QString& url);
	// 接口：发送消息
	void sendMsg(const QString& msg);
	// 接口：断开
	void closeConnection();

signals:
	void sigConnected();
	void sigDisconnected();
	void sigMessageReceived(const QString& msg);

private slots:
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(const QString& frame);

private:
	QWebSocket* m_socket; // 使用指针，方便管理生命周期
};