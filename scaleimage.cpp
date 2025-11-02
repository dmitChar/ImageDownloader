#include "scaleimage.h"
#include <opencv2/dnn_superres.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace dnn_superres;

ScaleImage::ScaleImage(const QUrl &url, const QByteArray &d, QObject *parent)
    : QObject{parent},
    url(url),
    data(d)
{
    setAutoDelete(true);

}

QString HashUrl(const QUrl &url)
{
    QByteArray hash = QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Sha1);
    return QString(hash.toHex());
}

void ScaleImage::saveImage(QString name, const QImage &img, ImgType type)
{
    if (name.isEmpty() || img.isNull())
    {
        qDebug() << "Ошибка: Пустое имя или изображение";
        return;
    }
    QString basePath = QDir::currentPath();
    QString subDir;
    QString prefix = "";

    switch (type)
    {
    case ImgType::Scaled:
        prefix = "scaled_";
        subDir = "Scaled";
        break;

    case ImgType::Source:
        subDir = "Source";
        break;
    }

    QDir dir(basePath);

    if (!dir.exists(subDir))
        dir.mkpath(subDir);

    QString fileName = prefix + name;
    QString fullPath = dir.filePath(subDir + "/" + fileName);

    if (img.save(fullPath))
        qDebug() << "Сохранено изображение" << fullPath;
    else qDebug() << "Ошибка сохранения изображения" << fullPath;
}

void ScaleImage::run()
{
    QImage img;
    img.loadFromData(data);
    if (img.isNull()) return;

    Mat rgb_img(img.height(), img.width(), CV_8UC4, (void*)img.bits(), img.bytesPerLine());
    cv::Mat bgr_img;
    cv::cvtColor(rgb_img, bgr_img, cv::COLOR_RGBA2BGR);

    //Загружаем модель
    DnnSuperResImpl model;
    model.readModel(":/models/EDSR_x4.pb");
    model.setModel("edsr", 4);

    cv::Mat upscaled;
    model.upsample(bgr_img, upscaled);

    cv::cvtColor(upscaled, rgb_img, cv::COLOR_BGR2RGB);

    QImage result((uchar*)rgb_img.data, rgb_img.cols, rgb_img.rows, rgb_img.step, QImage::Format_RGB888);
    qDebug() << "Изображение обработано";

    saveImage(url.toString(), img, ImgType::Source);
    saveImage(url.toString(), result, ImgType::Scaled);



    emit finished(this->url, result);
}
