#include "FaceTestWindow.h"

FaceTestWindow::FaceTestWindow(QWidget* parent)
	: QWidget(parent)
{
	// ===========================
	// 1. 纯代码 UI 布局 (Layout)
	// ===========================
	this->setWindowTitle("纯代码布局 - SeetaFace检测测试");
	this->resize(800, 600); // 设置初始窗口大小

	// 创建布局管理器
	mainLayout = new QVBoxLayout(this);

	// 创建视频显示区域 (Label)
	videoLabel = new QLabel(this);
	videoLabel->setAlignment(Qt::AlignCenter); // 居中显示
	videoLabel->setStyleSheet("QLabel { background-color : black; }"); // 黑色背景
	videoLabel->setMinimumSize(640, 480); // 最小尺寸
	mainLayout->addWidget(videoLabel); // 放入布局

	// 创建一个按钮 (示例)
	btnBack = new QPushButton("关闭摄像头 / 退出", this);
	btnBack->setFixedHeight(40); // 按钮高度
	mainLayout->addWidget(btnBack); // 放入布局

	// 按钮点击事件
	connect(btnBack, &QPushButton::clicked, [this]() {
		this->close(); // 点击关闭窗口
		});


	// ===========================
	// 2. 初始化算法模型
	// ===========================
	try {
		seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
		int id = 0;

		// 加载检测模型 (确保 fd_2_00.dat 在 exe 旁边)
		seeta::ModelSetting FD_model("fd_2_00.dat", device, id);
		detector = new seeta::FaceDetector(FD_model);
		detector->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80);

		// 加载关键点模型 (确保 pd_2_00_pts5.dat 在 exe 旁边)
		seeta::ModelSetting PD_model("pd_2_00_pts5.dat", device, id);
		landmarker = new seeta::FaceLandmarker(PD_model);
	}
	catch (...) {
		qDebug() << "错误：模型加载失败！请检查 .dat 文件路径。";
		// 如果这里崩溃，记得把那三个 .dat 文件复制到 exe 目录
	}


	// ===========================
	// 3. 启动摄像头与定时器
	// ===========================
	cap.open(0);
	if (!cap.isOpened()) {
		videoLabel->setText("无法打开摄像头");
	}
	else {
		timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &FaceTestWindow::updateFrame);
		timer->start(30); // 33 FPS
	}
}

FaceTestWindow::~FaceTestWindow()
{
	// 释放资源
	if (timer) {
		if (timer->isActive()) {
			timer->stop();
		}
		// 不需要 delete timer，因为它是 QObject 且指定了 parent (this)，Qt 会自动回收
	}
	// 2. 释放摄像头资源
	if (cap.isOpened()) {
		cap.release();
	}

	if (detector) delete detector;
	if (landmarker) delete landmarker;
}

void FaceTestWindow::updateFrame()
{
	cv::Mat frame;
	cap >> frame;
	if (frame.empty()) return;

	// --- A. SeetaFace 检测逻辑 ---
	// 【修改点1】去掉 seeta::，直接用 SeetaImageData
	SeetaImageData simg;
	simg.height = frame.rows;
	simg.width = frame.cols;
	simg.channels = frame.channels();
	simg.data = frame.data;

	// 检测人脸
	// 【修改点2】返回值类型通常是 SeetaFaceInfoArray
	SeetaFaceInfoArray faces = detector->detect(simg);

	for (int i = 0; i < faces.size; i++) {
		// 【修改点3】使用 SeetaFaceInfo，去掉 seeta::
		SeetaFaceInfo& face = faces.data[i];

		// 1. 画框 (OpenCV)
		cv::Rect faceRect(face.pos.x, face.pos.y, face.pos.width, face.pos.height);
		cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);

		// 2. 画关键点 (5点)
		// 【修改点4】类型是 SeetaPointF，不是 Point5
		SeetaPointF points[5];
		landmarker->mark(simg, face.pos, points);

		for (auto& p : points) {
			cv::circle(frame, cv::Point(p.x, p.y), 3, cv::Scalar(0, 0, 255), -1);
		}
	}

	// --- B. 显示逻辑 ---
	cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
	QImage qimg((const unsigned char*)frame.data,
		frame.cols, frame.rows,
		frame.step,
		QImage::Format_RGB888);

	QPixmap pixmap = QPixmap::fromImage(qimg);
	videoLabel->setPixmap(pixmap.scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}