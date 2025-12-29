#pragma once
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel> // ✅ 新增 Label
#include "UsbRawHelper.h"

class UsbRawWindow : public QWidget
{
	Q_OBJECT

public:
	explicit UsbRawWindow(QWidget* parent = nullptr);

private slots:
	void onScan();
	void onOpen();
	void onSend();
	void onReadOnce();
	void onLog(const QString& msg);

	// ✅ 新增：专门处理原始数据的槽函数
	void onDataReceived(const QByteArray& data);

private:
	UsbRawHelper* m_helper;

	// UI 控件
	QListWidget* m_listDevices;
	QTextEdit* m_textLog;
	QLineEdit* m_lineSend;

	QPushButton* m_btnScan;
	QPushButton* m_btnOpen;
	QPushButton* m_btnRead;
	QPushButton* m_btnSend;

	QCheckBox* m_checkAuto;

	// ✅ 新增：显示接收到的总字节数
	QLabel* m_lblCount;
	long long    m_totalBytes = 0; // 计数变量
};