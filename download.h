#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
//#include "mainwindow.h"



namespace Ui {
class download;
}

class download : public QMainWindow
{
    Q_OBJECT

private:
    QNetworkAccessManager networkManager;//网络管理
    QNetworkReply *reply;//网络响应
    QFile *downloadedFile;//下载保存的临时文件
    QFile *testFile;//测试QFile类，用来临时保存文件

public:
    explicit download(QWidget *parent = 0);
    ~download();

private slots:
    //自定义槽函数
    void on_readyRead();
    void on_downloadProgress(qint64 bytesRead, qint64 totalBytes);
    void on_finished();
    void on_btnDefaultPath_clicked();
    void on_btnDownload_clicked();

    void on_editURL_textChanged(const QString &arg1);

private:
    Ui::download *ui;
};

#endif // DOWNLOAD_H
