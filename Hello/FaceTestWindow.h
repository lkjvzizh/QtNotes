#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QImage>
#include <QDebug>
#include <QInputDialog> // 用于输入名字
#include <QMessageBox>  // 用于提示信息

// OpenCV
#include <opencv2/opencv.hpp>

// SeetaFace2
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/FaceRecognizer.h>
#include <seeta/Struct.h>

// 定义一个结构体，用来存录入的人脸信息
struct FaceUser {
	QString name;                   // 名字
	std::shared_ptr<float> feature; // 特征值 (1024维数组)，使用智能指针管理内存
};

class FaceTestWindow : public QWidget
{
	Q_OBJECT

public:
	explicit FaceTestWindow(QWidget* parent = nullptr);
	~FaceTestWindow();

private slots:
	void updateFrame();          // 每一帧的处理逻辑
	void onBtnRegisterClicked(); // 注册按钮点击事件

private:
	// --- UI 控件 ---
	QVBoxLayout* mainLayout;
	QLabel* videoLabel;
	QPushButton* btnRegister;
	QPushButton* btnBack;

	// --- 核心变量 ---
	QTimer* timer = nullptr;
	cv::VideoCapture cap;

	// SeetaFace 引擎指针
	seeta::FaceDetector* detector = nullptr;
	seeta::FaceLandmarker* landmarker = nullptr;
	seeta::FaceRecognizer* recognizer = nullptr;

	// --- 简易人脸数据库 ---
	std::vector<FaceUser> faceParams;
};