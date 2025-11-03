#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QTimer>


class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    void addImgToDownload(const QUrl &url);

private:
    QQueue<QUrl> urlQueue;
    QNetworkAccessManager *manager;

    void saveImage(QString path, const QImage &img);

signals:
    void downloadProgress(const QUrl& url, int a);
    void downloadEnd(const QUrl &url, QByteArray bytesReceived);
    void downloadError(const QUrl &url, const QString &error);

private slots:
    void startDownloadNext();
    void onFinished();
};

#endif // DOWNLOADMANAGER_H
