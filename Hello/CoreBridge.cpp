#include "CoreBridge.h"
#include <QMessageBox>
#include <QDebug>

CoreBridge::CoreBridge(QObject* parent) : QObject(parent)
{
}

void CoreBridge::showMsgBox(const QString& msg)
{
	qDebug() << "🌍 [Web -> C++] showMsgBox:" << msg;

	// 弹出一个原生 Windows 对话框
	QMessageBox::information(nullptr, "C++ 收到消息",
		"网页发来了指令：\n" + msg);
}
