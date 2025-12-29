#include "Hello.h"
#include "SerialWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QLocale>
#include "NetworkHelperWindow.h"
#include "VideoWindow.h"
#include "ReportWindow.h"
#include "UsbRawWindow.h"
#include "WebWindow.h"
#include "BrowserWindow.h"

#include "wsServerWindow.h" // 名字变了
#include "wsClientWindow.h"   // 保持不变

#include "BluetoothWindow.h"

int main(int argc, char *argv[])
{

	// --- 🚀 性能优化核心配置 ---

	// 1. 强制使用桌面级 OpenGL (利用独立显卡)
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	// 2. 开启 OpenGL 上下文共享 (WebEngine 必须)
	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    // 加载翻译文件
    QTranslator translator;
    QString locale = QLocale::system().name(); // 获取系统语言，如 "zh_CN" 或 "en_US"
    
    // 尝试加载对应语言的翻译文件
    //if (translator.load(":/translations/Hello_" + locale + ".qm")) {
    if (translator.load(":/translations/Hello_en_US.qm")) {
        app.installTranslator(&translator);
    }

    //1.Hello窗口 
    //Hello window;
    //window.show();

    // 2. 创建并显示 串口 窗口
    //SerialWindow serialWin;
   // serialWin.show();

    //3.网络请求
    //NetworkHelperWindow w;
    //w.show();

    //4.视频播放
    //QString moviePath = "./LEKA/video.mp4";
    //VideoWindow player;
    //player.show();
    //player.setVideoPath(moviePath);
    
    //5.打印窗口
    //ReportWindow reportWindow;
    //reportWindow.show();

    //6.usb窗口
    //UsbRawWindow usbRawWindow;
    //usbRawWindow.show();

    //7.WebEngine
    //WebWindow webWindow;
    //webWindow.show();

    //8.与html交互
	//BrowserWindow w;
	//w.showMaximized();

    //9.websocket
	// 1. 启动服务器
    /*
	wsServerWindow server;
	server.move(100, 100);
	server.show();

	// 2. 启动客户端 A
	wsClientWindow client1;
	client1.setWindowTitle("Client A");
	client1.move(550, 100);
	client1.show();

	// 3. 启动客户端 B
	wsClientWindow client2;
	client2.setWindowTitle("Client B");
	client2.move(550, 450);
	client2.show();
    */

    //10.BluetoothWindow
    BluetoothWindow w;
    w.show();

    return app.exec();
}