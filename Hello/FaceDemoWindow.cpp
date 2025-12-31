#include "FaceDemoWindow.h"
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <string>

// 辅助函数：简化路径拼接，转为 c_str() 避免 string 跨 DLL 崩溃
std::string getModelFile(const std::string& dir, const std::string& fileName) {
	return dir + fileName;
}

FaceDemoWindow::FaceDemoWindow(QWidget* parent)
	: QWidget(parent)
{
	setupUI();     // 1. 画界面
	initSeetaFace(); // 2. 加载模型

	// 3. 打开摄像头
	if (m_cap.open(0)) {
		m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
		m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

		m_timer = new QTimer(this);
		connect(m_timer, &QTimer::timeout, this, &FaceDemoWindow::updateFrame);
		m_timer->start(30); // 30ms 刷新一次
	}
	else {
		m_labelStatus->setText("错误: 无法打开摄像头！");
		QMessageBox::critical(this, "Error", "无法连接摄像头");
	}
}

FaceDemoWindow::~FaceDemoWindow()
{
	// 释放所有引擎，防止内存泄漏
	if (m_detector) delete m_detector;
	if (m_landmarker) delete m_landmarker;
	if (m_antiSpoofing) delete m_antiSpoofing;

	if (m_poseEstimator) delete m_poseEstimator;
	if (m_agePredictor) delete m_agePredictor;
	if (m_genderPredictor) delete m_genderPredictor;
	if (m_eyeStateDetector) delete m_eyeStateDetector;

	if (m_cap.isOpened()) m_cap.release();
}

void FaceDemoWindow::setupUI()
{
	this->setWindowTitle("SeetaFace6 全功能演示 (Release)");
	this->resize(800, 600);

	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(10, 10, 10, 10);

	// 显示视频的 Label
	m_labelDisplay = new QLabel(this);
	m_labelDisplay->setAlignment(Qt::AlignCenter);
	m_labelDisplay->setStyleSheet("background-color: black; border: 2px solid gray;");
	// 关键：忽略尺寸策略，防止窗口自动变大
	m_labelDisplay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	// 状态栏
	m_labelStatus = new QLabel("正在初始化模型...", this);
	m_labelStatus->setMaximumHeight(30);

	m_mainLayout->addWidget(m_labelDisplay);
	m_mainLayout->addWidget(m_labelStatus);
	this->setLayout(m_mainLayout);
}

void FaceDemoWindow::initSeetaFace()
{
	try {
		seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
		int id = 0;

		// 动态获取 exe 所在目录
		QString exePath = QCoreApplication::applicationDirPath();
		std::string modelDir = (exePath + "/SeetaFace6_Models/").toStdString();

		qDebug() << "Model Dir:" << QString::fromStdString(modelDir);

		// ★★★ 这里使用 c_str() 传递路径，确保 Release 模式稳定 ★★★

		// 1. 基础：检测与关键点
		m_detector = new seeta::FaceDetector(
			seeta::ModelSetting(getModelFile(modelDir, "face_detector.csta").c_str(), device, id));
		m_detector->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80);

		m_landmarker = new seeta::FaceLandmarker(
			seeta::ModelSetting(getModelFile(modelDir, "face_landmarker_pts5.csta").c_str(), device, id));

		// 2. 活体检测
		m_antiSpoofing = new seeta::FaceAntiSpoofing(
			seeta::ModelSetting(getModelFile(modelDir, "fas_first.csta").c_str(), device, id));

		// 3. 姿态估计 (摇头点头)
		m_poseEstimator = new seeta::PoseEstimator(
			seeta::ModelSetting(getModelFile(modelDir, "pose_estimation.csta").c_str(), device, id));

		// 4. 年龄预测
		m_agePredictor = new seeta::AgePredictor(
			seeta::ModelSetting(getModelFile(modelDir, "age_predictor.csta").c_str(), device, id));

		// 5. 性别预测
		m_genderPredictor = new seeta::GenderPredictor(
			seeta::ModelSetting(getModelFile(modelDir, "gender_predictor.csta").c_str(), device, id));

		// 6. 眼睛状态 (眨眼)
		m_eyeStateDetector = new seeta::EyeStateDetector(
			seeta::ModelSetting(getModelFile(modelDir, "eye_state.csta").c_str(), device, id));

		m_labelStatus->setText("状态: 全功能引擎加载完毕！");
	}
	catch (const std::exception& e) {
		m_labelStatus->setText("状态: 模型加载失败");
		QMessageBox::critical(this, "Error", QString("模型缺失或损坏:\n%1").arg(e.what()));
	}
}

void FaceDemoWindow::updateFrame()
{
	cv::Mat frame;
	m_cap >> frame;
	if (frame.empty()) return;

	cv::flip(frame, frame, 1); // 镜像

	// 转换为 Seeta 图像格式
	SeetaImageData simg;
	simg.height = frame.rows;
	simg.width = frame.cols;
	simg.channels = frame.channels();
	simg.data = frame.data;

	// 1. 人脸检测
	auto faces = m_detector->detect(simg);

	for (int i = 0; i < faces.size; i++) {
		seeta::Rect faceRect = faces.data[i].pos;

		// 2. 关键点 (所有后续功能的基础)
		std::vector<SeetaPointF> points(5);
		m_landmarker->mark(simg, faceRect, points.data());

		// --- A. 活体检测 ---
		auto status = m_antiSpoofing->Predict(simg, faceRect, points.data());
		std::string textFAS = (status == seeta::FaceAntiSpoofing::REAL) ? "REAL" : "FAKE";
		cv::Scalar color = (status == seeta::FaceAntiSpoofing::REAL) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
		if (status == seeta::FaceAntiSpoofing::FUZZY) {
			textFAS = "FUZZY";
			color = cv::Scalar(0, 255, 255);
		}

		// --- B. 年龄 & 性别 ---
		int age = 0;
		m_agePredictor->PredictAgeWithCrop(simg, points.data(), age);

		seeta::GenderPredictor::GENDER genderEnum;
		m_genderPredictor->PredictGenderWithCrop(simg, points.data(), genderEnum);
		std::string textGender = (genderEnum == seeta::GenderPredictor::MALE) ? "Male" : "Female";

		// --- C. 姿态估计 (点头摇头) ---
		float pitch = 0, yaw = 0, roll = 0;
		m_poseEstimator->Estimate(simg, faceRect, &yaw, &pitch, &roll);

		std::string textPose = "Normal";
		// 阈值设为 15度，超过即认为有动作
		if (abs(pitch) > 15) textPose = (pitch > 0) ? "Nodding Up" : "Nodding Down";
		else if (abs(yaw) > 15) textPose = (yaw > 0) ? "Turn Left" : "Turn Right";
		else if (abs(roll) > 15) textPose = "Rolling";

		// --- D. 眼睛状态 (眨眼检测) ---
		seeta::EyeStateDetector::EYE_STATE leftState, rightState;
		m_eyeStateDetector->Detect(simg, points.data(), leftState, rightState);

		std::string eyeL = (leftState == seeta::EyeStateDetector::EYE_CLOSE) ? "CLOSE" : "OPEN";
		std::string eyeR = (rightState == seeta::EyeStateDetector::EYE_CLOSE) ? "CLOSE" : "OPEN";
		std::string textEye = "L:" + eyeL + " R:" + eyeR;


		// --- 绘制结果 ---
		cv::rectangle(frame, cv::Rect(faceRect.x, faceRect.y, faceRect.width, faceRect.height), color, 2);

		// 在框的右侧显示多行信息
		int x = faceRect.x + faceRect.width + 5;
		int y = faceRect.y + 20;
		int step = 25;

		// 活体
		cv::putText(frame, textFAS, cv::Point(x, y), 1, 1.2, color, 2);
		// 年龄性别
		cv::putText(frame, textGender + "," + std::to_string(age), cv::Point(x, y + step), 1, 0.8, cv::Scalar(255, 255, 0), 1);
		// 姿态
		cv::putText(frame, textPose, cv::Point(x, y + step * 2), 1, 0.8, cv::Scalar(0, 255, 255), 1);
		// 眼睛
		cv::putText(frame, textEye, cv::Point(x, y + step * 3), 1, 0.8, cv::Scalar(255, 0, 255), 1);
	}

	// 转为 Qt 格式显示
	cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
	QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
	m_labelDisplay->setPixmap(QPixmap::fromImage(qimg).scaled(m_labelDisplay->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}