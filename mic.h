#ifndef MIC_H
#define MIC_H

#include <QMainWindow>
#include <QtCharts>
#include <QAudioRecorder>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class XYSeriesIODevice;

QT_BEGIN_NAMESPACE
class QAudioInput;
QT_END_NAMESPACE


namespace Ui {
class mic;
}

class mic : public QMainWindow
{
    Q_OBJECT

public:
    explicit mic(QWidget *parent = 0);
    ~mic();

private slots:
    void on_btnStart_clicked();

    void on_btnEnd_clicked();

    void on_btnChoose_clicked();

    void updateProgress(qint64 duration);

private:
    QAudioRecorder *audioRecorder;
    XYSeriesIODevice *mic_device;
    QChart *mic_chart;
    QLineSeries *mic_series;
    QAudioInput *mic_audioInput;
    Ui::mic *ui;
};

#endif // MIC_H
