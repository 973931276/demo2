#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define testTime
//extern qint64 testTime;
#include <download.h>
#include <mic.h>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlayList>
#include    <QtCharts>
#include "wavefile.h"
#include <QtWidgets/QGraphicsView>
#include <QPaintDevice>


//#include "download.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
//class MainWindow: public QGraphicsView
{
    Q_OBJECT

private:
    //下载内容到内存
    QNetworkAccessManager networkManager;//网络管理
    QNetworkReply *reply;//网络响应
    QNetworkReply *getReply;//获取按钮，实时的访问服务器

    //wav
    QLineSeries *lineSeries;//曲线序列
    QLineSeries *lineSeries2;//第二条曲线序列
    WaveFile m_Wavefile;//wav文件信息

    QGraphicsScene *scene;

    //4.14  下载功能
    QFile *downloadedFile;

    //4.17鼠标右键菜单项
    QMenu *rightButtonMenu;
    QAction *addNote;

    //7.4鼠标右键菜单项
//    QMenu *rightButtonMenu_text;不需要  在rightButtonMenu中添加动作就可以
    QAction *saveText;
    QAction *renewText;

    qint64 testTime;
    int startTimeEdit;//截取片段开始的时间（以秒为单位）
    int endTimeEdit;//截取片段结束的时间（以秒为单位）
    int durationTimeEdit;//截取片段的持续时间（以秒为单位）
    bool isFragmentPlay = false;//设置截取一段的时候，循环播放
    bool isFragment = false;//设置点击列表的时候
    //坐标轴设置成全局变量
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    QValueAxis *axisX2 = new QValueAxis;
    QValueAxis *axisY2 = new QValueAxis;
    //QCategoryAxis *isX = new QCategoryAxis;
    int noteCount = 0;
    QMap<QString,QString> mapNameAndAddress;
    QMap<QString,QString> mapNameAndText;


    //7.5将数据保存到内存中
    QByteArray wavByteArray;//创建QBuffer的时候会自动创建一个QByteArray，但这里还是声明一下
    QBuffer wavBuffer;


    //当前选区
    QRect selection;





public:

    void paintEvent(QPaintEvent *event);


    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QString durationTime;//总时长
    QString positionTime;//当前进度

    download *download_page;//下载页面，新窗口
    mic *mic_page;//麦克风页面，新窗口
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //将ui界面改为共同使用的
    // 2、定义一个静态的ui 定义一个窗口类 ui在自定义窗口类myui下(为了callout能使用MainWindow的ui界面）
//    static MainWindow *myMainWindow;
    //1、将ui公有化(为了callout能使用MainWindow的ui界面）
    Ui::MainWindow *ui;

    void setWavFile(QString strFileName);

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);


private:
//    Ui::MainWindow *ui;

    void prepareDate();
    void drawSingleChannel();
    void drawDoubleChannel(QPainter &p, int W, int H);
private slots:
    void on_stateChanged(QMediaPlayer::State state);
    void on_positionChanged(qint64 positionTime);
    void on_durationChanged(qint64 duration);
    void on_PlayListIndexChanged(int index);

    void on_btnAdd_clicked();
    void on_btnRemove_clicked();
    void on_btnClear_clicked();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_btnPlay_clicked();
    void on_btnPause_clicked();
    void on_btnStop_clicked();
    void on_btnPrevious_clicked();
    void on_btnNext_clicked();
    void on_sliderVolumn_valueChanged(int value);
    void on_sliderPosition_valueChanged(int value);
    void on_btnSound_clicked();
//    void on_btnDownload_clicked();

    //wav
    void on_wav_durationChanged(qint64 duration);
    void on_wav_PlayListIndexChanged(int index);

//    void on_btnAddNote_clicked();添加到右键菜单栏了，可以去掉了
    void on_tableWidgetNote_doubleClicked(const QModelIndex &index);




    void on_btnGet_clicked();
//    void on_btnMicro_clicked();
    void on_listWidget_2_doubleClicked(const QModelIndex &index);

    //下载功能
    void on_readyRead();
    void on_finished();

    //鼠标右键功能
    void on_addNote_triggered(bool checked);
    void on_saveText_triggered(bool checked);
    void on_renewText_triggered(bool);
};




#endif // MAINWINDOW_H
