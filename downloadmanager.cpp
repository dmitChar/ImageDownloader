#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent)
    : QObject{parent}
{

}

void DownloadManager::addImgToDownload(const QUrl &url)
{
    if (url.isValid() && !url.isEmpty()) {
        urlQueue.enqueue(url);
        if (urlQueue.size() == 1)
        {
            startDownloadNext();
        }
    }
}
void DownloadManager::startDownloadNext()
{
    if (this->urlQueue.isEmpty())
        return;

    QUrl url = this->urlQueue.dequeue();
    QNetworkRequest req(url);
    QNetworkReply *rep = manager->get(req);

    //Отслеживание прогресса загрузки
    connect(rep, &QNetworkReply::downloadProgress, this, [this, url] (qint64 bytesReceived, qint64 bytesTotal)
    {
        if (bytesReceived > 0)
        {
            emit downloadProgress(url, static_cast<int>(bytesReceived / bytesTotal * 100));
        }
    });

    connect(rep, &QNetworkReply::finished, this, &DownloadManager::onFinished);
    startDownloadNext();
}

void DownloadManager::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    emit downloadEnd(reply->request().url(), data);
    reply->deleteLater();
}
