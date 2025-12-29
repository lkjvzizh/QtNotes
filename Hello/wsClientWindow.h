#pragma once

#include <QWidget>

// 前置声明
class QTextEdit;
class QLineEdit;
class QPushButton;
class WsClientManager;

class wsClientWindow : public QWidget {
	Q_OBJECT

public:
	explicit wsClientWindow(QWidget* parent = nullptr);
	~wsClientWindow();

private slots:
	// UI 按钮事件
	void onBtnSendClicked();

	// 来自 Manager 的信号处理
	void onServerConnected();
	void onServerDisconnected();
	void onMsgReceived(const QString& msg);

private:
	void log(const QString& msg);

	// UI 控件
	QTextEdit* m_logDisplay;
	QLineEdit* m_inputEdit;
	QPushButton* m_btnSend;

	// 通讯管理器 (这个类名保持不变，还是 WsClientManager)
	WsClientManager* m_manager;
};