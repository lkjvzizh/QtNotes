#pragma once

#include <QWidget>
#include <QList>

// 前置声明
class QWebSocketServer;
class QWebSocket;
class QTextEdit;
class QLineEdit;   // <--- 新增
class QPushButton; // <--- 新增

class wsServerWindow : public QWidget {
	Q_OBJECT

public:
	explicit wsServerWindow(QWidget* parent = nullptr);
	~wsServerWindow();

private slots:
	void onNewConnection();
	void processTextMessage(const QString& message);
	void socketDisconnected();

	// --- 新增：处理服务器发送按钮点击 ---
	void onBtnSendClicked();

private:
	void log(const QString& msg);

	QWebSocketServer* m_server;
	QList<QWebSocket*> m_clients;

	// UI 控件
	QTextEdit* m_logDisplay;
	QLineEdit* m_inputEdit; // <--- 新增输入框
	QPushButton* m_btnSend; // <--- 新增按钮
};