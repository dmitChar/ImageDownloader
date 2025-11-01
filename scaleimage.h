#ifndef SCALEIMAGE_H
#define SCALEIMAGE_H

#include <QObject>
#include <QRunnable>
#include <QUrl>
#include <QHash>
#include <QCryptographicHash>
#include <QDir>
#include <QSize>
#include <QBuffer>
#include <QImage>

class ScaleImage : public QObject, QRunnable
{
    Q_OBJECT
public:
    explicit ScaleImage(const QUrl &url, QSize size, const QByteArray &d, QObject *parent = nullptr);
    void run() override;

signals:
    void finished(const QUrl &url, const QImage &image);

private:
    QSize targetSize;
    QUrl url;
    QByteArray data;

};

#endif // SCALEIMAGE_H
