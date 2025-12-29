#pragma once
#include <QObject>

class CoreBridge : public QObject
{
	Q_OBJECT
public:
	explicit CoreBridge(QObject* parent = nullptr);

public slots:
	// 供网页调用的函数 (之前写的)
	void showMsgBox(const QString& msg);

	// ✅ 新增：信号区域
signals:
	// 这个信号一旦发射，网页里的 JS 就会收到通知
	// 参数 msg 就是我们要推给网页的数据
	void sendDataToWeb(const QString& msg);
};