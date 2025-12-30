#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QImage>
#include <QDebug>

// OpenCV
#include <opencv2/opencv.hpp>

// SeetaFace2
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/Struct.h>

class FaceTestWindow : public QWidget
{
	Q_OBJECT

public:
	explicit FaceTestWindow(QWidget* parent = nullptr);
	~FaceTestWindow();

private slots:
	void updateFrame(); // 定时器槽函数：读取并处理每一帧

private:
	// --- UI 控件 (纯代码布局) ---
	QVBoxLayout* mainLayout; // 垂直布局管理器
	QLabel* videoLabel;      // 用于显示视频画面
	QPushButton* btnBack;    // 一个返回或退出的按钮

	// --- 核心业务变量 ---
	QTimer* timer = nullptr;
	cv::VideoCapture cap;

	// SeetaFace 指针
	seeta::FaceDetector* detector;
	seeta::FaceLandmarker* landmarker;
};