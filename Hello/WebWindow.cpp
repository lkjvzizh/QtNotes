#include "WebWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QIcon> // 如果有图标资源可以用，没有就用文字

// --- 🌐 WebEngine 核心头文件 (Qt 6 必须引用这些) ---
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineNewWindowRequest>

WebWindow::WebWindow(QWidget* parent)
	: QWidget(parent)
{
	// 1. 设置窗口基本属性
	this->resize(1200, 800);
	this->setWindowTitle("Qt WebEngine 浏览器");

	// 2. 初始化控件
	m_webView = new QWebEngineView(this);

	m_btnBack = new QPushButton("⬅ 后退", this);
	m_btnRefresh = new QPushButton("🔄 刷新", this);
	m_btnGo = new QPushButton("前往", this);

	m_lineUrl = new QLineEdit(this);
	m_lineUrl->setPlaceholderText("请输入网址 (例如 bilibili.com)");

	m_progressBar = new QProgressBar(this);
	m_progressBar->setFixedHeight(3); // 进度条设细一点，好看
	m_progressBar->setTextVisible(false); // 不显示百分比数字
	m_progressBar->setRange(0, 100);
	m_progressBar->setValue(0);

	// 3. 布局管理
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0); // 去掉边缘空白，让浏览器铺满
	mainLayout->setSpacing(0);

	// 顶部导航栏布局
	QHBoxLayout* navLayout = new QHBoxLayout;
	navLayout->setContentsMargins(5, 5, 5, 5); // 导航栏留点边距
	navLayout->setSpacing(5);

	navLayout->addWidget(m_btnBack);
	navLayout->addWidget(m_btnRefresh);
	navLayout->addWidget(m_lineUrl, 1); // 地址栏自动拉伸
	navLayout->addWidget(m_btnGo);

	// 组装到主布局
	mainLayout->addLayout(navLayout);
	mainLayout->addWidget(m_progressBar); // 进度条紧贴导航栏下方
	mainLayout->addWidget(m_webView);     // 浏览器占据剩余所有空间


	// --- 4. 核心逻辑配置 (解决弹窗与后退问题) ---

	// (A) 允许网页里的 JavaScript 触发"打开新窗口"的操作
	m_webView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
	m_webView->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
	m_webView->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

	// --- 🚀 新增：开启硬件加速 ---
// 开启 2D 绘图加速 (对 B站 这种包含大量弹幕的 canvas 很有用)
	//m_webView->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvas, true);
	// 开启 WebGL (3D 加速)
	m_webView->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
	// 允许流畅滚动
	m_webView->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);


	// (B) 🚀 关键修复：拦截新窗口请求
	// 当网页想打开新标签页时，我们捕获这个请求，并在"当前页"加载它。
	// 这样既不会被拦截，又属于正常的"页面跳转"，所以"后退按钮"会生效！
	connect(m_webView->page(), &QWebEnginePage::newWindowRequested, this,
		[this](QWebEngineNewWindowRequest& request) {
			QString url = request.requestedUrl().toString();
			qDebug() << "🌍 拦截到新窗口请求，正在当前页跳转:" << url;

			// 在当前视图加载新 URL -> 历史记录+1 -> 后退有效
			m_webView->load(request.requestedUrl());
		}
	);

	// 4. 信号连接
	// UI -> 逻辑
	connect(m_btnGo, &QPushButton::clicked, this, &WebWindow::onGoClicked);
	connect(m_lineUrl, &QLineEdit::returnPressed, this, &WebWindow::onGoClicked); // 回车也能跳转

	// 按钮 -> 浏览器动作 (直接连接到 WebEngine 的槽)
	connect(m_btnBack, &QPushButton::clicked, m_webView, &QWebEngineView::back);
	connect(m_btnRefresh, &QPushButton::clicked, m_webView, &QWebEngineView::reload);

	// 浏览器 -> UI 更新
	connect(m_webView, &QWebEngineView::urlChanged, this, &WebWindow::onUrlChanged);
	connect(m_webView, &QWebEngineView::titleChanged, this, &WebWindow::onTitleChanged);
	connect(m_webView, &QWebEngineView::loadProgress, this, &WebWindow::onLoadProgress);

	// 5. 默认加载一个页面
	loadUrl("https://www.baidu.com");
}

WebWindow::~WebWindow()
{
	// WebEngineView 通常会自动清理，但显式停止加载是个好习惯
	m_webView->stop();
}

void WebWindow::loadUrl(const QString& url)
{
	QString target = url;

	// 智能补全 http (如果用户没写)
	if (!target.startsWith("http://") && !target.startsWith("https://")) {
		target = "https://" + target;
	}

	m_webView->load(QUrl(target));
}

void WebWindow::onGoClicked()
{
	QString text = m_lineUrl->text().trimmed();
	if (!text.isEmpty()) {
		loadUrl(text);
	}
}

void WebWindow::onBackClicked()
{
	// 这个槽函数其实可以不写，直接 connect 到 m_webView->back 即可
	// 但如果想加额外逻辑（比如按钮置灰）可以在这里写
	m_webView->back();
}

void WebWindow::onRefreshClicked()
{
	m_webView->reload();
}

void WebWindow::onUrlChanged(const QUrl& url)
{
	// 当网页内部发生跳转时，更新地址栏显示
	m_lineUrl->setText(url.toString());
}

void WebWindow::onTitleChanged(const QString& title)
{
	// 更新窗口标题
	if (title.isEmpty()) {
		this->setWindowTitle("Qt 浏览器");
	}
	else {
		this->setWindowTitle(title + " - Qt 浏览器");
	}
}

void WebWindow::onLoadProgress(int progress)
{
	m_progressBar->setValue(progress);

	// 加载完了就隐藏进度条，没加载完显示
	if (progress >= 100) {
		m_progressBar->hide();
	}
	else {
		m_progressBar->show();
	} 
}