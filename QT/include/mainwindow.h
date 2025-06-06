#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCheckBox;
class QPushButton;
class QLabel;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // LED控制
    void onLedToggled(int ledNumber, bool state);
    void onLed0Toggled(bool state);
    void onLed1Toggled(bool state);
    void onLed2Toggled(bool state);
    void onLed3Toggled(bool state);
    void onLed4Toggled(bool state);
    void onLed5Toggled(bool state);
    void onLed6Toggled(bool state);
    void onLed7Toggled(bool state);
    void onAllLedsOn();
    void onAllLedsOff();
    
    // 全局控制
    void closeAllDevices();

private:
    // 设备文件描述符和寄存器
    int m_memFd;                   // 内存设备文件描述符
    unsigned char *m_addrStart;    // 映射基地址
    volatile unsigned char *m_gpe0Data;  // GPE0数据寄存器
    volatile unsigned char *m_gpe1Data;  // GPE1数据寄存器
    
    // 数据存储
    int m_ledValues[8];      // LED状态
    
    // UI组件
    QCheckBox *m_ledCheckBoxes[8];
    QPushButton *m_allLedsOnBtn;
    QPushButton *m_allLedsOffBtn;
    
    // 初始化函数
    void setupUi();
    void initDevices();
};

#endif // MAINWINDOW_H 