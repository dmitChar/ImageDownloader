#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    void addImgToDownload(const QUrl &url);

private:
    QQueue<QUrl> urlQueue;
    QNetworkAccessManager *manager;

signals:
    void downloadProgress(const QUrl &url, int a);
    void downloadEnd(const QUrl &url, QByteArray bytesReceived);
private slots:
    void startDownloadNext();
    void onFinished();
};

#endif // DOWNLOADMANAGER_H
