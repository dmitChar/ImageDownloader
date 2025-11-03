#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager;

}

void DownloadManager::addImgToDownload(const QUrl &url)
{
    urlQueue.enqueue(url);
    if (urlQueue.size() == 1)
    {
        startDownloadNext();
    }

}
void DownloadManager::startDownloadNext()
{
    if (this->urlQueue.isEmpty())
        return;

    QUrl url = this->urlQueue.dequeue();
    QNetworkRequest req(url);
    QNetworkReply *rep = manager->get(req);

    QTimer *timeoutTimer = new QTimer(rep);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(15000);

    //Обработка таймаута
    connect(timeoutTimer, &QTimer::timeout, this, [this, rep, url]()
    {
        qDebug() << "Таймаут при загрузке";
        rep->abort();
        emit downloadError(url, "Timeout error");
    });

    //Обработка ошибок сети
    connect(rep, &QNetworkReply::errorOccurred, this, [this, url, rep] ()
    {
        qDebug() << "Ошибка сети" << rep->errorString();
        emit downloadError(url, rep->errorString());
    });


    //Отслеживание прогресса загрузки
    connect(rep, &QNetworkReply::downloadProgress, this, [this, url] (qint64 bytesReceived, qint64 bytesTotal)
    {
        if (bytesReceived > 0)
        {
            emit downloadProgress(url, static_cast<int>(bytesReceived / bytesTotal * 100));
        }
    });

    connect(rep, &QNetworkReply::finished, this, &DownloadManager::onFinished);
}

void DownloadManager::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QUrl url = reply->request().url();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200)
    {
        qDebug() << "HTTP статус" << statusCode << "для" << url;
        emit downloadError(url, QString("HTTP статус %1").arg(statusCode));
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << "Изображение скачено по ссылке" << url;
    emit downloadEnd(url, data);
    reply->deleteLater();
}
