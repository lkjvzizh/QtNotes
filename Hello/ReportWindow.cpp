#include "ReportWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

ReportWindow::ReportWindow(QWidget* parent)
    : QWidget(parent)
{
    this->setWindowTitle("报表打印测试工坊");
    this->resize(600, 500);

    m_reportHelper = new ReportHelper(this);

    // --- 1. 顶部控制栏 ---
    m_btnAddData = new QPushButton("➕ 生成 50 条模拟数据", this);
    connect(m_btnAddData, &QPushButton::clicked, this, &ReportWindow::onAddMockData);

    // --- 2. 中间数据预览区 ---
    m_listWidget = new QListWidget(this);
    // 先加几条默认的
    m_listWidget->addItems({ "测试项目 A", "测试项目 B", "测试项目 C" });

    // --- 3. 底部打印设置区 ---
    QGroupBox* optionGroup = new QGroupBox("打印选项", this);
    QHBoxLayout* optionLayout = new QHBoxLayout(optionGroup);

    m_checkSign = new QCheckBox("显示签字栏", this);
    m_checkSign->setChecked(true);

    m_checkDate = new QCheckBox("显示打印日期", this);
    m_checkDate->setChecked(true);

    m_checkColor = new QCheckBox("隔行变色表格", this);
    m_checkColor->setChecked(true);

    optionLayout->addWidget(m_checkSign);
    optionLayout->addWidget(m_checkDate);
    optionLayout->addWidget(m_checkColor);

    // 打印按钮
    m_btnPrint = new QPushButton("🖨️ 立即打印", this);
    m_btnPrint->setFixedHeight(40); // 大一点
    // 设个背景色让它显眼点 (可选)
    m_btnPrint->setStyleSheet("background-color: #0078d7; color: white; font-weight: bold; border-radius: 5px;");

    connect(m_btnPrint, &QPushButton::clicked, this, &ReportWindow::onPrintClicked);

    // --- 4. 组装总布局 ---
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("<h3>数据预览</h3>"));
    mainLayout->addWidget(m_btnAddData);
    mainLayout->addWidget(m_listWidget);
    mainLayout->addWidget(optionGroup);
    mainLayout->addWidget(m_btnPrint);
}

ReportWindow::~ReportWindow()
{
}

// 生成模拟数据，测试长表格
void ReportWindow::onAddMockData()
{
    static int batchCount = 1;
    for (int i = 1; i <= 50; ++i) {
        QString item = QString("第 %1 批数据 - 模拟记录项 #%2")
            .arg(batchCount).arg(i);
        m_listWidget->addItem(item);
    }
    // 滚动到底部
    m_listWidget->scrollToBottom();
    batchCount++;
}

// 核心：收集界面状态 -> 调用助手
void ReportWindow::onPrintClicked()
{
    // 1. 收集数据 (从 ListWidget 转成 QStringList)
    QStringList items;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        items << m_listWidget->item(i)->text();
    }

    if (items.isEmpty()) {
        // 如果列表是空的，加一行提示，防止打印空白
        items << "（无数据）";
    }

    // 2. 收集配置 (从 CheckBox 获取)
    ReportOptions options;
    options.showSignature = m_checkSign->isChecked();
    options.showDate = m_checkDate->isChecked();
    options.highlightRows = m_checkColor->isChecked();

    // 3. 执行打印
    // 标题可以动态设置，这里演示写死
    m_reportHelper->printListReport("系统数据导出报告", items, options);
}