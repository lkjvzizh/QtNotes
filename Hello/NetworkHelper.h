#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <functional> // 用于 std::function

class NetworkHelper : public QObject
{
    Q_OBJECT

public:
    // 定义回调函数类型：void func(bool success, const QByteArray& data)
    using RequestCallback = std::function<void(bool, const QByteArray&)>;

    // 单例模式获取实例
    static NetworkHelper* instance();

    // GET 请求封装
    void get(const QString& url, RequestCallback callback);

    // POST 请求封装 (假设发送 JSON)
    void post(const QString& url, const QByteArray& jsonData, RequestCallback callback);

private:
    explicit NetworkHelper(QObject* parent = nullptr);
    QNetworkAccessManager* m_manager;
};