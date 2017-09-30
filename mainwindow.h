#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTextCodec>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void replyFinished(QNetworkReply *reply);
    void keyPressEvent(QKeyEvent *event);

    void on_pushButton_clicked();


    void on_timeCheck_clicked();

    void on_transCheck_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QString path;
    void set_get_request(QString url);
    void Get_Data(QString string);
    bool first;
    QString raw;//原歌词
    QString raw_;//无时间轴
    QString trans;//翻译
    QString trans_;//无时间轴
    QString songID;//歌曲id
    QString singer;//歌手
    QString song;//歌名
};

#endif // MAINWINDOW_H
