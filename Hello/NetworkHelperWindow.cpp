#include "NetworkHelperWindow.h"
#include "NetworkHelper.h" // 引入刚才封装的工具类
#include <QDateTime>
#include "ApiResponse.h"


NetworkHelperWindow::NetworkHelperWindow(QWidget* parent)
    : QWidget(parent)
{
    // --- 1. 界面布局初始化 ---
    this->setWindowTitle("网络请求测试工具");
    this->resize(600, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // URL 输入行
    QHBoxLayout* topLayout = new QHBoxLayout();
    m_urlEdit = new QLineEdit("https://www.baidu.com/", this);
    m_btnGet = new QPushButton("发送 GET", this);
    m_btnPost = new QPushButton("发送 POST", this);

    topLayout->addWidget(m_urlEdit);
    topLayout->addWidget(m_btnGet);
    topLayout->addWidget(m_btnPost);

    // 日志显示区
    m_logArea = new QTextEdit(this);
    m_logArea->setReadOnly(true); // 只读

    // 添加到主布局
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_logArea);


    // --- 2. 绑定 GET 按钮逻辑 ---
    connect(m_btnGet, &QPushButton::clicked, this, [=]() {
        QString url = m_urlEdit->text();
        log(">>> 正在发送 GET 请求: " + url);

        // 调用我们封装好的 NetworkHelper
        NetworkHelper::instance()->get(url, [=](bool success, const QByteArray& data) {
            if (success) {
                log("<<< [成功] 服务器返回:\n" + QString(data));
            }
            else {
                log("<<< [失败] 请求出错！");
            }
            });

        });


    // --- 3. 绑定 POST 按钮逻辑 ---
    connect(m_btnPost, &QPushButton::clicked, this, [=]() {
        // 为了演示，我们将 URL 临时改为 post 接口
        QString url = "https://ytj.hnljdz.com/ytj/api/check_data";

        // 模拟一个 JSON 数据
        QByteArray jsonData = "{\"username\": \"qt_tester\", \"msg\": \"Hello Server!\"}";

        log(">>> 正在发送 POST 请求: " + url);
        log(">>> 发送数据: " + jsonData);

        // 调用 NetworkHelper
        NetworkHelper::instance()->post(url, jsonData, [=](bool success, const QByteArray& data) {
            if (success) {
                log("<<< [成功] 服务器返回:\n" + QString(data));
            }
            else {
                log("<<< [失败] 请求出错！");
            }

            // 2. 一行代码解析
            ApiResponse res = ApiResponse::fromJson(QString(data).toUtf8());

            // 3. 使用数据
            log("<<< 状态码:"+res.code);
            log("<<< 服务端消息:" + res.msg);

            });
        });
}

void NetworkHelperWindow::log(const QString& text)
{
    // 获取当前时间
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_logArea->append(QString("[%1] %2").arg(time, text));
}