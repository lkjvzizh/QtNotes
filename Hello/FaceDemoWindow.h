#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <opencv2/opencv.hpp>

// 引入 SeetaFace6 所有模块的头文件
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceAntiSpoofing.h>
#include <seeta/PoseEstimator.h>      // 姿态估计 (摇头)
#include <seeta/AgePredictor.h>       // 年龄
#include <seeta/GenderPredictor.h>    // 性别
#include <seeta/EyeStateDetector.h>   // 眼睛状态

class FaceDemoWindow : public QWidget
{
	Q_OBJECT

public:
	FaceDemoWindow(QWidget* parent = nullptr);
	~FaceDemoWindow();

private slots:
	void updateFrame(); // 定时刷新画面

private:
	void setupUI();      // 初始化界面
	void initSeetaFace(); // 初始化 AI 引擎

private:
	// UI 控件
	QLabel* m_labelDisplay;
	QLabel* m_labelStatus;
	QVBoxLayout* m_mainLayout;

	// 摄像头与定时器
	cv::VideoCapture m_cap;
	QTimer* m_timer;

	// --- SeetaFace 引擎指针 ---
	seeta::FaceDetector* m_detector = nullptr;
	seeta::FaceLandmarker* m_landmarker = nullptr;
	seeta::FaceAntiSpoofing* m_antiSpoofing = nullptr;

	// 新增功能的引擎
	seeta::PoseEstimator* m_poseEstimator = nullptr;
	seeta::AgePredictor* m_agePredictor = nullptr;
	seeta::GenderPredictor* m_genderPredictor = nullptr;
	seeta::EyeStateDetector* m_eyeStateDetector = nullptr;
};