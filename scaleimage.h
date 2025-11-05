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
#include <QThread>

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
    void upscaleFinished(const QUrl &url, const QString &soureImgPath, const QString &scaledImgPath);
    void imageSaved(const QUrl &url, const QString &path, const QString &path2);
    void scaleError(const QUrl &url, const QString &error);

private:
    QUrl url;
    QByteArray data;
    const QString modelPath = ":/models/EDSR_x4.pb";

    bool saveImage(const QString &path, const QImage &img);
    QString getImgPath(QString name, ImgType type);

};

#endif // SCALEIMAGE_H
