#pragma once

#include <QWidget>
#include <QWebEngineView> // 核心浏览器组件
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>   // 进度条

class WebWindow : public QWidget
{
	Q_OBJECT

public:
	explicit WebWindow(QWidget* parent = nullptr);
	~WebWindow();

	// 公开接口：加载指定网址
	void loadUrl(const QString& url);

private slots:
	// 界面交互槽函数
	void onGoClicked();       // 点击"前往"或回车
	void onBackClicked();     // 点击后退
	void onRefreshClicked();  // 点击刷新

	// 浏览器回调槽函数
	void onUrlChanged(const QUrl& url);        // 网页变了 -> 更新地址栏
	void onTitleChanged(const QString& title); // 标题变了 -> 更新窗口标题
	void onLoadProgress(int progress);         // 加载中 -> 更新进度条

private:
	// 核心浏览器控件
	QWebEngineView* m_webView;

	// UI 控件
	QLineEdit* m_lineUrl;
	QPushButton* m_btnBack;
	QPushButton* m_btnRefresh;
	QPushButton* m_btnGo;
	QProgressBar* m_progressBar;
};