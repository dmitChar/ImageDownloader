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
#include <QImageReader>

enum class ImgType
{
    Scaled,
    Source
};

class ScaleImage : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ScaleImage(const QUrl &url, const QByteArray &d, QObject *parent = nullptr);
    void run() override;

signals:
    void finished(const QUrl &url, const QImage &image);

private:
    QUrl url;
    QByteArray data;
    const QString modelPath = ":/models/EDSR_x4.pb";

    static void saveImage(QString name, const QImage &img, ImgType type);

};

#endif // SCALEIMAGE_H
