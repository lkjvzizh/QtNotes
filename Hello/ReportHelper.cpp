#include "ReportHelper.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QDateTime>

ReportHelper::ReportHelper(QObject* parent) : QObject(parent) {}

void ReportHelper::printListReport(const QString& title, const QStringList& items, const ReportOptions& options)
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, nullptr);
    dialog.setWindowTitle("打印设置");

    if (dialog.exec() == QDialog::Accepted) {
        QString html = generateHtml(title, items, options);
        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&printer);
    }
}

QString ReportHelper::generateHtml(const QString& title, const QStringList& items, const ReportOptions& options)
{
    QString html = "<html><head><style>";
    html += "table { width: 100%; border-collapse: collapse; margin-top: 10px; }";
    html += "th, td { border: 1px solid #333; padding: 6px; text-align: left; }";
    html += "th { background-color: #ddd; }";

    // 隔行变色
    if (options.highlightRows) {
        html += "tr:nth-child(even) { background-color: #f9f9f9; }";
    }
    html += "</style></head><body>";

    // 标题
    html += QString("<h2 style='text-align:center;'>%1</h2>").arg(title);

    // 日期
    if (options.showDate) {
        html += QString("<p style='text-align:right;'>生成时间: %1</p>")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"));
    }

    // 表格
    html += "<table><tr><th>序号</th><th>文件名</th></tr>";
    for (int i = 0; i < items.size(); ++i) {
        html += "<tr>";
        html += QString("<td>%1</td>").arg(i + 1);
        html += QString("<td>%1</td>").arg(items[i]);
        html += "</tr>";
    }
    html += "</table>";

    // 签字栏 (根据配置决定显隐)
    if (options.showSignature) {
        html += "<div style='margin-top: 50px; text-align: right;'>";
        html += "<p>审核人签字：___________________</p>";
        html += "<p>日　　　期：___________________</p>";
        html += "</div>";
    }

    html += "</body></html>";
    return html;
}