#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    first = true;
//    manager->get(QNetworkRequest(QUrl("http://music.163.com/api/song/lyric?os=pc&id=488256319&lv=-1&kv=-1&tv=-1")));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::replyFinished(QNetworkReply *reply){
    if (reply->error() == QNetworkReply::NoError){
        QByteArray bytes = reply->readAll();
        QString string = QString::fromUtf8(bytes);
        Get_Data(string);
    } else {
        first = true;
        ui->Lrc->setText("handle errors here");
//        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//        //statusCodeV是HTTP服务器的相应码，reply->error()是Qt定义的错误码，可以参考QT的文档
//        qDebug( "found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
//        qDebug(qPrintable(reply->errorString()));
    }
    if(!first){
        QString s = QString("http://music.163.com/api/song/lyric?os=pc&id=%1&lv=-1&kv=-1&tv=-1").arg(songID);
//        qDebug()<<s;
        set_get_request(s);
    }
    reply->deleteLater();
}

void MainWindow::Get_Data(QString string){
    if(first){
        QRegExp rx;
        rx.setMinimal(true);
        rx.setPattern("data-res-name=\"(.*)\"");
        int pos = string.indexOf(rx);
        if ( pos >= 0 ){
            song = rx.cap(1);
            ui->songNameEdit->setText(song);
        }
        rx.setPattern("data-res-author=\"(.*)\"");
        pos = string.indexOf(rx);
        if ( pos >= 0 ){
            singer = rx.cap(1);
            ui->singerEdit->setText(singer);
        }
        first = false;
    }
    else{
        first = true;
        //QString => QJsonDocument
        QJsonDocument jsonDocument = QJsonDocument::fromJson(string.toUtf8().data());
        if(jsonDocument.isNull()){
            qDebug()<< "===> please check the string "<< string.toUtf8().data();
            return;
        }
        QJsonObject jsonObject = jsonDocument.object();
        // get the response object
        QJsonValue lrc = jsonObject.value("lrc");
        QJsonObject lrcObj = lrc.toObject();
        QJsonValue lyric = lrcObj.value("lyric");
//        qDebug()<<lyric;
        if(lyric.isUndefined()){
            ui->Lrc->setText("无歌词...");
            return;
        }
        QRegExp rx("\\[\\d.*\\]");
        rx.setMinimal(true);
        raw_ = raw = lyric.toString();
        ui->Lrc->setText(raw);
        raw_.replace(rx,"");
        //翻译
        if(jsonObject.find("tlyric") != jsonObject.end()){
            lrc = jsonObject.value("tlyric");
            lrcObj = lrc.toObject();
            lyric = lrcObj.value("lyric");
//            qDebug()<<lyric;
            if(lyric.isNull()){
                trans_ = trans = "";
                return;
            }
            trans_ = trans = lyric.toString();
            ui->Lrc->setText(raw + trans);
            trans_.replace(rx, "");
        }
    }
}


void MainWindow::set_get_request(QString url){
    QNetworkRequest *request = new QNetworkRequest();
    request->setUrl(QUrl(url));
    request->setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.104 Safari/537.36");
    manager->get(*request);
}


void MainWindow::on_pushButton_clicked(){
    //GET
    if(ui->songUrlEdit->text().isEmpty()){
        return;
    }
    QRegExp rx("(\\d+\\d+\\d+\\d+)");
    int pos = ui->songUrlEdit->text().indexOf(rx);
    if ( pos >= 0 ){
        songID = rx.cap(1);
//        qDebug()<<songID;
        QString s = QString("http://music.163.com/song?id=%1").arg(songID);
        set_get_request(s);
    }
}

//键盘事件 ctrl+s
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier){
        if(event->key() == Qt::Key_S){
            if(singer.isEmpty() || song.isEmpty()) return;
//            qDebug()<<singer<<song;
            QRegExp rx("/");
            int pos = singer.indexOf(rx);
            while(pos >= 0) {
                singer.replace(pos, 1, '&');
                pos = singer.indexOf(rx);
//                qDebug()<<pos;
            }
            path = QFileDialog::getSaveFileName(this, "Sava File", song+" - "+singer+".lrc", "*.lrc");
            if(path.isEmpty()){
                return;
            }
            QFile outFile(path);
            if (!outFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
                QMessageBox::critical(this, "警告", "无法创建文件");
                return;
            }
            QTextStream outTS(&outFile);
            outTS.setCodec(QTextCodec::codecForName("UTF-8"));
            outTS<<ui->Lrc->toPlainText().toUtf8();
            outTS.setGenerateByteOrderMark(true);
            outTS.flush();
            outFile.close();
        }

    }
}

void MainWindow::on_timeCheck_clicked(){
    if(raw.isEmpty()){
        return;
    }
    if(ui->timeCheck->isChecked()){
        if(ui->transCheck->isChecked()){
            ui->Lrc->setText(raw + trans);
        }else{
            ui->Lrc->setText(raw);
        }
    }else{
        if(ui->transCheck->isChecked()){
            ui->Lrc->setText(raw_ + trans_);
        }else{
            ui->Lrc->setText(raw_);
        }
    }

}

void MainWindow::on_transCheck_clicked(){
    if(trans.isEmpty()){
        return;
    }
    if(ui->timeCheck->isChecked()){
        if(ui->transCheck->isChecked()){
            ui->Lrc->setText(raw + trans);
        }else{
            ui->Lrc->setText(raw);
        }
    }else{
        if(ui->transCheck->isChecked()){
            ui->Lrc->setText(raw_ + trans_);
        }else{
            ui->Lrc->setText(raw_);
        }
    }
}
