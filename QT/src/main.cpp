#include "mainwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFont>
#include <QPalette>
#include <QMessageBox>
#include <QTextCodec>
#include <fcntl.h>      // 用于文件控制，如 open
#include <unistd.h>     // 提供对 POSIX 操作系统 API 的访问，如 close
#include <sys/mman.h>   // 用于内存映射 (mmap, munmap)
#include <stdlib.h>     // 用于 system() 函数
#include <QProcess>
#include <QObject>

// GPIO 寄存器相关的物理地址偏移量
#define DEV_NAME "/dev/mem"         // 物理内存设备文件
#define GPE0CON_OFFSET 0x00E0       // GPE0 控制寄存器偏移
#define GPE1CON_OFFSET 0x0100       // GPE1 控制寄存器偏移
#define GPE0DAT_OFFSET (GPE0CON_OFFSET + 0x04) // GPE0 数据寄存器偏移
#define GPE1DAT_OFFSET (GPE1CON_OFFSET + 0x04) // GPE1 数据寄存器偏移

// MainWindow 类的构造函数
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_memFd(-1),          // 初始化内存文件描述符为-1 (无效)
      m_addrStart(NULL),    // 初始化内存映射起始地址为 NULL
      m_gpe0Data(NULL),     // 初始化 GPE0 数据寄存器指针为 NULL
      m_gpe1Data(NULL)      // 初始化 GPE1 数据寄存器指针为 NULL
{
    // 设置窗口标题
    setWindowTitle("LED控制面板");
    
    // 设置窗口背景色为白色
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);
    
    // 初始化LED状态数组，所有灯默认为0 (关闭)
    for (int i = 0; i < 8; i++) {
        m_ledValues[i] = 0;
    }
    
    // 调用函数设置用户界面
    setupUi();
    // 调用函数初始化硬件设备
    initDevices();
}

// MainWindow 类的析构函数
MainWindow::~MainWindow()
{
    // 在对象销毁时，确保所有设备被安全关闭
    closeAllDevices();
}

// LED 0-7 的状态切换槽函数，这些辅助函数最终都调用 onLedToggled
void MainWindow::onLed0Toggled(bool state) { onLedToggled(0, state); }
void MainWindow::onLed1Toggled(bool state) { onLedToggled(1, state); }
void MainWindow::onLed2Toggled(bool state) { onLedToggled(2, state); }
void MainWindow::onLed3Toggled(bool state) { onLedToggled(3, state); }
void MainWindow::onLed4Toggled(bool state) { onLedToggled(4, state); }
void MainWindow::onLed5Toggled(bool state) { onLedToggled(5, state); }
void MainWindow::onLed6Toggled(bool state) { onLedToggled(6, state); }
void MainWindow::onLed7Toggled(bool state) { onLedToggled(7, state); }

// 核心的 LED 状态处理函数
void MainWindow::onLedToggled(int ledNumber, bool state)
{
    // 检查设备是否已成功初始化
    if (m_memFd < 0 || m_addrStart == NULL) {
        QMessageBox::critical(this, "错误", "GPIO设备未正确初始化!");
        return;
    }

    // 更新内部记录的LED状态
    m_ledValues[ledNumber] = state ? 1 : 0;
    
    // 根据LED编号操作不同的寄存器
    if (ledNumber < 5) { // 操作 GPE0 寄存器 (控制前5个灯)
        unsigned char ledByte = *m_gpe0Data;
        int correctedIndex = 4 - ledNumber; // 索引修正，因为硬件接线可能与逻辑顺序相反
        if (state) ledByte &= ~(1 << (7 - correctedIndex)); // 打开LED (低电平有效)
        else ledByte |= (1 << (7 - correctedIndex));       // 关闭LED (高电平有效)
        *m_gpe0Data = ledByte;
    } else { // 操作 GPE1 寄存器 (控制后3个灯)
        unsigned char ledByte = *m_gpe1Data;
        int idx = ledNumber - 5;
        if (state) ledByte &= ~(1 << idx); // 打开LED (低电平有效)
        else ledByte |= (1 << idx);       // 关闭LED (高电平有效)
        *m_gpe1Data = ledByte;
    }
}

// "全部打开" 按钮的槽函数
void MainWindow::onAllLedsOn()
{
    // 检查设备是否已成功初始化
    if (m_memFd < 0 || m_addrStart == NULL) {
        QMessageBox::critical(this, "错误", "GPIO设备未正确初始化!");
        return;
    }

    // 更新UI和内部状态
    for (int i = 0; i < 8; i++) {
        m_ledValues[i] = 1;
        m_ledCheckBoxes[i]->blockSignals(true);  // 临时阻塞信号，避免触发 onLedToggled
        m_ledCheckBoxes[i]->setChecked(true);
        m_ledCheckBoxes[i]->blockSignals(false); // 恢复信号
    }
    // 直接写寄存器，一次性打开所有LED
    *m_gpe0Data = 0x00;
    *m_gpe1Data = 0x00;
}

// "全部关闭" 按钮的槽函数
void MainWindow::onAllLedsOff()
{
    // 检查设备是否已成功初始化
    if (m_memFd < 0 || m_addrStart == NULL) {
        QMessageBox::critical(this, "错误", "GPIO设备未正确初始化!");
        return;
    }

    // 更新UI和内部状态
    for (int i = 0; i < 8; i++) {
        m_ledValues[i] = 0;
        m_ledCheckBoxes[i]->blockSignals(true);  // 临时阻塞信号
        m_ledCheckBoxes[i]->setChecked(false);
        m_ledCheckBoxes[i]->blockSignals(false); // 恢复信号
    }
    // 直接写寄存器，一次性关闭所有LED
    *m_gpe0Data = 0xFF;
    *m_gpe1Data = 0xFF;
}

// 安全关闭所有设备资源的函数
void MainWindow::closeAllDevices()
{
    // 确保设备已经被打开过
    if (m_memFd >= 0 && m_addrStart != NULL) {
        // 关闭所有LED灯
        *m_gpe0Data = 0xFF;
        *m_gpe1Data = 0xFF;
        // 解除内存映射
        munmap((void *)m_addrStart, 1024 * 16);
        // 关闭文件描述符
        ::close(m_memFd);
        // 重置状态变量
        m_memFd = -1;
        m_addrStart = NULL;
    }
}

// 构建用户界面的函数
void MainWindow::setupUi()
{
    // 创建中央窗口部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // 使用 QSS (Qt Style Sheets) 设置全局样式
    setStyleSheet("QMainWindow { background-color: white; }"
                 "QGroupBox { font-weight: bold; border: 2px solid #aaa; border-radius: 5px; margin-top: 8px; padding-top: 15px; background-color: #f8f8f8; }"
                 "QCheckBox { padding: 5px; margin: 2px; border-radius: 3px; }"
                 "QCheckBox:hover { background-color: #e0e0e0; }"
                 "QPushButton { border-radius: 5px; padding: 8px; }"
                 "QPushButton:pressed { background-color: #d0d0d0; }");
    
    // 创建并设置标题标签
    QLabel *titleLabel = new QLabel("LED控制面板", this);
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleFont.setFamily("unifont"); // 指定字体
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);
    titleLabel->setStyleSheet("background-color: #3498db; color: white; border-radius: 5px;");
    mainLayout->addWidget(titleLabel);
    
    // 创建 "LED控制" 分组框
    QGroupBox *ledGroup = new QGroupBox("LED控制");
    QFont groupFont;
    groupFont.setPointSize(18);
    groupFont.setBold(true);
    groupFont.setFamily("unifont");
    ledGroup->setFont(groupFont);
    QGridLayout *ledLayout = new QGridLayout(ledGroup); // 使用网格布局
    ledLayout->setSpacing(25);
    
    // 定义不同LED的颜色，用于UI显示
    QStringList ledColors;
    ledColors << "red" << "orange" << "gold" << "green" 
             << "blue" << "indigo" << "violet" << "gray";
    
    // 循环创建8个LED复选框
    for (int i = 0; i < 8; i++) {
        m_ledCheckBoxes[i] = new QCheckBox(QString("LED %1").arg(i+1));
        QFont checkFont;
        checkFont.setPointSize(20);
        checkFont.setBold(true);
        checkFont.setFamily("unifont");
        m_ledCheckBoxes[i]->setFont(checkFont);
        // 为每个复选框设置独特的样式
        m_ledCheckBoxes[i]->setStyleSheet(QString("QCheckBox { color: %1; background-color: white; border: 1px solid #ccc; } "
                                            "QCheckBox::indicator { width: 35px; height: 35px; }")
                                          .arg(ledColors.at(i)));
        m_ledCheckBoxes[i]->setMinimumHeight(60);
        // 将复选框添加到网格布局中
        ledLayout->addWidget(m_ledCheckBoxes[i], i/4, i%4);
    }
    
    // 将8个复选框的 toggled 信号连接到对应的槽函数
    connect(m_ledCheckBoxes[0], SIGNAL(toggled(bool)), this, SLOT(onLed0Toggled(bool)));
    connect(m_ledCheckBoxes[1], SIGNAL(toggled(bool)), this, SLOT(onLed1Toggled(bool)));
    connect(m_ledCheckBoxes[2], SIGNAL(toggled(bool)), this, SLOT(onLed2Toggled(bool)));
    connect(m_ledCheckBoxes[3], SIGNAL(toggled(bool)), this, SLOT(onLed3Toggled(bool)));
    connect(m_ledCheckBoxes[4], SIGNAL(toggled(bool)), this, SLOT(onLed4Toggled(bool)));
    connect(m_ledCheckBoxes[5], SIGNAL(toggled(bool)), this, SLOT(onLed5Toggled(bool)));
    connect(m_ledCheckBoxes[6], SIGNAL(toggled(bool)), this, SLOT(onLed6Toggled(bool)));
    connect(m_ledCheckBoxes[7], SIGNAL(toggled(bool)), this, SLOT(onLed7Toggled(bool)));
    
    mainLayout->addWidget(ledGroup);
    
    // 创建 "全局控制" 分组框
    QGroupBox *controlGroup = new QGroupBox("全局控制");
    controlGroup->setFont(groupFont);
    QHBoxLayout *buttonLayout = new QHBoxLayout(controlGroup); // 使用水平布局
    buttonLayout->setSpacing(20);
    
    m_allLedsOnBtn = new QPushButton("全部打开");
    m_allLedsOffBtn = new QPushButton("全部关闭");
    
    // 设置按钮字体
    QFont btnFont;
    btnFont.setPointSize(20);
    btnFont.setBold(true);
    btnFont.setFamily("unifont");
    m_allLedsOnBtn->setFont(btnFont);
    m_allLedsOffBtn->setFont(btnFont);
    
    m_allLedsOnBtn->setMinimumHeight(70);
    m_allLedsOffBtn->setMinimumHeight(70);
    
    // 设置按钮样式
    m_allLedsOnBtn->setStyleSheet("background-color: #27ae60; color: white; border: none;");
    m_allLedsOffBtn->setStyleSheet("background-color: #c0392b; color: white; border: none;");
    
    buttonLayout->addWidget(m_allLedsOnBtn);
    buttonLayout->addWidget(m_allLedsOffBtn);
    mainLayout->addWidget(controlGroup);
    
    // 连接 "全部打开" 和 "全部关闭" 按钮的 clicked 信号到槽函数
    connect(m_allLedsOnBtn, SIGNAL(clicked()), this, SLOT(onAllLedsOn()));
    connect(m_allLedsOffBtn, SIGNAL(clicked()), this, SLOT(onAllLedsOff()));
    
    // 创建并设置 "退出程序" 按钮
    QPushButton *exitButton = new QPushButton("退出程序", this);
    exitButton->setFont(btnFont);
    exitButton->setMinimumHeight(70);
    exitButton->setStyleSheet("background-color: #3498db; color: white; border: none;");
    connect(exitButton, SIGNAL(clicked()), this, SLOT(close())); // 连接到窗口的 close() 槽
    mainLayout->addWidget(exitButton);
    
    // 添加一个伸缩项，使所有控件顶部对齐
    mainLayout->addStretch();
}

// 初始化硬件设备函数
void MainWindow::initDevices()
{
    // 打开物理内存设备文件
    m_memFd = open(DEV_NAME, O_RDWR);
    if (m_memFd < 0) {
        QMessageBox::critical(this, "错误", "无法打开/dev/mem设备! 请确保以root权限运行。");
        return;
    }
    
    // 使用 mmap 将GPIO寄存器的物理地址映射到进程的虚拟地址空间
    // 0xE0200000 是目标开发板GPIOE寄存器组的基地址
    m_addrStart = (unsigned char *)mmap(NULL, 1024 * 16, PROT_READ | PROT_WRITE, MAP_SHARED, m_memFd, 0xE0200000);
    if (m_addrStart == MAP_FAILED) {
        QMessageBox::critical(this, "错误", "无法映射GPIO寄存器!");
        ::close(m_memFd);
        m_memFd = -1;
        return;
    }
    
    // 配置 GPE0 寄存器，将相关引脚设置为输出模式 (0x1)
    volatile unsigned int *gpe0con = (volatile unsigned int *)(m_addrStart + GPE0CON_OFFSET);
    *gpe0con = 0x11111000;
    
    // 配置 GPE1 寄存器，将相关引脚设置为输出模式 (0x1)
    volatile unsigned int *gpe1con = (volatile unsigned int *)(m_addrStart + GPE1CON_OFFSET);
    *gpe1con = 0x00000111;
    
    // 获取 GPE0 和 GPE1 数据寄存器的指针，用于后续的写操作
    m_gpe0Data = (volatile unsigned char *)(m_addrStart + GPE0DAT_OFFSET);
    m_gpe1Data = (volatile unsigned char *)(m_addrStart + GPE1DAT_OFFSET);
    
    // 初始化所有LED为关闭状态 (写入高电平)
    *m_gpe0Data = 0xFF;
    *m_gpe1Data = 0xFF;
}

// 程序退出时调用的清理函数
void resumeQtopia()
{
    // 唤醒Qtopia的图形界面进程，将控制权交还给系统桌面。
    system("killall -CONT qpe qss");
}

int main(int argc, char *argv[])
{
    // 在程序启动时立刻挂起Qtopia的图形界面。
    // 这个命令会冻结Qtopia相关进程，让我们的程序能够独占屏幕。
    system("killall -STOP qpe qss");

    // 注册清理函数，它将在程序正常退出时被自动调用。
    atexit(resumeQtopia);

    // 使用标准的构造函数，程序的运行模式由启动参数(-qws)决定。
    QApplication app(argc, argv);
    
    // 设置UTF-8编码，以正确显示中文
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    
    // 设置一个默认字体大小
    QFont defaultFont;
    defaultFont.setPointSize(12);
    app.setFont(defaultFont);
    
    // 创建主窗口对象
    MainWindow window;
    // 显示主窗口
    window.show();
    
    // 连接Qt应用程序的 aboutToQuit 信号到窗口的 closeAllDevices 槽。
    // 这确保了在程序退出前，会调用我们的清理函数来关闭LED和释放硬件资源。
    QObject::connect(&app, SIGNAL(aboutToQuit()), &window, SLOT(closeAllDevices()));
    
    // 进入Qt事件循环，程序将在此处运行，直到退出。
    return app.exec();
}
