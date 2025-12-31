#pragma once

#include <QWidget> // ★ 改为 QWidget
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

// OpenCV
#include <opencv2/opencv.hpp>

// SeetaFace6 头文件
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceAntiSpoofing.h>
#include <seeta/Struct.h>

class FaceDemoWindow : public QWidget // ★ 改为 QWidget
{
	Q_OBJECT

public:
	FaceDemoWindow(QWidget* parent = nullptr);
	~FaceDemoWindow();

private:
	// --- 界面元素 ---
	QLabel* m_labelDisplay;   // 显示画面
	QLabel* m_labelStatus;    // 显示状态文字
	QVBoxLayout* m_mainLayout; // 主布局

	// --- 摄像头与定时器 ---
	cv::VideoCapture m_cap;
	QTimer* m_timer = nullptr;

	// --- SeetaFace 引擎指针 ---
	seeta::FaceDetector* m_detector = nullptr;
	seeta::FaceLandmarker* m_landmarker = nullptr;
	seeta::FaceAntiSpoofing* m_antiSpoofing = nullptr;

	// --- 核心函数 ---
	void setupUI();         // 布局界面
	void initSeetaFace();   // 加载模型
	void updateFrame();     // 刷新每一帧
};