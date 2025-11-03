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
#include <QTableWidget>
#include <QHeaderView>
#include <QHash>

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
    QPushButton *addUrlBtn;
    QPushButton *delUrlBtn;
    QPushButton *downloadBtn;
    QTableWidget *progressTable;

    //Объекты вспомогательных классов
    DownloadManager *dManager;

    QHash<QUrl, int> hashUrl2Row;

    void setUpUI();
    void setStyles();

private slots:
    void onAddUrlBtnClicked();
    void onDelUrlBtnClicked();
    bool isEnabledDelUrlBtn();
    void checkAddUrlBtn(QString text);
    void onDownloadBtnClicked();
    void onUpdateProgress(const QUrl &url, int);
    void onDownloadFinished(const QUrl &u, const QByteArray &);
    void onUpsampleFinished(const QUrl &u, const QImage &img);


};
#endif // MAINWINDOW_H
