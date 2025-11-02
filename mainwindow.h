#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QDropEvent>
#include <QThreadPool>

#include "downloadmanager.h"
#include "scaleimage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dropEvent(QDropEvent *event) override;

private:
    //Виджеты
    QListWidget *urlList;
    QLineEdit *urlInputLine;
    QPushButton *addUrlBtl;
    QPushButton *downloadBtn;
    QListWidget *progressList;

    //Объекты вспомогательных классов
    DownloadManager *dManager;

    void setUpUI();

private slots:
    void onAddUrlBtnClicked();
    void checkAddUrlBtn(QString text);
    void onDownloadBtnClicked();
    void onUpdateProgress(const QUrl &url, int);
    void onDownloadFinished(const QUrl &u, QByteArray);


};
#endif // MAINWINDOW_H
