#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <download.h>
#include "download.h"
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>

#include <QGraphicsSimpleTextItem>

#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMessageBox>
#include <QDesktopServices>


// 3、初始化自定义窗口类(为了callout能使用MainWindow的ui界面）
//MainWindow* MainWindow::myMainWindow = nullptr;
//QGraphicsView(new QGraphicsView,parent),
//QMainWindow(parent),




MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QBuffer wavBuffer(&wavByteArray);//将wav文件写入到这个变量之中

    //7.5 ======将要写入数据的wavBuffer变量打开读写权限
    wavBuffer.open(QBuffer::ReadWrite);

    //默认设置为全屏
    this->setWindowState(Qt::WindowMaximized);

    //4.给自定义窗口赋值(为了callout能使用MainWindow的ui界面）
//    myMainWindow = this;

    resize(2000,800);

    //设置功能开启类
    //设置鼠标可以跟随

    ui->chartView->setMouseTracking(true);
    ui->chartView2->setMouseTracking(true);
//    this->setMouseTracking(true);
    //启用chartView的鼠标跟踪
    ui->chartView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->chartView2->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    //启用underMouse，可以检测鼠标事件的位置
    ui->chartView->setAttribute(Qt::WA_UnderMouse,true);
    ui->chartView2->setAttribute(Qt::WA_UnderMouse,true);

    //读取wav文件的数据  2.25

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);//循环播放
    player->setPlaylist(playlist);

    //状态变化
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),
            this,SLOT(on_stateChanged(QMediaPlayer::State)));
    //位置改变
    connect(player,SIGNAL(positionChanged(qint64)),
            this,SLOT(on_positionChanged(qint64)));
    //时间总长度的改变
    connect(player,SIGNAL(durationChanged(qint64)),
            this,SLOT(on_durationChanged(qint64)));
    //播放列表，当前播放文件改变
    connect(playlist,SIGNAL(currentIndexChanged(int)),
            this,SLOT(on_PlayListIndexChanged(int)));

    //wav
    //wav  chart表的改变
    connect(player,SIGNAL(durationChanged(qint64)),
            this,SLOT(on_wav_durationChanged(qint64)));
    //wav  绘图的改变
    connect(playlist,SIGNAL(currentIndexChanged(int)),
            this,SLOT(on_wav_PlayListIndexChanged(int)));

    //给画图界面，鼠标右键生成的菜单栏添加功能&QAction::triggered
    rightButtonMenu = new QMenu(this);
    addNote = new QAction(QStringLiteral("添加注释"),rightButtonMenu);
    connect(addNote,SIGNAL(triggered(bool)),
            this,SLOT(on_addNote_triggered(bool)));

    //7.4   保存文本功能
    //这是将整个字典生成一个文件
    saveText = new QAction(QStringLiteral("保存文本文件"),rightButtonMenu);
    connect(saveText,SIGNAL(triggered(bool)),
            this,SLOT(on_saveText_triggered(bool)));
    //更新字典中某一个键值对
    renewText = new QAction(QStringLiteral("更新字典"),rightButtonMenu);
    connect(renewText,SIGNAL(triggered(bool)),
            this,SLOT(on_renewText_triggered(bool)));





    //隐藏后面2列
//    ui->tableWidgetNote->hideColumn(5);
//    ui->tableWidgetNote->hideColumn(6);
    //tableWidget设置最佳列和行
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    //放到linux环境下运行，所以那些本地文件类的暂时注释掉=======================================================
    //====================测试用功能，先把text文件存在本地测试
    //文本列表先从本地获取
    QFile text("C:\\Users\\kk\\Desktop\\text.txt");

    //设置一些报错信息
    if (! text.open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug()<<text.errorString();
    else
        qDebug()<<"text open ok";
//    text.seek()
    //文本流读取
    QTextStream textShuru(&text);

    //设置编码，还不知道设置什么
    textShuru.setCodec("UTF-8");

    //提前设置好变量
    int pos;
    QString pre;
    QString post;

    //一行一行的读取
    while (! textShuru.atEnd())
    {
        QString line = textShuru.readLine();
//        qDebug()<<line;
        //pos就是指第一次遇到空格
        pos = line.indexOf(" ");
        pre = line.mid(0,pos).trimmed();//文件名字
        post = line.mid(pos).trimmed();//文本内容
        mapNameAndText.insert(pre,post);//添加到字典中
    }
    text.close();


    //加载scp文件，btnadd里面我全部注销了
//    QFile scp("D:\\workSpace\\Project\\music\\wav.scp");
    //为了方便调试，只加载一项，新的scp和text文件只包含一项
    QFile scp("C:\\Users\\kk\\Desktop\\wav.scp");
    if (! scp.open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug()<<scp.errorString();
    else
        qDebug()<<"openok";

    QTextStream shuru(&scp);
    shuru.setCodec("UTF-8");

    //提前设置好变量
    QString prefix;
    QString line;
    QStringList list;

    //有用的，暂时注释掉
    while (! shuru.atEnd())
    {
        prefix = "http://101.35.48.220:10109";
        line = shuru.readLine();
        list = line.split(QRegExp("\\s+"));
        list[1] = prefix + line.mid(line.indexOf("/home/mgd519/resources/uyghur_data/RY-UG-4")+42);
        qDebug()<<list[1];
        mapNameAndAddress.insert(list[0],list[1]);
        ui->listWidget_2->addItem(list[0]);
    }
    scp.close();



}




MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_stateChanged(QMediaPlayer::State state)
{//改变按钮状态
    qDebug()<<"on_stateChanged(QMediaPlayer::State state)";
    if (testTime != NULL)
    {
        ui->btnPlay->setEnabled(true);
    }
    else
    {
        ui->btnPlay->setEnabled(!(state==QMediaPlayer::PlayingState));
    }

    ui->btnPause->setEnabled(state==QMediaPlayer::PlayingState);
    ui->btnStop->setEnabled(state==QMediaPlayer::PlayingState);
}

void MainWindow::on_positionChanged(qint64 position)
{
    qDebug()<<"on_positionChanged(qint64 position)";
    qDebug()<<"position="<<position;
    //正在拖拽的时候，就不要影响了，直接返回
    if (ui->sliderPosition->isSliderDown()) return;

    ui->sliderPosition->setSliderPosition(position);

    int secs = position/1000;
    int mins = secs/60;
    secs = secs%60;
    //当前播放位置
    positionTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);

    qDebug()<<"mins*60+secs="<<mins*60+secs;
    qDebug()<<"player->position()="<<player->position();

    //当选中一段区域之后,播放到结尾之后，重新回到起点播放
    if (isFragmentPlay)
    {
//        if (fabs(player->position() - endTimeEdit*1000) < 1000)
        if ((mins*60+secs) == endTimeEdit)
        {
            on_positionChanged(startTimeEdit*1000);
            ui->sliderPosition->valueChanged(startTimeEdit);
            player->setPosition(startTimeEdit*1000);
//            on_btnPlay_clicked();
        }
    }
}

void MainWindow::on_durationChanged(qint64 duration)
{
    qDebug()<<"on_durationChanged(qint64 duration)";
    //设置滑动的最大值
    ui->sliderPosition->setMaximum(duration);
    //以毫秒为单位，所以/1000
    int secs = duration/1000;
//    int ms = duration%1000;//毫秒
    int mins = secs/60;
    secs = secs%60;
    //总时长
    durationTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);

    //播放文件切换的时候，更改表格的数据
    ui->tableWidget->setItem(1,0,new QTableWidgetItem("0:00"));
    ui->tableWidget->setItem(1,1,new QTableWidgetItem(durationTime));
    ui->tableWidget->setItem(1,2,new QTableWidgetItem(durationTime));
}

void MainWindow::on_PlayListIndexChanged(int index)
{//播放列表位置改变
    qDebug()<<"on_PlayListIndexChanged(int index)";
//    ui->tableWidgetNote->clear();

    testTime = 0;
    ui->listWidget->setCurrentRow(index);//设置当前行为index
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item)
        ui->LabCurMedia->setText(item->text());
}


void MainWindow::on_btnAdd_clicked()
{
    qDebug()<<"on_btnAdd_clicked()";
    //设置默认打开路径
//    QString curPath = QDir::currentPath();
    QString curPath = "D:/workSpace/Project/music";
    //设置窗口标题
    QString dlgTitle = "选择音频文件";
    //设置过滤器
    QString filter = "音频文件(*.mp3 *.wav *.wma);;"
                     "wav文件(*.wav);;"
                     "wma文件(*.wma);;"
                     "mp3文件(*.mp3);;"
                     "scp文件(*.scp);;"
                     "所有文件(*.*)";
    //获取播放列表
    QStringList fileList = QFileDialog::getOpenFileNames(this,dlgTitle,curPath,filter);
    if (fileList.count() < 0) return;//如果添加的数量小于0，返回
    //添加每个文件的名字到播放列表
    foreach (auto aFile, fileList) {
        playlist->addMedia(QUrl::fromLocalFile(aFile));//因为是当地文件
        QFileInfo fileInfo(aFile);//获取信息
        ui->listWidget->addItem(fileInfo.absoluteFilePath());//只获取绝对路径
    }
    //如果不是播放状态，设置索引为第一个
    if (player->state()!= QMediaPlayer::PlayingState) {
        playlist->setCurrentIndex(0);
    }
    player->play();
}

void MainWindow::on_btnRemove_clicked()
{//移除列表
    qDebug()<<"on_btnRemove_clicked()";
    //从listWidget和playlist中删除
    int pos = ui->listWidget->currentRow();
    QListWidgetItem *item = ui->listWidget->takeItem(pos);
    delete item;
    playlist->removeMedia(pos);

//    if (playlist->currentIndex()==pos) //是当前播放的曲目
//    {
//        int nextPos=0;
//        if (pos>=1)
//            nextPos=pos-1;

//        playlist->removeMedia(pos);//从播放列表里移除
//        if (ui->listWidget->count()>0)
//        {
//            playlist->setCurrentIndex(nextPos);
//            on_PlayListIndexChanged(nextPos);
//        }
//        else
//        {
//            player->stop();
//            ui->LabCurMedia->setText("无曲目");
//        }
//    }
//    else
//        playlist->removeMedia(pos);

    //如果播放列表还存在项，改变LabCurMedia的文本
    QString str;
    if (ui->listWidget->currentItem())
        str = ui->listWidget->currentItem()->text();
    ui->LabCurMedia->setText(str);
}

void MainWindow::on_btnClear_clicked()
{//清空列表
    qDebug()<<"on_btnClear_clicked()";
    playlist->clear();
    ui->listWidget->clear();
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{//双击播放列表
    qDebug()<<"on_listWidget_doubleClicked(const QModelIndex &index)";
    int rowNO = index.row();
    playlist->setCurrentIndex(rowNO);
    player->play();
}

void MainWindow::on_btnPlay_clicked()
{//播放按钮
    qDebug()<<"on_btnPlay_clicked()";
    if (playlist->currentIndex() < 0)
        playlist->setCurrentIndex(0);
    qDebug()<<isFragment;
    qDebug()<<isFragmentPlay;

    //单机表格的时候，回放到这里
    if (isFragment)
    {
        qDebug()<<testTime;
        on_positionChanged(startTimeEdit*1000);
        ui->sliderPosition->valueChanged(startTimeEdit);
        player->setPosition(startTimeEdit*1000);
    }
    else if (isFragmentPlay)
    {
        on_positionChanged(startTimeEdit*1000);
        ui->sliderPosition->valueChanged(startTimeEdit);
        player->setPosition(startTimeEdit*1000);
    }

    player->play();
}

void MainWindow::on_btnPause_clicked()
{//暂停按钮
    player->pause();
}

void MainWindow::on_btnStop_clicked()
{//停止按钮
    player->stop();
}

void MainWindow::on_btnPrevious_clicked()
{//前一首
    playlist->previous();
}

void MainWindow::on_btnNext_clicked()
{//下一首
    playlist->next();
}

void MainWindow::on_sliderVolumn_valueChanged(int value)
{//设置声音大小
    player->setVolume(value);
}

void MainWindow::on_sliderPosition_valueChanged(int value)
{
    player->setPosition(value);

}

void MainWindow::on_btnSound_clicked()
{
    //静音按钮
    bool mute = player->isMuted();
    player->setMuted(!mute);
    mute = player->isMuted();

    //改变图标
    if(mute)
        ui->btnSound->setIcon(QIcon(":/images/images/mute.bmp"));
    else
        ui->btnSound->setIcon(QIcon(":/images/images/volumn.bmp"));
}

//4.14展示不要
//void MainWindow::on_btnDownload_clicked()
//{
//    //打开一个新窗口
//    download_page = new download;
//    download_page->show();


//}

//设置wav文件的信息
void MainWindow::setWavFile(QString strFileName)
{
    qDebug()<<"setWavFile(QString strFileName)";
    m_Wavefile.readWave(string((const char *)strFileName.toLocal8Bit()));
}

void MainWindow::on_wav_durationChanged(qint64 duration)
{


    qDebug()<<"on_wav_durationChanged(qint64 duration)";
    //添加wav波形做的改变
    //播放时长和播放进度获取
    //创建显示图表
    QChart *chart = new QChart;
    QChart *chart2 = new QChart;
    chart->setTitle("音频输入原始信号");
    ui->chartView->setChart(chart);
    ui->chartView2->setChart(chart2);
//    ui->chartView->setRenderHint(QPainter::Antialiasing);
    lineSeries = new QLineSeries(); //序列  上层曲线
    lineSeries2 = new QLineSeries();
    chart->addSeries(lineSeries);
    chart2->addSeries(lineSeries2);

//    QValueAxis *axisX = new QValueAxis;  //坐标轴
    axisX = new QValueAxis;
    axisX->setRange(0, duration/1000); //设置范围
    axisX->setLabelFormat("%g"); //是个实数，让它自己去选
    axisX->setTitleText("Samples");

//    QValueAxis *axisY = new QValueAxis;  //坐标轴
    axisY = new QValueAxis;
    axisY->setRange(0, 256); // UnSingedInt采样，数据范围0-255，故意设置的大一点，好看
    axisY->setRange(-30000, 30000);//测试用sin函数
    axisY->setTitleText("Audio level");

    axisX2 = new QValueAxis;
    axisX2->setRange(0, duration/1000); //设置范围
    axisX2->setLabelFormat("%g"); //是个实数，让它自己去选
    axisX2->setTitleText("Samples");

    axisY2 = new QValueAxis;
    axisY2->setRange(0, 256); // UnSingedInt采样，数据范围0-255，故意设置的大一点，好看
    axisY2->setRange(-30000, 30000);//测试用sin函数
    axisY2->setTitleText("Audio level");

//    chart->setAxisX(axisX, lineSeries);
//    chart->setAxisY(axisY, lineSeries);
    //setAxisX()，setAxisY()函数在Qt 5.12.1中过时，用下面的方法替代

    chart->addAxis(axisX,Qt::AlignBottom);
    chart->addAxis(axisY,Qt::AlignLeft);
    chart2->addAxis(axisX2,Qt::AlignBottom);
    chart2->addAxis(axisY2,Qt::AlignLeft);
    lineSeries->attachAxis(axisX);  //添加坐标轴
    lineSeries->attachAxis(axisY);
    lineSeries2->attachAxis(axisX2);
    lineSeries2->attachAxis(axisY2);

    chart->legend()->hide();//chart的标题下面有个小方块，隐藏
    chart2->legend()->hide();//chart的标题下面有个小方块，隐藏


//    //sin函数测试
//    lineSeries->clear();
//    qsrand(QTime::currentTime().second());//随机数初始化
//    qreal t=0,y,intv=0.1,rd;
//    int cnt=player->duration()/100;
//    for(int i = 0; i < cnt ; i++)
//    {
//        rd=(qrand() % 10) - 5;
//        y = qSin(t) + rd/50;
//        *lineSeries<<QPointF(t,y);
//        t+=intv;
//    }

    //wav
    // wav文件头信息结构
    struct WAVFILEHEADER
    {
        // RIFF 头;
        char RiffName[4];
        unsigned long nRiffLength;

        // 数据类型标识符;
        char WavName[4];

        // 格式块中的块头;
        char FmtName[4];
        unsigned long nFmtLength;

        // 格式块中的块数据;
        unsigned short nAudioFormat;
        unsigned short nChannleNumber;
        unsigned long nSampleRate;
        unsigned long nBytesPerSecond;
        unsigned short nBytesPerSample;
        unsigned short nBitsPerSample;


        // 附加信息(可选),根据 nFmtLength 来判断;
        // 扩展域大小;
        unsigned short nAppendMessage;
        // 扩展域信息;
        char* AppendMessageData;

        //Fact块,可选字段，一般当wav文件由某些软件转化而成，则包含该Chunk;
        char FactName[4];
        unsigned long nFactLength;
        char FactData[4];

        // 数据块中的块头;
        char    DATANAME[4];
        unsigned long   nDataLength;

        // 以下是附加的一些计算信息;
        int fileDataSize;               // 文件音频数据大小;
        int fileHeaderSize;             // 文件头大小;
        int fileTotalSize;              // 文件总大小;


        // 理论上应该将所有数据初始化，这里只初始化可选的数据;
        WAVFILEHEADER()
        {
            nAppendMessage = 0;
            AppendMessageData = NULL;
            strcpy(FactName, "");
            nFactLength = 0;
            strcpy(FactData, "");
        }




    };
//    QString fileAbsoluteFilePath = item->text();
//    QFile fileInfo(fileAbsoluteFilePath);
    QListWidgetItem *item = ui->listWidget->currentItem();
    QFile fileInfo(item->text());
    qDebug()<<"============当前播放text（）" << item->text();

    if (!fileInfo.open(QIODevice::ReadOnly))
    {
        return ;
    }
    WAVFILEHEADER m_wavFileHeader;
    fileInfo.read(m_wavFileHeader.RiffName, sizeof(m_wavFileHeader.RiffName));
    fileInfo.read((char*)&m_wavFileHeader.nRiffLength, sizeof(m_wavFileHeader.nRiffLength));
    fileInfo.read(m_wavFileHeader.WavName, sizeof(m_wavFileHeader.WavName));
    fileInfo.read(m_wavFileHeader.FmtName, sizeof(m_wavFileHeader.FmtName));
    fileInfo.read((char*)&m_wavFileHeader.nFmtLength, sizeof(m_wavFileHeader.nFmtLength));
    fileInfo.read((char*)&m_wavFileHeader.nAudioFormat, sizeof(m_wavFileHeader.nAudioFormat));
    fileInfo.read((char*)&m_wavFileHeader.nChannleNumber, sizeof(m_wavFileHeader.nChannleNumber));
    fileInfo.read((char*)&m_wavFileHeader.nSampleRate, sizeof(m_wavFileHeader.nSampleRate));
    fileInfo.read((char*)&m_wavFileHeader.nBytesPerSecond, sizeof(m_wavFileHeader.nBytesPerSecond));
    fileInfo.read((char*)&m_wavFileHeader.nBytesPerSample, sizeof(m_wavFileHeader.nBytesPerSample));
    fileInfo.read((char*)&m_wavFileHeader.nBitsPerSample, sizeof(m_wavFileHeader.nBitsPerSample));

    QString strAppendMessageData;
    if (m_wavFileHeader.nFmtLength >= 18)
    {
        fileInfo.read((char*)&m_wavFileHeader.nAppendMessage, sizeof(m_wavFileHeader.nAppendMessage));

        int appendMessageLength = m_wavFileHeader.nFmtLength - 18;
        m_wavFileHeader.AppendMessageData = new char[appendMessageLength];
        fileInfo.read(m_wavFileHeader.AppendMessageData, appendMessageLength);
        strAppendMessageData = QString(m_wavFileHeader.AppendMessageData);
    }
    char chunkName[5];
    fileInfo.read(chunkName, sizeof(chunkName) - 1);
    chunkName[4] = '\0';
    QString strChunkName(chunkName);
    if (strChunkName.compare("fact") == 0)
    {
        strcpy(m_wavFileHeader.FactName, chunkName);
        fileInfo.read((char*)&m_wavFileHeader.nFactLength, sizeof(m_wavFileHeader.nFactLength));
        fileInfo.read(m_wavFileHeader.FactData, sizeof(m_wavFileHeader.FactData));
        fileInfo.read(m_wavFileHeader.DATANAME, sizeof(m_wavFileHeader.DATANAME));
    }
    else
    {
        strcpy(m_wavFileHeader.DATANAME, chunkName);
    }

    fileInfo.read((char*)&m_wavFileHeader.nDataLength, sizeof(m_wavFileHeader.nDataLength));

    QByteArray pcmData;
    pcmData = fileInfo.readAll();
    m_wavFileHeader.fileDataSize = pcmData.size();
    m_wavFileHeader.fileTotalSize = m_wavFileHeader.nRiffLength + 8;
    m_wavFileHeader.fileHeaderSize = m_wavFileHeader.fileTotalSize - m_wavFileHeader.fileDataSize;

    fileInfo.close();


    QVector<double> waveData;
//     uint len = m_wavFileHeader.fileDataSize/m_wavFileHeader.nBytesPerSample;
//     uint len = m_wavFileHeader.fileDataSize/17744;
     uint len = m_wavFileHeader.fileDataSize/1774;
     qDebug()<<"len="<<len;
     qDebug()<<"m_wavFileHeader.fileDataSize="<<m_wavFileHeader.fileDataSize;
     qDebug()<<"m_wavFileHeader.nBytesPerSample="<<m_wavFileHeader.nBytesPerSample;
     qDebug()<<__FUNCTION__<<pcmData.size()<<len;
     if(m_wavFileHeader.nBytesPerSample == 1)//8位
     {
         char *data = (char *)pcmData.data();
         for (uint i = 0; i < len; i++)
         {

//             qDebug()<<data[i]<<"1绘图的时候";
             waveData.append(data[i]);
             *lineSeries<<QPointF(i,data[i]);
             *lineSeries2<<QPointF(i,data[i]);
         }
     }
     else//16位
     {
         short *data = (short *)pcmData.data();
         for (uint i = 0; i < len; i++)
         {
//             qDebug()<<data[i]<<"2绘图的时候";
             waveData.append(data[i]);
             *lineSeries<<QPointF(i,data[i]);
             *lineSeries2<<QPointF(i,data[i]);
         }
     }


     //给chart设置主题，黑夜
//     ui->chartView->chart()->setTheme(QChart::ChartThemeDark);

     //序列的数据点标签是否可见
//     lineSeries->setPointLabelsVisible(true);
     //序列数据点标签的显示格式
//     lineSeries->setPointLabelsFormat("@xPoint");
     fileInfo.close();
     //序列的数据点是否可见
//     lineSeries->setPointsVisible(true);



}



void MainWindow::on_wav_PlayListIndexChanged(int index)
{
    qDebug()<<"on_wav_PlayListIndexChanged(int index)";
    ui->listWidget->setCurrentRow(index);//设置当前行为index
    QListWidgetItem *item = ui->listWidget->currentItem();




}


//鼠标控制事件

//滑轮事件
void MainWindow::wheelEvent(QWheelEvent *event){
    qDebug()<<"wheelEvent(QWheelEvent *event){";
    if (event->delta() > 0) {
        ui->chartView->chart()->zoom(1.2);
    } else if (event->delta() < 0) {
        ui->chartView->chart()->zoom(0.8);
    } /*else if(event==Qt::MidButton) {
        ui->chartView->chart()->zoomReset();
    }*/
}


//qint64 testTime;
//按键事件
void MainWindow::mousePressEvent(QMouseEvent *event){
    qDebug()<<"mousePressEvent(QMouseEvent *event){";
//    this->setMouseTracking(true);
//    ui->chartView->setMouseTracking(true);
//    ui->chartView->setAttribute(Qt::WA_TransparentForMouseEvents);


    qDebug()<<"鼠标按下"<<endl;
      if(event->button()==Qt::LeftButton)
      {
          qDebug()<<"左键按下"<<endl;
//          isFragmentPlay = false;//当鼠标左键按下的时候，截取片段结束
      }


      else if(event->button()==Qt::RightButton)
          qDebug()<<"右键按下"<<endl;
      else if(event->button()==Qt::MidButton)
          qDebug()<<"中键按下"<<endl;


      //通过变换求出鼠标在表格中点击的位置
      QPointF point;
      point = ui->chartView->mapToScene(event->pos());
      qDebug()<<"mapToScene："<<point;
      point = ui->chartView->chart()->mapFromScene(point);
      qDebug()<<"mapFromScene："<<point;
      point = ui->chartView->chart()->mapToValue(point);
      qDebug()<<"mapToValue："<<point;

      qDebug()<<"全局位置："<<event->globalPos();//全局位置
      qDebug()<<"windos窗口位置："<<event->windowPos();
      qDebug()<<"点的坐标"<<point<<endl;
      qDebug()<<"点的横坐标"<<point.x();

      //获取总时间，以秒的形式
      int duration = player->duration()/1000;
      //通过在同一个点上，播放两个音频，得到比例关系，求出应该减的时间
      int subTime = duration*(6.3186)/238 + 0.32*duration + 1;
      testTime = (qint64)(point.x() - subTime);

      //
      startTimeEdit = testTime;

      qDebug()<<"testTime="<<testTime;
      QString j = typeid(testTime).name();
      qDebug()<<j;
      qint64 testg = 65;
      QString g = typeid(testg).name();
      qDebug()<<g;
//      player->setPosition(testTime*1000);
//      on_positionChanged(testTime*1000);
//      ui->sliderPosition->valueChanged(testTime);
//      player->pause();
      ui->btnPlay->setEnabled(true);


      qDebug()<<"高为"<<this->frameGeometry().height();
      qDebug()<<"宽度为"<<this->frameGeometry().width();
      qDebug()<<"鼠标是否点击在charView内"<<ui->chartView->underMouse();

      if (event->button()==Qt::LeftButton && ui->chartView->underMouse())
      {
          isFragmentPlay = false;
          isFragment = true;

          //单击的时候，截取片段功能暂时结束


          startTimeEdit = testTime;
          int mins = testTime/60;
          int secs = testTime%60;
          //当前播放位置
          QString start = QString::asprintf("%d:%d",mins,secs);

          ui->tableWidget->item(0,0)->setText(start);
      }


      if (event->button()==Qt::RightButton && (ui->chartView->underMouse() || ui->chartView2->underMouse())) {
//          rightButtonMenu = new QMenu(this);
//          addNote = new QAction(QStringLiteral("添加注释"),rightButtonMenu);
          rightButtonMenu->addAction(addNote);
          rightButtonMenu->addAction(saveText);
          rightButtonMenu->addAction(renewText);
          rightButtonMenu->exec(event->pos());

      }

}


//释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    qDebug()<<"mouseReleaseEvent(QMouseEvent *event){";
    qDebug()<<"鼠标释放"<<endl;
    qDebug()<<event->globalPos()<<endl;
    qDebug()<<event->windowPos()<<endl;

    //设置鼠标跟随
//    ui->chartView->setMouseTracking(true);
//    this->setMouseTracking(true);


    if (isFragment)
    {
        //通过变换求出鼠标在表格中点击的位置
        QPointF point;
        point = ui->chartView->mapToScene(event->pos());
        qDebug()<<"mapToScene："<<point;
        point = ui->chartView->chart()->mapFromScene(point);
        qDebug()<<"mapFromScene："<<point;
        point = ui->chartView->chart()->mapToValue(point);
        qDebug()<<"mapToValue："<<point;



        qDebug()<<"全局位置："<<event->globalPos();//全局位置
        qDebug()<<"windos窗口位置："<<event->windowPos();
        qDebug()<<"点的坐标"<<point<<endl;
        qDebug()<<"点的横坐标"<<point.x();

        //获取总时间，以秒的形式
        int duration = player->duration()/1000;
        //通过在同一个点上，播放两个音频，得到比例关系，求出应该减的时间
        int subTime = duration*(6.3186)/238 + 0.32*duration + 1;
        testTime = (qint64)(point.x() - subTime);
        qDebug()<<"testTime="<<testTime;
        endTimeEdit = testTime;
        int mins = testTime/60;
        int secs = testTime%60;
        //当前播放位置
        QString end = QString::asprintf("%d:%d",mins,secs);

        ui->tableWidget->setItem(0,1,new QTableWidgetItem(end));

        durationTimeEdit = endTimeEdit - startTimeEdit;
        qDebug()<<"startTimeEdit="<<startTimeEdit<<endl;
        qDebug()<<"endTimeEdit"<<endTimeEdit<<endl;;
        mins = durationTimeEdit/60;
        secs = durationTimeEdit%60;
        QString durationEdit = QString::asprintf("%d:%d",mins,secs);

        ui->tableWidget->setItem(0,2,new QTableWidgetItem(durationEdit));

        //设置持续时间标签
        if (endTimeEdit != NULL && startTimeEdit != NULL && endTimeEdit > startTimeEdit)
        {


//            ui->tableWidget->item(0,2)->setText(durationEdit);
            ui->tableWidget->setItem(0,2,new QTableWidgetItem(durationEdit));


            //设置截取片段播放功能恢复
            isFragment=false;
            isFragmentPlay = true;
        }

    }
//    if (event->button()==Qt::LeftButton && ui->chartView->underMouse())
//    {


//        isFragment=false;
//    }


}
//移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event){
//    QPainter p(ui->chartView);
//    p.setRenderHint(QPainter::HighQualityAntialiasing);//抗锯齿以平滑
//    p.setRenderHint(QPainter::Antialiasing);
//    p.setRenderHint(QPainter::TextAntialiasing);//抗锯齿以平滑
//    p.setPen(QColor(170,255,127,150));//绿色半透明边框
//    int xLen = ui->chartView->width();
//    int yLen = ui->chartView->height();
//    p.drawLine(QPoint(xLen / 2,0),QPoint(xLen / 2, yLen));
}
//双击事件
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event){
    qDebug()<<"mouseDoubleClickEvent(QMouseEvent *event){";
    qDebug()<<"鼠标双击"<<endl;
    qDebug()<<event->globalPos()<<endl;
    qDebug()<<event->windowPos()<<endl;
    ui->chartView->chart()->zoomReset();

    //截取功能全部失效
    if (event->button()== Qt::RightButton & ui->chartView->underMouse())
    {
        isFragment = false;
        isFragmentPlay = false;
    }

    qDebug()<<"右键双击后,isFragment="<<isFragment<<",isFragmentPlay="<<isFragmentPlay;
}



//void MainWindow::on_btnAddNote_clicked()
//{
//    qDebug()<<"on_btnAddNote_clicked()";



//    if (isFragment)
//    {
//        int i = 0;
//        QString name = "名称";
//        int row_count = ui->tableWidgetNote->rowCount(); //获取表单行数

//        ui->tableWidgetNote->insertRow(row_count);//插入新行
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(name));
//        int mins = startTimeEdit/60;
//        int secs = startTimeEdit%60;
//        QString start = QString::asprintf("%d:%d",mins,secs);
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem("0:00"));
//        ui->tableWidgetNote->setItem(noteCount,5,new QTableWidgetItem(QString::asprintf("%d",startTimeEdit)));


//        //给tableWidgetNote的类型那一列添加复选框
//        //添加下拉项的内容
//        QStringList m_strList;
//        m_strList << "提示" << "子剪辑" << "CD 音轨" << "录放音计时器";

//        //添加下拉控件
//        QComboBox *m_comboBox = new QComboBox();
//        m_comboBox->addItems(m_strList);

//        //下拉控件放到tableWidgetNote
//        ui->tableWidgetNote->setCellWidget(noteCount,4,m_comboBox);

//        noteCount++;
//    }
//    else if(isFragmentPlay)
//    {
//        int i = 0;
//        QString name = "名称";
//        int row_count = ui->tableWidgetNote->rowCount(); //获取表单行数

//        ui->tableWidgetNote->insertRow(row_count);//插入新行
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(name));

//        int mins = startTimeEdit/60;
//        int secs = startTimeEdit%60;
//        QString start = QString::asprintf("%d:%d",mins,secs);
//        mins = endTimeEdit/60;
//        secs = endTimeEdit%60;
//        QString end = QString::asprintf("%d:%d",mins,secs);
//        mins = durationTimeEdit/60;
//        secs = durationTimeEdit%60;
//        QString duration = QString::asprintf("%d:%d",mins,secs);
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(end));
//        ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(duration));
//        ui->tableWidgetNote->setItem(noteCount,5,new QTableWidgetItem(QString::asprintf("%d",startTimeEdit)));

//        //给tableWidgetNote的类型那一列添加复选框
//        //添加下拉项的内容
//        QStringList m_strList;
//        m_strList << "提示" << "子剪辑" << "CD 音轨" << "录放音计时器";

//        //添加下拉控件
//        QComboBox *m_comboBox = new QComboBox();
//        m_comboBox->addItems(m_strList);

//        //下拉控件放到tableWidgetNote
//        ui->tableWidgetNote->setCellWidget(noteCount,4,m_comboBox);

//        noteCount++;
//    }
//}




void MainWindow::on_tableWidgetNote_doubleClicked(const QModelIndex &index)
{
    qDebug()<<"on_tableWidgetNote_doubleClicked(const QModelIndex &index)";
    qDebug()<<"双击了tableWidgetNote";
//    qDebug()<<index;
//    qDebug()<<index.column();
//    qDebug()<<index.row();
    isFragment = true;
//    startTimeEdit =
//    qDebug()<<ui->tableWidgetNote->item(index.row(),1);
    QString s = ui->tableWidgetNote->item(index.row(),5)->text();
    bool ok;
    int temp = s.toInt(&ok,10);
    qDebug()<< temp;

    startTimeEdit = temp;
}

//

void MainWindow::on_btnGet_clicked()
{

//    //告诉程序我开始运行这个功能了
//    qDebug()<<"on_btnGet_clicked()";


//    //设置打开文件夹
//    QString curPath = "D:/workSpace/Project/music";
//    //设置窗口标题
//    QString dlgTitle = "选择scp文件";
//    //设置过滤器
//    QString filter = "scp文件(*.scp);;"
//                     "所有文件(*.*)";
//    QString file = QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

//    QFileInfo fileInfo(file);

//    qDebug()<<fileInfo.fileName();//"wav.scp"

////    QFile afile(fileInfo.fileName());
//    QFile afile(file);
//    if (! afile.open(QIODevice::ReadOnly|QIODevice::Text))
//        qDebug()<<afile.errorString();
//    else
//        qDebug()<<"openok";
////    afile.seek();//好像是自动填充，扩充大小功能

//    QTextStream shuru(&afile);
////    in->setCodec("UTF-8");
//    shuru.setCodec("UTF-8");

//    //提前设置好变量
//    QString prefix;
//    QString line;
//    QStringList list;

//    while (! shuru.atEnd())
//    {
//        prefix = "http://101.35.48.220:10109";
//        line = shuru.readLine();
//        list = line.split(QRegExp("\\s+"));
////        list[1] = prefix + list[1];
////        pos = line.indexOf(" ");
//        list[1] = prefix + line.mid(line.indexOf("/home/mgd519/resources/uyghur_data/RY-UG-4")+42);
//        qDebug()<<list[1];
//        mapNameAndAddress.insert(list[0],list[1]);

//        ui->listWidget_2->addItem(list[0]);


//    }


//    afile.close();



    //=========================================================4.13之前的，现在重新写
//    //去掉多余空格
//    QString urlSpec = ui->URLEdit->text().trimmed();

//    //判断是否为空
//    if (urlSpec.isEmpty())
//    {
//        QMessageBox::information(this,"错误","请指定需要下载的URL");
//        return;
//    }

//    //判断URL地址是否正确
//    QUrl newUrl = QUrl::fromUserInput(urlSpec);
//    if (!newUrl.isValid())
//    {
//        QMessageBox::information(this,"错误",
//                                 QString("无效URL：%1 \n 错误信息：%2").arg(urlSpec,newUrl.errorString()));
//        return;
//    }

//    //发布网络请求，请求下载URL地址表示的文件，并创建网络响应
//    reply = networkManager.get(QNetworkRequest(newUrl));
//    playlist->addMedia(QNetworkRequest(newUrl));
//    QFile *testFile;//测试QFile类，用来临时保存文件
//    testFile->write(reply->readAll());
//    ui->listWidget->addItem(testFile->fileName());
}



void MainWindow::on_listWidget_2_doubleClicked(const QModelIndex &index)
{
    qDebug()<<"on_listWidget_2_doubleClicked(const QModelIndex &index)";

    //=========================更新字典和生成文本这个功能在文本框右键实现了，所以这里注释掉了

//    //切换目录的时候，询问是否进行修改保存
//    //一级弹窗
////    if (ui->plainTextEdit)
//    QString dlgTitle="Question消息框";
//    QString strInfo="当前文件已经被切换，是否保存修改？";
//    QMessageBox::StandardButton  defaultBtn=QMessageBox::NoButton; //缺省按钮
//    QMessageBox::StandardButton result;//返回选择的按钮
//    QMessageBox saveDictionary;
//    saveDictionary.setText("保存到字典");
//    QMessageBox saveFile;
//    saveFile.setText("保存到文件");
//    result=QMessageBox::question(this, dlgTitle, strInfo,
//                      QMessageBox::Yes|QMessageBox::No |QMessageBox::YesAll,
//                      defaultBtn);
////    result=QMessageBox::question(this, dlgTitle, strInfo,
////                      saveDictionary|QMessageBox::No|saveFile,
////                      defaultBtn);
//    //二级弹窗
//    if (result==QMessageBox::Yes && !ui->plainTextEdit->toPlainText().isEmpty())
//    {
//        //更新字典值
//        qDebug() << ui->plainTextEdit->toPlainText();
//        mapNameAndText[index.data().toString()] = ui->plainTextEdit->toPlainText();
//    }
//    else if (result==QMessageBox::YesAll)
//    {
//        //对话框，选择名字，选择保存路径，选择文件类型
//        QString curPath=QDir::currentPath();//获取系统当前目录
//        QString dlgTitle="另存为一个文件"; //对话框标题
//        QString filter="txt文件(*.txt);;scp文件(*.scp);;所有文件(*.*)"; //文件过滤器
//        QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
//        if (aFileName.isEmpty())
//            return;
//        //用QTextStream保存文本文件
//        QFile aFile(aFileName);
//        if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
//            qDebug()<<aFile.errorString();
//        QTextStream aStream(&aFile);
//        aStream.setCodec("UTF-8");
//        QMap<QString, QString>::const_iterator it = mapNameAndText.constBegin();
//        //遍历
//        while (it != mapNameAndText.constEnd())
//        {
//            //获取一对键值对
//            QString str = it.key() + " " + it.value() + "\n";
////            str.setUnicode("UTF-8");

//            qDebug()<<str;
//            //写入文本流
//            aStream<<str;
////             qDebug() << it.key() << " " << it.value() << endl;
//             ++it;
//        }
//    }


//    else if(result==QMessageBox::No)

//    else if(result==QMessageBox::Cancel)

//    else


    //获取双击选择的行数
    int rowNo = index.row();
    //这是文本值
    qDebug()<<"文本列表值"<<index.data().toString();

    //设置到文本框里面
    ui->plainTextEdit->setPlainText(mapNameAndText.value(index.data().toString()));

    //获取下载地址，在mapNameAndAddress中已经修改为正确的下载地址
    QString getURL = mapNameAndAddress.value(index.data().toString());
    qDebug()<<"下载地址URL"<<getURL;

    //判断是否为空
    if (getURL.isEmpty())
    {
        QMessageBox::information(this,"错误","请指定需要下载的URL");
        return;
    }

    //判断URL地址是否正确
    QUrl newGetURL = QUrl::fromUserInput(getURL);
    if (!newGetURL.isValid())
    {
        QMessageBox::information(this,"错误",
                                 QString("无效URL：%1 \n 错误信息：%2").arg(getURL,newGetURL.errorString()));
        return;
    }

    //测试，存储在内存中,
    //目前，能将读取到的数据存储在一个QString中
//    playlist->addMedia(newGetURL);
    downloadedFile = new QFile("test999.wav");
//    downloadedFile->open(QIODevice::Truncate | QIODevice::WriteOnly);
   if (!downloadedFile->open(QIODevice::WriteOnly))
   {
       QMessageBox::information(this,"错误","临时文件打开错误");
       return;
   }
    //发布网络请求，请求下载URL地址表示的文件，并创建网络响应
    getReply = networkManager.get(QNetworkRequest(newGetURL));


    //让响应冷却，以便下次继续访问
    connect(getReply,SIGNAL(finished()),this,SLOT(on_finished()));

    connect(getReply,SIGNAL(readyRead()),this,SLOT(on_readyRead()));



    //功能完整，双击就去下载到本地
//    QString tempDir = "D:\\workSpace\\Project\\music\\";
//   //检查目录中是否有重命名的文件
//   QString fullFileName = tempDir + newGetURL.fileName();
//   qDebug()<<fullFileName;
//   if (QFile::exists(fullFileName))
//       QFile::remove(fullFileName);

//   downloadedFile = new QFile(fullFileName);
//   if (!downloadedFile->open(QIODevice::WriteOnly))
//   {
//       QMessageBox::information(this,"错误","临时文件打开错误");
//       return;
//   }

//   //发布网络请求，请求下载URL地址表示的文件，并创建网络响应
//   getReply = networkManager.get(QNetworkRequest(newGetURL));
//   //让响应冷却，以便下次继续访问
//   connect(getReply,SIGNAL(finished()),this,SLOT(on_finished()));

//   connect(getReply,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

}

//void MainWindow::on_btnMicro_clicked()
//{
//    //打开一个新窗口
////    micro_page = new micro;

////    micro_page->show();
//    mic_page = new mic;
//    mic_page->show();
//}


void MainWindow::paintEvent(QPaintEvent *event)
{
    //实例化
    QPainter painter(this);//相当于QPainter *painter = new QPainter();
    //设置渲染模式，使得平滑渲染
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(0,0,100,100);

}


//===============下载功能======================
//进行返回状态判断操作，资源关闭等操作
void MainWindow::on_finished()
{
    qDebug()<<"on_finished()";
//    wavBuffer.open(QBuffer::ReadWrite);
//    wavBuffer.close();
//    qDebug()<<"wavBuffer.data()================="<<wavByteArray.data();
    qDebug()<<"wavBuffer.buffer()================="<<wavBuffer.buffer();
    qDebug()<<"wavByteArray======================="<<wavByteArray;
//    wavBuffer.close();
    wavBuffer.open(QBuffer::ReadWrite);
    //网络相应结束
    QFileInfo fileInfo;
    fileInfo.setFile(downloadedFile->fileName());

    //删除，关闭
    downloadedFile->close();
    delete downloadedFile;
    downloadedFile = Q_NULLPTR;

    getReply->deleteLater();
    getReply = Q_NULLPTR;

    //以缺省方式打开
//    if (ui->checkOpen->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));

//    ui->btnDownload->setEnabled(true);
}

//主要是进行网络资源的保存操作
void MainWindow::on_readyRead()
{
     qDebug()<<"on_readyRead()";
//     wavBuffer.open(QBuffer::WriteOnly);
    //读取下载的数据
//    downloadedFile->write(getReply->readAll());//有用的

     //=============
    QByteArray tempArray = getReply->readAll();
    qDebug()<<sizeof(tempArray);
//    char *pArr = tempArray.data();
//    qDebug()<<pArr;
    QString str = QString::fromUtf8(tempArray);

    wavBuffer.write(tempArray);



//     wavBuffer.write("123");
//     wavBuffer.write("==========123");
//     wavBuffer.close();
     qDebug()<<"wavBuffer.data()================="<<wavByteArray.data();
     qDebug()<<"wavBuffer.buffer()================="<<wavBuffer.buffer();

}


//右键菜单栏注释功能
void MainWindow::on_addNote_triggered(bool checked)
{
    qDebug()<<"on_addNote_triggered(bool checked)";
//    if (checked == true)
//    {
        if (isFragment)
        {
            int i = 0;
            QString name = "名称";
            int row_count = ui->tableWidgetNote->rowCount(); //获取表单行数

            ui->tableWidgetNote->insertRow(row_count);//插入新行
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(name));
            int mins = startTimeEdit/60;
            int secs = startTimeEdit%60;
            QString start = QString::asprintf("%d:%d",mins,secs);
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem("0:00"));
            ui->tableWidgetNote->setItem(noteCount,5,new QTableWidgetItem(QString::asprintf("%d",startTimeEdit)));


            //给tableWidgetNote的类型那一列添加复选框
            //添加下拉项的内容
            QStringList m_strList;
            m_strList << "提示" << "子剪辑" << "CD 音轨" << "录放音计时器";

            //添加下拉控件
            QComboBox *m_comboBox = new QComboBox();
            m_comboBox->addItems(m_strList);

            //下拉控件放到tableWidgetNote
            ui->tableWidgetNote->setCellWidget(noteCount,4,m_comboBox);

            noteCount++;
        }
        else if(isFragmentPlay)
        {
            int i = 0;
            QString name = "名称";
            int row_count = ui->tableWidgetNote->rowCount(); //获取表单行数

            ui->tableWidgetNote->insertRow(row_count);//插入新行
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(name));

            int mins = startTimeEdit/60;
            int secs = startTimeEdit%60;
            QString start = QString::asprintf("%d:%d",mins,secs);
            mins = endTimeEdit/60;
            secs = endTimeEdit%60;
            QString end = QString::asprintf("%d:%d",mins,secs);
            mins = durationTimeEdit/60;
            secs = durationTimeEdit%60;
            QString duration = QString::asprintf("%d:%d",mins,secs);
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(start));
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(end));
            ui->tableWidgetNote->setItem(noteCount,i++,new QTableWidgetItem(duration));
            ui->tableWidgetNote->setItem(noteCount,5,new QTableWidgetItem(QString::asprintf("%d",startTimeEdit)));

            //给tableWidgetNote的类型那一列添加复选框
            //添加下拉项的内容
            QStringList m_strList;
            m_strList << "提示" << "子剪辑" << "CD 音轨" << "录放音计时器";

            //添加下拉控件
            QComboBox *m_comboBox = new QComboBox();
            m_comboBox->addItems(m_strList);

            //下拉控件放到tableWidgetNote
            ui->tableWidgetNote->setCellWidget(noteCount,4,m_comboBox);

            noteCount++;
        }
//    }

}


//7.4       将整个字典文件保存文本文件
void MainWindow::on_saveText_triggered(bool checked)
{
    //对话框，选择名字，选择保存路径，选择文件类型
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="另存为一个文件"; //对话框标题
    QString filter="txt文件(*.txt);;scp文件(*.scp);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
    //用QTextStream保存文本文件
    QFile aFile(aFileName);
    if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug()<<aFile.errorString();
    QTextStream aStream(&aFile);
    aStream.setCodec("UTF-8");
    QMap<QString, QString>::const_iterator it = mapNameAndText.constBegin();
    //遍历
    while (it != mapNameAndText.constEnd())
    {
        //获取一对键值对
        QString str = it.key() + " " + it.value() + "\n";
//            str.setUnicode("UTF-8");

        qDebug()<<str;
        //写入文本流
        aStream<<str;
//             qDebug() << it.key() << " " << it.value() << endl;
         ++it;
    }
}

//更新字典中的键值对
void MainWindow::on_renewText_triggered(bool)
{
    mapNameAndText[ui->listWidget_2->currentIndex().data().toString()] = ui->plainTextEdit->toPlainText();
//    ui->listWidget_2->currentIndex().data()
}


