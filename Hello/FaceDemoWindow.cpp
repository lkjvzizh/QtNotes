#include "FaceDemoWindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

FaceDemoWindow::FaceDemoWindow(QWidget* parent)
	: QWidget(parent) // ★ 基类构造改为 QWidget
{
	// 1. 先把界面画出来
	setupUI();

	// 2. 加载 SeetaFace 算法
	initSeetaFace();

	// 3. 打开摄像头
	if (m_cap.open(0)) {
		// 设置一下分辨率，太高会跑不动
		m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
		m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

		m_timer = new QTimer(this);
		connect(m_timer, &QTimer::timeout, this, &FaceDemoWindow::updateFrame);
		m_timer->start(30); // 约 33 FPS
	}
	else {
		m_labelStatus->setText("错误: 无法打开摄像头！");
		QMessageBox::critical(this, "摄像头错误", "无法连接摄像头，请检查设备。");
	}
}

FaceDemoWindow::~FaceDemoWindow()
{
	// 退出时释放指针，防止内存泄漏
	if (m_detector) delete m_detector;
	if (m_landmarker) delete m_landmarker;
	if (m_antiSpoofing) delete m_antiSpoofing;
	if (m_cap.isOpened()) m_cap.release();
}

void FaceDemoWindow::setupUI()
{
	this->setWindowTitle("SeetaFace6 Live Demo (QWidget)");
	this->resize(800, 600);

	// ★★★ QWidget 布局非常简单 ★★★

	// 1. 创建布局管理器
	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(10, 10, 10, 10); // 设置一点边距
	m_mainLayout->setSpacing(10); // 控件之间的间距

	// 2. 创建显示画面的 Label
	m_labelDisplay = new QLabel(this);
	m_labelDisplay->setAlignment(Qt::AlignCenter);
	// 给个黑色背景和边框，看起来更像播放器
	m_labelDisplay->setStyleSheet("background-color: black; border: 2px solid #555; border-radius: 5px;");
	//m_labelDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 让画面尽可能撑满
	m_labelDisplay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	// 3. 创建状态栏 Label
	m_labelStatus = new QLabel("系统状态: 正在初始化...", this);
	m_labelStatus->setStyleSheet("font-size: 14px; font-weight: bold; color: #333;");
	m_labelStatus->setMaximumHeight(30);

	// 4. 将控件加入布局
	m_mainLayout->addWidget(m_labelDisplay);
	m_mainLayout->addWidget(m_labelStatus);

	// 5. 应用布局 (QWidget 专属写法)
	this->setLayout(m_mainLayout);
}

void FaceDemoWindow::initSeetaFace()
{
	try {
		seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
		int id = 0;

		// 模型路径 (确保 SeetaFace6_Models 文件夹在 exe 旁边)
		//std::string modelPath = "SeetaFace6_Models/";

		QString exePath = QCoreApplication::applicationDirPath();
		std::string modelPath = (exePath + "/SeetaFace6_Models/").toStdString();
		qDebug() << "Model Path:" << QString::fromStdString(modelPath);

		// 1. 初始化检测器
		seeta::ModelSetting detectorModel(modelPath + "face_detector.csta", device, id);
		m_detector = new seeta::FaceDetector(detectorModel);
		m_detector->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80);

		// 2. 初始化关键点 (配合活体检测)
		seeta::ModelSetting landmarkerModel(modelPath + "face_landmarker_pts5.csta", device, id);
		m_landmarker = new seeta::FaceLandmarker(landmarkerModel);

		// 3. 初始化活体检测
// 		seeta::ModelSetting fasModel(modelPath + "fas_first.csta", device, id);
// 		m_antiSpoofing = new seeta::FaceAntiSpoofing(fasModel);

		// 1. 手动拼接好路径
		std::string pathStr = modelPath + "fas_first.csta";

		// 2. ★★★ 关键修改：转成 const char* 传进去 ★★★
		// 这样可以避免直接构造临时的 std::string 对象跨越 DLL 边界
		seeta::ModelSetting fasModel(pathStr.c_str(), device, id);

		m_antiSpoofing = new seeta::FaceAntiSpoofing(fasModel);

		m_labelStatus->setText("系统状态: 引擎加载成功！请看向摄像头。");
		qDebug() << "SeetaFace6 Engines Loaded Success.";

	}
	catch (const std::exception& e) {
		QString err = QString("模型加载失败: %1").arg(e.what());
		m_labelStatus->setText("系统状态: 初始化失败");
		QMessageBox::critical(this, "严重错误",
			"无法加载模型文件！\n请确认 'SeetaFace6_Models' 文件夹在 exe 运行目录下。\n\n" + err);
	}
}

void FaceDemoWindow::updateFrame()
{
	cv::Mat frame;
	m_cap >> frame;
	if (frame.empty()) return;

	// 镜像翻转，体验更好
	cv::flip(frame, frame, 1);

	// 开始识别
	if (m_detector && m_landmarker && m_antiSpoofing) {

		SeetaImageData simg;
		simg.height = frame.rows;
		simg.width = frame.cols;
		simg.channels = frame.channels();
		simg.data = frame.data;

		// 1. 检测人脸
		auto faces = m_detector->detect(simg);

		for (int i = 0; i < faces.size; i++) {
			auto& face = faces.data[i];
			seeta::Rect pos = face.pos;

			// 2. 关键点定位
			std::vector<SeetaPointF> points(5);
			m_landmarker->mark(simg, pos, points.data());

			// 3. ★ 活体检测核心调用 ★
			auto status = m_antiSpoofing->Predict(simg, pos, points.data());

			// 4. 根据结果画框
			cv::Scalar color(0, 0, 255); // 默认红色 (假)
			std::string text = "FAKE";

			if (status == seeta::FaceAntiSpoofing::REAL) {
				color = cv::Scalar(0, 255, 0); // 绿色 (真)
				text = "REAL";
			}
			else if (status == seeta::FaceAntiSpoofing::FUZZY) {
				color = cv::Scalar(0, 255, 255); // 黄色 (模糊)
				text = "FUZZY";
			}
			else if (status == seeta::FaceAntiSpoofing::DETECTING) {
				color = cv::Scalar(255, 0, 0); // 蓝色 (检测中)
				text = "DETECTING";
			}

			// 绘制矩形框
			cv::rectangle(frame, cv::Rect(pos.x, pos.y, pos.width, pos.height), color, 2);
			// 绘制文字
			cv::putText(frame, text, cv::Point(pos.x, pos.y - 10),
				cv::FONT_HERSHEY_SIMPLEX, 1.0, color, 2);
		}
	}

	// 显示到界面
	cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
	QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

	// 缩放显示，保持比例
	m_labelDisplay->setPixmap(QPixmap::fromImage(qimg).scaled(
		m_labelDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}