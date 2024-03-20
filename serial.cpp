#include "serial.h"
#include "ui_serial.h"

Serial::Serial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Serial)
{
    ui->setupUi(this);
    serial_port = new QSerialPort();
    timer = new QTimer();
    chart = new QChart();
    series = new QSplineSeries[6];
    // series0 = new QSplineSeries();
    ax = new QDateTimeAxis();
    ax->setFormat("hh:mm:ss"); //设置显示日期格式
    ay = new QValueAxis();
    // values = new int[5];
    config_init();
    // config_timer();
    // config_chart();
}

Serial::~Serial()
{
    if (serial_port->isOpen())
        serial_port->close();
    if (timer->isActive())
        timer->stop();
    delete ui;
    delete serial_port;
    delete timer;
    delete chart;
    delete []series;
    //delete series;
    //delete series0;
    delete ax;
    delete ay;
    //delete []values;
}
/*-----------------------------------
 *            工具函数（静态类函数）
 * ---------------------------------*/
// 转化为16进制字符
char Serial::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
// 字符串转16进制
void Serial::String2Hex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1(); //.toAscii();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();//.toAscii();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}
// 字符串转16进制
QByteArray Serial::StringToHex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}
// 延时
void Serial::Delay(unsigned int ms)
{
    QTime late_time = QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < late_time)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
/*-----------------------------------
 *            普通函数
 * ---------------------------------*/
// 初始化配置
void Serial::config_init()
{
    /*-----------------初始化配置-------------------*/
    // 配置端口
    // serial_port->setPortName(ui->cmb_com->currentText());
    // serial_port->setParity(QSerialPort::NoParity);  // 设置无奇偶校验位
    // serial_port->setDataBits(QSerialPort::Data8);   // 默认8位数据位
    // serial_port->setStopBits(QSerialPort::OneStop); // 设置1位停止位
    this->config_port();
    this->config_timer();
    this->config_chart();
    // 以下打印可用串口设备信息
//    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
//    {
//        //qDebug() << info;
//        qDebug() << info.portName() << info.description() << info.serialNumber();
//        qDebug() << info.manufacturer() << info.productIdentifier();
//    }
    //QByteArray test;
    //test[0] = 0x01;
    //test[1] = 0x03;
    //qDebug() << test;
    // 绑定槽函数
    // connect(信号发出者, 信号（地址）, （接受信号）处理类（的地址）（在哪个类触发）, 接受类中用于处理的槽函数);
    // connect(ui->btn_open, &QPushButton::clicked, this, &Serial::on_btn_open_clicked);   // 对于使用QT生成的槽函数 可以不用自己手动绑定
    // connect(ui->btn_open, &QPushButton::clicked, this, &Serial::on_btn_close_clicked);

    // connect(serial_port, SIGNAL(QSerialPort::readyRead), this, SLOT(Serial::on_serial_received()));  // 绑定接收信号
    connect(serial_port, &QSerialPort::readyRead, this, &Serial::on_serial_received);  // 绑定串口接收数据信号
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_out())); // 绑定计时器逻辑 使用信号SIGNAL宏与槽函数SLOT宏
}
// 配置串口端口属性
void Serial::config_port()
{
    if (!serial_port)
        return;
    int index = 0;
    QString COM = "COM1";
    qint32 BAUD = QSerialPort::Baud115200;
    QSerialPort::Parity PARITY = QSerialPort::NoParity;
    QSerialPort::DataBits DBITS = QSerialPort::Data8;
    QSerialPort::StopBits SBITS = QSerialPort::OneStop;
    // --------------------------------设置端口名
    index = ui->cmb_com->currentIndex();
    switch (index)
    {
    case 0:
        COM = "COM1";
        break;
    case 1:
        COM = "COM2";
        break;
    case 2:
        COM = "COM3";
        break;
    case 3:
        COM = "COM4";
        break;
    case 4:
        COM = "COM5";
        break;
    case 5:
        COM = "COM6";
        break;
    case 6:
        COM = "COM7";
        break;
    case 7:
        COM = "COM8";
        break;
    case 8:
        COM = "COM9";
        break;
    }
    serial_port->setPortName(COM);
    // qDebug() << COM;
    //----------------------------------设置波特率
    index = ui->cmb_baud->currentIndex();
    switch (index)
    {
    case 0:
        BAUD = QSerialPort::Baud115200;
        break;
    case 1:
        BAUD = QSerialPort::Baud57600;
        break;
    case 2:
        BAUD = QSerialPort::Baud38400;
        break;
    case 3:
        BAUD = QSerialPort::Baud19200;
        break;
    case 4:
        BAUD = QSerialPort::Baud9600;
        break;
    case 5:
        BAUD = QSerialPort::Baud4800;
        break;
    case 6:
        BAUD = QSerialPort::Baud2400;
        break;
    case 7:
        BAUD = QSerialPort::Baud1200;
        break;
    }
    serial_port->setBaudRate(BAUD);
    // qDebug() << BAUD;
    //---------------------------------- 设置奇偶校验位
    index = ui->cmb_parity->currentIndex();
    switch (index)
    {
    case 0:
        PARITY = QSerialPort::NoParity;
        break;
    case 1:
        PARITY = QSerialPort::EvenParity;
        break;
    case 2:
        PARITY = QSerialPort::OddParity;
        break;
    case 3:
        PARITY = QSerialPort::MarkParity;
        break;
    case 4:
        PARITY = QSerialPort::SpaceParity;
        break;
    }
    serial_port->setParity(PARITY);
    // qDebug() << PARITY;
    //------------------------------------设置数据位
    index = ui->cmb_data_bits->currentIndex();
    switch (index)
    {
    case 0:
        DBITS = QSerialPort::Data8;
        break;
    case 1:
        DBITS = QSerialPort::Data7;
        break;
    case 2:
        DBITS = QSerialPort::Data6;
        break;
    case 3:
        DBITS = QSerialPort::Data5;
        break;
    }
    serial_port->setDataBits(DBITS);
    // qDebug() << DBITS;
    //------------------------------------设置停止位
    index = ui->cmb_stop_bits->currentIndex();
    switch (index)
    {
    case 0:
        SBITS = QSerialPort::OneStop;
        break;
    case 1:
        SBITS = QSerialPort::OneAndHalfStop;
        break;
    case 2:
        SBITS = QSerialPort::TwoStop;
        break;
    }
    serial_port->setStopBits(SBITS);
    // qDebug() << SBITS;
    serial_port->setFlowControl(QSerialPort::NoFlowControl);  // 设置无流控制
}
// 配置定时器属性
void Serial::config_timer()
{
    if (!timer)
        return;
    QString text = ui->edit_circle->text();
    int interval = text.isEmpty() || text.isNull() ? 1000 : text.toInt();
    timer->setInterval(interval);
}
// 配置图表属性
void Serial::config_chart()
{
    if (!chart || !series)
        return;
    // 设置曲线名
    int i = 0;
    for (i = 0; i < 6; ++i)
    {
        QString label = QString::asprintf("系列%d", i + 1);
        series[i].setName(label);
        chart->addSeries(&series[i]);
    }
    //series->setName("实时接收数据1");
    //series0->setName("实时接收数据2");
    // 把曲线数据序列加入到chart
    // chart->addSeries(series);
    // chart->addSeries(series0);
    // 声明并初始化x\y轴
    // 设置坐标轴范围
    ax->setMin(QDateTime::currentDateTime().addSecs(-1200*1));  // 1200 表示20分钟 后续升级可在控件设置 TODO:
    ax->setMax(QDateTime::currentDateTime().addSecs(0));
    // test y
    ay->setMin(300);     // TODO: 后续升级可在空间设置初始最大最小值
    ay->setMax(1500);
    // 设置坐标轴格点
    ax->setTickCount(6);
    ay->setTickCount(6);
    // 设置坐标轴标题
    ax->setTitleText("时间/s");
    ay->setTitleText("观测值");
    // 设置绘制曲线颜色 去网格等
    ax->setLinePenColor(QColor(Qt::darkBlue));
    ay->setLinePen(QPen(Qt::darkBlue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    // 将坐标轴添加到chart中
    // AlignBottom底部 AlignLeft左边 AlignRight右边 AlignTop顶部
    chart->addAxis(ax, Qt::AlignBottom);
    chart->addAxis(ay, Qt::AlignLeft);
    // 把曲线数据序列关联到坐标轴
    //qDebug() << series->attachAxis(ax);
    //qDebug() << series->attachAxis(ay);
    //qDebug() << series0->attachAxis(ax);
    //qDebug() << series0->attachAxis(ay);
    for (i = 0; i < 6; ++i)
    {
        series->setUseOpenGL(true);
        series[i].attachAxis(ax);
        series[i].attachAxis(ay);
    }
    // 将chart显示在view上
    ui->chart_view->setChart(chart);
}
// 画chart图表的曲线
void Serial::draw_line()
{
    if (!chart || !series)
        return;
    // 每增加一个点改变X轴的范围，实现曲线的动态更新效果
    QDateTime vt = QDateTime::currentDateTime();
    // qsrand((uint)QTime(0, 0, 0).secsTo(QTime::currentTime()));
    chart->axisX()->setMin(QDateTime::currentDateTime().addSecs(-1200*1));
    chart->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));
    //当曲线上最早的点超出X轴的范围时，剔除最早的点，
    //if(series->count() > 2399) // 1200 * 2 - 1
    //    series->removePoints(0, series->count() - 2399);
    //if(series0->count() > 2399) // 1200 * 2 - 1
    //    series0->removePoints(0, series->count() - 2399);

    // 测试 ------------- test y start
    // test
    //int vy = 400 + qrand() % 250;
    // 添加新的点到曲线末端
    //series->append(vt.toMSecsSinceEpoch(), vy);
    // 测试 ------------------ end
    QList<int> ks = this->kvs.keys();
    // QList<int> vs = this->kvs.values();
    // qDebug() << ks.length() << ks;
//    if (ks.length() >= 1)
//    {
//        int v1 = this->kvs.value(ks.at(0));
//        // int v2 = this->kvs.value(ks.at(1));
//        // qDebug() << v1 << v2;
//        // series->append(vt.toMSecsSinceEpoch(), v1 + qrand() % 10);
//        series->append(vt.toMSecsSinceEpoch(), v1 + qrand() % 10);
//        //series0->append(vt.toMSecsSinceEpoch(), v2 + qrand() % 10);
//        //chart->axisY()->setMin(v1 < v2 ? v1 - 20 : v2 - 20);
//        //chart->axisY()->setMax(v1 < v2 ? v2 + 30 : v1 + 20);
//    }
    for (int i = 0; i < ks.length(); ++i)
    {
        int v = this->kvs.value(ks.at(i));
        //if (i == 4) v -= 260;
        QPointF p;
        p.setX(vt.toMSecsSinceEpoch());
        p.setY(v);
        points[i].append(p);
        //series[i].append(vt.toMSecsSinceEpoch(), v); // + qrand() % 50
        series[i].replace(points[i]);
        //if (series[i].count() > 1200)
        //    series[i].removePoints(0, 1200-1);
    }
}
// 将写数据按符号分割成多行
QList<QByteArray> Serial::to_multipy(QString data, QChar sep)
{
    QStringList slist;
    QList<QByteArray> list;
    if (!data.isEmpty() && !data.isNull())  // 若数据不为空
    {
        slist = data.split(QString(sep));     // 用sep分割为列表
        if (slist.length() == 0)
            slist.append(data);
        qDebug() << slist << slist.length();
        foreach (QString s, slist)
        {
            QByteArray buf;
            if (ui->rad_hex->isChecked())
                buf = StringToHex(s);
            // else if (ui->rad_bin->isChecked()) // TODO: BIN DEC
            else
                buf = s.toLatin1();
            list.append(buf);
        }
    }
    return list;
}
// 将读的数据分成多行
QStringList Serial::to_multipy(QByteArray buf, QChar sep)
{
    QString sdata;
    QByteArray data;
    QStringList list;
    if(!buf.isEmpty())
    {
        if (ui->rad_hex->isChecked())
            data = buf.toHex();
        // else if (ui->rad_bin->isChecked()) // TODO: BIN DEC
        else
            data = buf;
        sdata = QString(data);
        list = sdata.split(sep);
        if (list.length() == 0)
            list.append(sdata);
    }
    return list;
}
// 将字节转为相应进制的数据帧字符串
QString Serial::to_frame(QByteArray buf)
{
    QString text;
    if (ui->rad_hex->isChecked())
    {
        text = QString(buf.toHex());
        for (int i = 2; i <= text.length() - 2; i += 3)
        {
            if (text[i] != ' ')
                text = text.insert(i, ' ');
        }
        text = text.toUpper();
    }
    // else if (ui->rad_bin->isChecked()) // TODO: BIN DEC
    else
        text = QString(buf);
    return text;
}
void Serial::to_frame(QString &text)
{
    if (ui->rad_hex->isChecked())
    {
        for (int i = 2; i <= text.length() - 2; i += 3)
        {
            if (text[i] != ' ')
                text = text.insert(i, ' ');
        }
        text = text.toUpper();
    }
    // else if (ui->rad_bin->isChecked()) // TODO: BIN DEC
}
/*-----------------------------------
 *            槽信号函数
 * ---------------------------------*/
// 打开串口
void Serial::on_btn_open_clicked(bool)
{
    /* ------------测试-------------*/
    //int test = 16;
    //qDebug() << "打开串口成功！";
    //qDebug("%d\n", test);
    //qDebug() << ui->cmb_com->currentIndex() << ui->cmb_com->currentText(); // 当前选中项
    /* ------------测试-------------*/

    /*-----------端口操作------------*/
    //---------------------------------- 配置串口
    // this->config_port();
    //---------------------------------- 打开串口
    if (serial_port->isOpen())
        return;
    // this->config_port();
    bool flag = serial_port->open(QIODevice::ReadWrite);    // 打开串口，并设置读写模式
    // serial_port->open(QIODevice::ReadWrite);    // 打开串口，并设置读写模式
    qDebug() << "串口打开状态：" << flag;
    if (!flag)
        return;
    /*-----------端口操作------------*/
    ui->btn_close->setEnabled(flag);
    ui->btn_open->setEnabled(!flag);
}
// 发送串口数据
void Serial::on_btn_send_clicked(bool)
{
    QString data = ui->edit_send->text();
    QList<QByteArray> list = this->to_multipy(data, ';');  // 按;分割多行数据
    // qDebug() << list << data;
    QDateTime time;
    foreach (QByteArray bytes, list)  // Qt中 foreach用法
    {
        time = QDateTime::currentDateTime();
        // yyyy-MM-dd hh:mm:ss.zzz 以该形式显示时间 例：16:15:11.566
        QString head = "Send:[" + time.toString("yyyy-MM-dd hh:mm:ss.zzz") + "]->";
        ui->edit_text_buffer->insertPlainText(head + this->to_frame(bytes) + "\n");
        ui->edit_text_buffer->moveCursor(QTextCursor::End);  // 光标移动到最后
        serial_port->write(bytes);
        this->Delay(200); // 延时300ms
    }
}
// 接收串口数据
void Serial::on_serial_received()
{
    QByteArray buf;
    QStringList list;
    QDateTime time;
//    qint64 bytes;
//    while ((bytes = serial_port->bytesAvailable()) != 0)
//    {
//        QByteArray data = serial_port->read(bytes);
//        buf += data;
//    }
    Delay(200); // 确保数据读完
    buf = serial_port->readAll();
    list = this->to_multipy(buf, ';');   // 若有多行数据
    //qDebug() << "received:" << list;
    buf.clear();
    // this->kvs.clear();  // 将之前数据清空
    foreach(QString s, list)
    {
        //qDebug() << s;
        //--------------------- 以下解析数据 TODO:后续可以在控件设置解析哪几帧
        bool ok;
        int k, v;
        k = s.mid(0, 2).toInt(&ok, 16);
        qDebug() << "转化地址：" << ok << k << s.mid(0, 2);
        v = s.mid(6, 4).toInt(&ok, 16);
        qDebug() << "转化数值" << ok << v << s.mid(6, 4);
//        v*=10;
        this->kvs.insert(k, v); //插入键值对，若已经有则覆盖
        //---------------------解析数据
        time = QDateTime::currentDateTime();
        QString head = "Recv:[" + time.toString("yyyy-MM-dd hh:mm:ss.zzz") + "]<-";
        this->to_frame(s);
        ui->edit_text_buffer->insertPlainText(head + s + "\n");
        ui->edit_text_buffer->insertPlainText(QString("value:%1:").arg(k) + time.toString("yyyy-MM-dd hh:mm:ss.zzz") + "=" + QString("%1\n").arg(v));
        ui->edit_text_buffer->moveCursor(QTextCursor::End);  // 光标移动到最后
    }
    // 以下画图
    if (!ui->btn_stop->isEnabled())  // 不允许画画
        return;
    this->draw_line();
}
// 关闭串口
void Serial::on_btn_close_clicked(bool)
{
    if (serial_port->isOpen())
    {
        serial_port->clear();
        serial_port->close();
    }
    ui->btn_open->setEnabled(true);
    ui->btn_close->setEnabled(false);
}
// 清空发送缓冲区
void Serial::on_btn_clear_clicked(bool)
{
    ui->edit_text_buffer->clear();
}
// 确定
void Serial::on_btn_ok_clicked(bool)
{
    //配置串口
    this->config_port();
}
// 重置端口属性
void Serial::on_btn_reset_clicked(bool)
{
    if (serial_port->isOpen())
    {
        serial_port->clear();
        serial_port->close();
        ui->btn_open->setEnabled(true);
        ui->btn_close->setEnabled(false);
    }
    // 重置为串口默认初始化配置
    serial_port->setPortName("COM1");
    serial_port->setBaudRate(QSerialPort::Baud115200); // 设置波特率115200
    serial_port->setParity(QSerialPort::NoParity);  // 设置无奇偶校验位
    serial_port->setDataBits(QSerialPort::Data8);   // 默认8位数据位
    serial_port->setStopBits(QSerialPort::OneStop); // 设置1位停止位
    // 复选框 复位
    ui->cmb_com->setCurrentIndex(0);
    ui->cmb_baud->setCurrentIndex(0);
    ui->cmb_parity->setCurrentIndex(0);
    ui->cmb_data_bits->setCurrentIndex(0);
    ui->cmb_stop_bits->setCurrentIndex(0);
}
// 保存数据文件
void Serial::on_btn_save_clicked(bool)
{
    QDateTime save_time = QDateTime::currentDateTime();
    QString text = ui->edit_text_buffer->toPlainText();
    QString name = QDir::currentPath() + "/" + save_time.toString("yyyy-MM-dd") + "/";  // save_time.toString("yyyy-MM-dd-hh-mm")
    qDebug() << name;
    // 存为txt文件
    if (ui->chk_txt->isChecked())
    {
        QDir dir; // dir(name)
        if (!dir.exists(name))
        {
            bool ok = dir.mkdir(name);
            qDebug() << "create dir:" << ok;
        }
        QFile file(name + save_time.toString("yyyy-MM-dd-hh-mm") + ".txt");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        qint64 flag = file.write(text.toUtf8());
        file.close();
        qDebug() << flag;
    }
    if (ui->chk_csv->isChecked())
    {
        // TODO::
    }
}
// 处理定时器逻辑
void Serial::on_timer_out()
{
    /*-----------测试------------*/
    //qDebug() << QDateTime::currentDateTime();
    //draw_line();
    //----------- 串口写
    //QString data = ui->edit_send->text();
    //QByteArray buf = data.toLatin1();
    //serial_port->write(buf);
    /*-----------测试------------*/
    if (!serial_port->isOpen())
        return;
    this->on_btn_send_clicked(true);
}
// 处理循环发送
void Serial::on_chk_circle_stateChanged(int)
{
    if (ui->chk_circle->isChecked())
    {
        this->config_timer();
        timer->start();
        //qDebug() << "开始计时！";
    }
    else
    {
        timer->stop();
        //qDebug() << "计时终止！";
    }
}
// 开始画图
void Serial::on_btn_draw_clicked(bool)
{
    ui->btn_draw->setEnabled(false);
    ui->btn_stop->setEnabled(true);
}
// 停止画图
void Serial::on_btn_stop_clicked(bool)
{
    ui->btn_draw->setEnabled(true);
    ui->btn_stop->setEnabled(false);
}
