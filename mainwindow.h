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
#include <QTimer>

#include "downloadmanager.h"
#include "scaleimage.h"
#include "comparewindow.h"

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
    QHash<QUrl, QString> hashUrl2SourceImgPath;
    QHash<QUrl, QString> hashUrl2ScaledImgPath;

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
    void onUpscaleFinished(const QUrl &u, const QString &soureImgPath, const QString &scaledImgPath);
    void onProgressTableDoubleClicked(int row, int col);


};
#endif // MAINWINDOW_H
