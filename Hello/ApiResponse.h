#ifndef APIRESPONSE_H
#define APIRESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>


// AI生成的{"code":"0","msg":"数据格式错误,无设备编号"}
class ApiResponse {
public:
    // 对应 JSON 中的字段
    // 注意：你的 JSON 中 "code" 是字符串 "0"，所以这里用 QString
    QString code;
    QString msg;

    ApiResponse() {}

    // ─── 反序列化 (JSON -> Object) ───────────────────────────
    static ApiResponse fromJson(const QByteArray& jsonBytes) {
        ApiResponse response;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonBytes, &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            qWarning() << "JSON Parse Error:" << parseError.errorString();
            return response; // 返回空对象或处理错误
        }

        QJsonObject obj = doc.object();

        // 提取数据 (使用 value() 方法比较安全，带默认值)
        response.code = obj.value("code").toString();
        response.msg = obj.value("msg").toString();

        return response;
    }

    // ─── 辅助判断方法 ────────────────────────────────────────
    // 判断业务逻辑是否成功（假设 code 为 "1" 或 "200" 是成功，这里根据你的实际情况修改）
    bool isSuccess() const {
        // 假设 "0" 代表失败，非 "0" 代表成功？或者反过来？
        // 通常 code 0 可能代表成功，也可能代表失败，看你的接口定义。
        // 这里仅作演示：
        return code == "200";
    }
};

#endif // APIRESPONSE_H