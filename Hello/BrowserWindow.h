#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QTimer>
#include "CoreBridge.h"

class BrowserWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BrowserWindow(QWidget* parent = nullptr);
	~BrowserWindow();

private slots:
	// 定时器槽函数 (用于模拟 C++ 主动发数据给网页)
	void onTimerTick();

private:
	// 唯一的控件：浏览器
	QWebEngineView* m_webView;

	// 通信核心
	QWebChannel* m_webChannel;
	CoreBridge* m_bridge;

	// 测试用的定时器
	QTimer* m_timer;
};