#pragma once
#include <QWidget>
#include <QListWidget>  // 用来展示要打印的数据
#include <QCheckBox>
#include <QPushButton>
#include "ReportHelper.h" // 复用之前的打印助手

class ReportWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget* parent = nullptr);
    ~ReportWindow();

private slots:
    void onAddMockData();   // 添加模拟数据
    void onPrintClicked();  // 点击打印

private:
    // UI 控件
    QListWidget* m_listWidget;   // 显示数据的列表
    QCheckBox* m_checkSign;    // 开关：签字栏
    QCheckBox* m_checkDate;    // 开关：日期
    QCheckBox* m_checkColor;   // 开关：隔行变色
    QPushButton* m_btnAddData;   // 按钮：生成测试数据
    QPushButton* m_btnPrint;     // 按钮：打印

    // 打印助手
    ReportHelper* m_reportHelper;
};