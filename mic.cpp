#include "mic.h"
#include "ui_mic.h"
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>
#include "xyseriesiodevice.h"

mic::mic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mic)
{
    ui->setupUi(this);

    //获取当前你有多少种输入设备
    audioRecorder = new QAudioRecorder;
    QStringList inputs = audioRecorder->audioInputs();
    ui->comboBox->addItems(inputs);


    /* 显示录制时间*/
    connect(audioRecorder,&QAudioRecorder::durationChanged,this,&mic::updateProgress);

    mic_chart = new QChart;
    ui->chartView->setChart(mic_chart);
    ui->chartView->setMinimumSize(800,600);
    mic_series = new QLineSeries;
    mic_chart->addSeries(mic_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 2000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setTitleText("Audio level");
    mic_chart->setAxisX(axisX, mic_series);
    mic_chart->setAxisY(axisY, mic_series);
    mic_chart->legend()->hide();
    mic_chart->setTitle("Data from the microphone");

    //设置采样格式
    QAudioFormat formatAudio;
    //设置采样率
    formatAudio.setSampleRate(44100);
    //设置通道数
    formatAudio.setChannelCount(1);
    //设置采样大小，一般为8位或16位
    formatAudio.setSampleSize(8);


    //设置编码方式
    formatAudio.setCodec("audio/pcm");
    //设置字节序
    formatAudio.setByteOrder(QAudioFormat::LittleEndian);
    //设置样本数据类型
    formatAudio.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo inputDevices = QAudioDeviceInfo::defaultInputDevice();
    mic_audioInput = new QAudioInput(inputDevices,formatAudio, this);

    mic_device = new XYSeriesIODevice(mic_series, this);
    mic_device->open(QIODevice::WriteOnly);

    mic_audioInput->start(mic_device);

}

mic::~mic()
{

    mic_audioInput->stop();
    mic_device->close();
    delete ui;
}

void mic::on_btnStart_clicked()
{
    // Set recording Settings
    QAudioEncoderSettings settings;
    settings.setChannelCount(2);


    settings.setCodec("audio/amr");     // Not sure what to put here
    settings.setSampleRate(16000);
    settings.setBitRate(32);

    qDebug()<<"settings.bitRate()="<<settings.bitRate();
//    settings.bitRate(17744);

//    settings.setQuality(QMultimedia::HighQuality);
//    settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    audioRecorder->setEncodingSettings(settings);


    audioRecorder->setAudioInput(ui->comboBox->currentText());
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(ui->editSaveAddress->text()));
    audioRecorder->record();

}

void mic::on_btnEnd_clicked()
{
    audioRecorder->stop();
}

void mic::on_btnChoose_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
                this,tr("保存文件"),"D://workSpace//Project//music",tr("wav文件 (*.wav)"));
    ui->editSaveAddress->setText(filename);

}

void mic::updateProgress(qint64 duration)
{
    if(audioRecorder->error() != QMediaRecorder::NoError || duration < 1000)
        return;
    setWindowTitle(tr("Recorded %1 sec").arg(duration / 1000));

}
