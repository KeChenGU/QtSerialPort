#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>

/*-----------自定义添加-------------*/
#include <QtCharts>      // 包含QChart QChartView 所有和图标相关的类
#include <QSerialPort>   // 包含串口操作所有类
#include <QSerialPortInfo>
#include <QDateTime>     // 显示日期时间用
#include <QDebug>
/*-----------自定义添加-------------*/

QT_CHARTS_USE_NAMESPACE
// using namespace QCharts;

namespace Ui {
class Serial;
}

class Serial : public QMainWindow
{
    Q_OBJECT

public:
    explicit Serial(QWidget *parent = nullptr);
    ~Serial();

private slots:
    void on_btn_open_clicked(bool);  // 点击打开串口
    void on_btn_send_clicked(bool);  // 向串口发送数据
    void on_serial_received();       // !串口接收数据 重要
    void on_btn_close_clicked(bool); // 点击关闭串口
    void on_btn_clear_clicked(bool); // 点击清空发送区
    void on_btn_ok_clicked(bool);    // 确定配置
    void on_btn_reset_clicked(bool); // 重置端口连接配置
    void on_btn_save_clicked(bool);  // 保存文本缓冲区 和 图表区的 数据文件
    void on_timer_out();             // !处理定时逻辑
    void on_chk_circle_stateChanged(int);   // 是否循环发送
    void on_btn_draw_clicked(bool);  // 点击允许实时画图
    void on_btn_stop_clicked(bool);  // 点击停止画图

private:
    Ui::Serial *ui;
    /*------函数-------*/
    void config_init();         // 初始配置串口
    void config_port();         // 配置串口
    void config_timer();        // 配置定时器
    void config_chart();        // 配置数据图表
    void draw_line();           // 画图表曲线
    QList<QByteArray> to_multipy(QString, QChar);   // 发送多行
    QStringList to_multipy(QByteArray, QChar);      // 接收多行
    QString to_frame(QByteArray);                   // 转化为数据帧字符串
    void to_frame(QString&);                        //
    /*------静态工具类函数------*/
    static char ConvertHexChar(char);
    static void String2Hex(QString, QByteArray&);
    static QByteArray StringToHex(QString);
    static void Delay(unsigned int);
    /*------变量-------*/
    QSerialPort *serial_port;  // 串口对象
    QTimer *timer;             // 定时器 用于循环发送
    QChart *chart;             // 数据图表 显示数据用
    QSplineSeries *series;     // 数据序列 , *series0
    QDateTimeAxis *ax;         // 显示时间的x轴
    QValueAxis *ay;            // 显示数据的y轴
    //int *values;             // 显示的数据值
    //QVector<int> values;     // 存显示的数据值
    QList<QPointF> points[6];     // 数据点
    QMap<int, int> kvs;        // 存对应序号的数据值
};

#endif // SERIAL_H
