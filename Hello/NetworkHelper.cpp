#include "NetworkHelper.h"
#include <QNetworkRequest>

// 单例实现
NetworkHelper* NetworkHelper::instance()
{
    static NetworkHelper instance;
    return &instance;
}

// 构造函数：初始化 QNAM
NetworkHelper::NetworkHelper(QObject* parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

// 封装 GET
void NetworkHelper::get(const QString& url, RequestCallback callback)
{
    QNetworkRequest request((QUrl(url)));
    // 可以设置通用的 Header，比如 User-Agent
    request.setHeader(QNetworkRequest::UserAgentHeader, "MyQtApp");

    // 发送请求
    QNetworkReply* reply = m_manager->get(request);

    // 【关键点】使用 Lambda 表达式直接处理结果
    // [=] 表示按值捕获外部变量（主要为了捕获 callback 和 reply）
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 成功：回调传回 true 和数据
            callback(true, reply->readAll());
        }
        else {
            // 失败：回调传回 false 和空数据 (或者你可以传错误信息)
            callback(false, QByteArray());
        }
        // 自动释放内存，防止内存泄漏
        reply->deleteLater();
        });
}

// 封装 POST
void NetworkHelper::post(const QString& url, const QByteArray& jsonData, RequestCallback callback)
{
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // 默认发 JSON

    QNetworkReply* reply = m_manager->post(request, jsonData);

    // 处理逻辑和 GET 一样
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            callback(true, reply->readAll());
        }
        else {
            callback(false, QByteArray());
        }
        reply->deleteLater();
        });
}