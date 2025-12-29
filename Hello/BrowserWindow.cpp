#include "BrowserWindow.h"

#include <QVBoxLayout>
#include <QCoreApplication>
#include <QDir>
#include <QTime>
#include <QDebug>

// WebEngine 核心
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineNewWindowRequest> 

BrowserWindow::BrowserWindow(QWidget* parent)
	: QWidget(parent)
{
	// 1. 设置窗口属性
	this->resize(1200, 800);
	this->setWindowTitle("C++ 极简浏览器容器");

	// 2. 创建唯一的控件：WebEngineView
	m_webView = new QWebEngineView(this);

	// 3. 布局：让浏览器填满整个窗口，没有边距
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0); // 0边距，实现沉浸式效果
	layout->addWidget(m_webView);

	// 4. 配置 WebEngine (开启 JS、跨域等)
	QWebEngineSettings* settings = m_webView->settings();
	settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
	settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
	settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
	settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

	// 5. 建立通信通道 (C++ <-> JS)
	m_webChannel = new QWebChannel(this);
	m_bridge = new CoreBridge(this);
	m_webChannel->registerObject(QStringLiteral("core"), m_bridge);
	m_webView->page()->setWebChannel(m_webChannel);

	// 6. 拦截弹窗 (防止点击链接跳出新窗口)
	connect(m_webView->page(), &QWebEnginePage::newWindowRequested, this,
		[this](QWebEngineNewWindowRequest& request) {
			m_webView->load(request.requestedUrl());
		}
	);

	// 7. 启动定时器 (测试 C++ 主动发消息给网页)
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &BrowserWindow::onTimerTick);
	m_timer->start(1000); // 每秒触发一次

	// 8. 加载本地网页
	QString localHtml = QCoreApplication::applicationDirPath() + "/index.html";
	if (QFile::exists(localHtml)) {
		qDebug() << "加载本地文件:" << localHtml;
		m_webView->load(QUrl("file:///" + localHtml));
	}
	else {
		qDebug() << "未找到 index.html，加载百度";
		m_webView->load(QUrl("https://www.baidu.com"));
	}
}

BrowserWindow::~BrowserWindow()
{
}

void BrowserWindow::onTimerTick()
{
	// 获取当前时间
	QString timeStr = QTime::currentTime().toString("HH:mm:ss");

	// 发送信号！网页里的 js 会收到 sendDataToWeb
	emit m_bridge->sendDataToWeb("C++ 后台时间: " + timeStr);
}