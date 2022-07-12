#include "download.h"
#include "ui_download.h"

#include <QDir>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include "mainwindow.h"

download::download(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::download)
{
    ui->setupUi(this);
}

download::~download()
{
    delete ui;
}

//进行返回状态判断操作，资源关闭等操作
void download::on_finished()
{
    qDebug()<<"on_finished()";
    //网络相应结束
    QFileInfo fileInfo;
    fileInfo.setFile(downloadedFile->fileName());

    //删除，关闭
    downloadedFile->close();
    delete downloadedFile;
    downloadedFile = Q_NULLPTR;

    reply->deleteLater();
    reply = Q_NULLPTR;

    //以缺省方式打开
    if (ui->checkOpen->isChecked())
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));

    ui->btnDownload->setEnabled(true);
}

//主要是进行网络资源的保存操作
void download::on_readyRead()
{
     qDebug()<<"on_readyRead()";
    //读取下载的数据
    downloadedFile->write(reply->readAll());
//    testFile->write(reply->readAll());
//    MainWindow::playlist->addMedia(testFile);
//    QFileInfo fileInfo(testFile);
//    MainWindow::ui->listWidget->addItem(fileInfo.fileName());
}

void download::on_downloadProgress(qint64 bytesRead, qint64 totalBytes)
{
     qDebug()<<"on_downloadProgress(qint64 bytesRead, qint64 totalBytes)";
    //下载进程  设置进度条
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}





void download::on_btnDefaultPath_clicked()
{
     qDebug()<<"on_btnDefaultPath_clicked()";
    //缺省路径 按钮
    ui->editPath->setText("D:/workSpace/Project/music/");

    //另一种，在当前文件下创建temp目录
//    QString curPath = QDir::currentPath();
//    QDir dir(curPath);
//    QString sub = "temp";
//    dir.mkdir(sub);//在dir这个路径中，创建sub文件夹
//    ui->editPath->setText(curPath+"/"+sub+"/");//设置路径
}

void download::on_btnDownload_clicked()
{
     qDebug()<<"on_btnDownload_clicked()";
    //下载按钮
    //去掉多余空格
    QString urlSpec = ui->editURL->text().trimmed();

    //判断是否为空
    if (urlSpec.isEmpty())
    {
        QMessageBox::information(this,"错误","请指定需要下载的URL");
        return;
    }

    //判断URL地址是否正确
    QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        QMessageBox::information(this,"错误",
                                 QString("无效URL：%1 \n 错误信息：%2").arg(urlSpec,newUrl.errorString()));
        return;
    }

    //判断下载的地址是否正确
    QString tempDir = ui->editPath->text().trimmed();
    if (tempDir.isEmpty())
    {
        QMessageBox::information(this,"错误","请指定保存下载文件的目录");
        return;
    }

    //检查目录中是否有重命名的文件
    QString fullFileName = tempDir + newUrl.fileName();
    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    downloadedFile = new QFile(fullFileName);
    if (!downloadedFile->open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this,"错误","临时文件打开错误");
        return;
    }

    ui->btnDownload->setEnabled(false);
    //发布网络请求，请求下载URL地址表示的文件，并创建网络响应
    reply = networkManager.get(QNetworkRequest(newUrl));
    //让响应冷却，以便下次继续访问
    connect(reply,SIGNAL(finished()),this,SLOT(on_finished()));

    connect(reply,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    //显示下载量
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),
            this,SLOT(on_downloadProgress(qint64,qint64)));

}


void download::on_editURL_textChanged(const QString &arg1)
{
     qDebug()<<"on_editURL_textChanged(const QString &arg1)";
    //当URL地址改变时，改变进度条
    Q_UNUSED(arg1);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
}
