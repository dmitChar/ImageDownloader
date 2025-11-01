#ifndef DOWNLOADTAKS_H
#define DOWNLOADTAKS_H

#include <QObject>

class DownloadTaks : public QObject
{
    Q_OBJECT
public:
    explicit DownloadTaks(QObject *parent = nullptr);

signals:
};

#endif // DOWNLOADTAKS_H
