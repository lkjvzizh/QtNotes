#pragma once
#include <QObject>
#include <QStringList>

// 打印选项结构体
struct ReportOptions {
    bool showSignature = true;
    bool showDate = true;
    bool highlightRows = true;
};

class ReportHelper : public QObject
{
    Q_OBJECT
public:
    explicit ReportHelper(QObject* parent = nullptr);
    void printListReport(const QString& title, const QStringList& items, const ReportOptions& options);

private:
    QString generateHtml(const QString& title, const QStringList& items, const ReportOptions& options);
};