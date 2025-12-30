#include "FaceTestWindow.h"

FaceTestWindow::FaceTestWindow(QWidget* parent)
    : QWidget(parent)
{
    // ===========================
    // 1. 纯代码 UI 布局
    // ===========================
    this->setWindowTitle("SeetaFace2 人脸识别完整版");
    this->resize(900, 700);

    mainLayout = new QVBoxLayout(this);

    // 视频显示区域
    videoLabel = new QLabel(this);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setStyleSheet("QLabel { background-color : black; }");
    videoLabel->setMinimumSize(640, 480);
    // 让 Label 扩张占据主要空间
    videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(videoLabel);

    // 注册按钮
    btnRegister = new QPushButton("注册当前人脸 (Register)", this);
    btnRegister->setFixedHeight(50);
    btnRegister->setStyleSheet("font-size: 18px; font-weight: bold; color: blue;");
    mainLayout->addWidget(btnRegister);

    // 退出按钮
    btnBack = new QPushButton("退出 (Exit)", this);
    btnBack->setFixedHeight(40);
    mainLayout->addWidget(btnBack);

    // 信号槽连接
    connect(btnRegister, &QPushButton::clicked, this, &FaceTestWindow::onBtnRegisterClicked);
    connect(btnBack, &QPushButton::clicked, [this]() { this->close(); });


    // ===========================
    // 2. 初始化 SeetaFace 模型
    // ===========================
    try {
        seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
        int id = 0;

        // 1. 检测模型
        seeta::ModelSetting FD_model("fd_2_00.dat", device, id);
        detector = new seeta::FaceDetector(FD_model);
        detector->set(seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80); // 最小人脸大小

        // 2. 关键点模型
        seeta::ModelSetting PD_model("pd_2_00_pts5.dat", device, id);
        landmarker = new seeta::FaceLandmarker(PD_model);

        // 3. 识别模型
        seeta::ModelSetting FR_model("fr_2_10.dat", device, id);
        recognizer = new seeta::FaceRecognizer(FR_model);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "错误", "模型加载失败！\n请检查 .dat 文件是否在 exe 目录下。");
        qDebug() << "模型加载报错：" << e.what();
        return; // 模型加载失败就不继续了
    }

    // ===========================
    // 3. 启动摄像头或视频
    // ===========================
    // 【注意】如果你没有摄像头，把下面改成 cap.open("你的视频.mp4");
    cap.open(0);

    if (!cap.isOpened()) {
        videoLabel->setText("无法打开摄像头/视频文件");
        return;
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FaceTestWindow::updateFrame);
    timer->start(30); // 约 33 FPS
}

FaceTestWindow::~FaceTestWindow()
{
    if (timer) { if (timer->isActive()) timer->stop(); delete timer; }
    if (cap.isOpened()) cap.release();

    if (detector) delete detector;
    if (landmarker) delete landmarker;
    if (recognizer) delete recognizer;
}

// --- 核心功能：注册人脸 ---
void FaceTestWindow::onBtnRegisterClicked()
{
    if (!cap.isOpened()) return;

    // 暂停画面
    timer->stop();

    cv::Mat frame;
    cap >> frame; // 抓取当前帧用于注册

    bool success = false;

    if (!frame.empty()) {
        SeetaImageData simg;
        simg.height = frame.rows;
        simg.width = frame.cols;
        simg.channels = frame.channels();
        simg.data = frame.data;

        // 1. 检测
        SeetaFaceInfoArray faces = detector->detect(simg);

        if (faces.size > 0) {
            // 只注册最大的一张脸 (索引0)
            SeetaFaceInfo& face = faces.data[0];

            // 2. 关键点
            SeetaPointF points[5];
            landmarker->mark(simg, face.pos, points);

            // 3. 提取特征
            // 申请 float[1024] 内存，并用 shared_ptr 托管，防止忘记 delete
            std::shared_ptr<float> feat(new float[1024], std::default_delete<float[]>());
            recognizer->Extract(simg, points, feat.get());

            // 4. 输入名字
            bool ok;
            QString text = QInputDialog::getText(this, "人脸注册",
                "请输入名字 (建议英文):", QLineEdit::Normal,
                "", &ok);
            if (ok && !text.isEmpty()) {
                FaceUser user;
                user.name = text;
                user.feature = feat;
                faceParams.push_back(user); // 存入数据库
                QMessageBox::information(this, "成功", "用户 [" + text + "] 注册成功！");
                success = true;
            }
        }
        else {
            QMessageBox::warning(this, "失败", "未检测到人脸，请正对摄像头！");
        }
    }

    // 恢复画面
    timer->start();
}

// --- 核心循环：检测 + 识别 + 显示 ---
void FaceTestWindow::updateFrame()
{
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;

    // 转换数据给 SeetaFace
    SeetaImageData simg;
    simg.height = frame.rows;
    simg.width = frame.cols;
    simg.channels = frame.channels();
    simg.data = frame.data;

    // 1. 人脸检测
    SeetaFaceInfoArray faces = detector->detect(simg);

    for (int i = 0; i < faces.size; i++) {
        SeetaFaceInfo& face = faces.data[i];

        // 2. 关键点定位 (为了画点，也为了识别对齐)
        SeetaPointF points[5];
        landmarker->mark(simg, face.pos, points);

        // 3. 特征提取 & 身份识别
        float current_feat[1024];
        recognizer->Extract(simg, points, current_feat);

        QString targetName = "Unknown";
        float max_sim = 0.0f;

        // 遍历数据库寻找最相似的人
        for (const auto& user : faceParams) {
            float sim = recognizer->CalculateSimilarity(current_feat, user.feature.get());
            if (sim > max_sim) {
                max_sim = sim;
                if (sim > 0.65f) { // 相似度阈值 (推荐 0.6 ~ 0.7)
                    targetName = user.name;
                }
            }
        }

        // 4. 绘制结果
        cv::Rect faceRect(face.pos.x, face.pos.y, face.pos.width, face.pos.height);

        // 认识的人用红框，不认识用绿框
        cv::Scalar color = (targetName == "Unknown") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
        cv::rectangle(frame, faceRect, color, 2);

        // 绘制关键点
        for (auto& p : points) {
            cv::circle(frame, cv::Point(p.x, p.y), 3, cv::Scalar(0, 255, 255), -1);
        }

        // 显示名字 (只支持英文)
        std::string textShow = targetName.toStdString() + " (" + std::to_string(max_sim).substr(0, 4) + ")";
        cv::putText(frame, textShow, cv::Point(faceRect.x, faceRect.y - 10),
            cv::FONT_HERSHEY_SIMPLEX, 0.8, color, 2);
    }

    // 5. 显示到 Qt 界面 (BGR -> RGB)
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    // 构造 QImage
    // 注意：frame.step 是每一行的字节数，必须填，否则倾斜
    QImage qimg((const unsigned char*)frame.data,
        frame.cols, frame.rows,
        frame.step,
        QImage::Format_RGB888);

    // 缩放并显示
    videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}