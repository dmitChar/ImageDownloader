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

    name = QString::number(qHash(name)) + ".jpg";
    QString basePath = QCoreApplication::applicationDirPath() + "/../Output";


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
    {
        dir.mkpath(subDir);
        QFile::setPermissions(subDir, QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup);
    }

    QString fileName = prefix + name;
    QString fullPath = dir.filePath(subDir + "/" + fileName);

    if (img.save(fullPath))
        qDebug() << "Изображение успешно сохранено" << fullPath;
    else qDebug() << "Ошибка сохранения изображения" << fullPath;
}

void ScaleImage::run()
{
    QImage originalImg;
    if (!originalImg.loadFromData(data))
    {
        qDebug() << "Ошибка загрузки данных изображения в апскейлере";
        return;
    }

    if (originalImg.isNull()) {
        qDebug() << "Изображение пустое";
        return;
    }

    cv::Mat srcMat(originalImg.height(), originalImg.width(), CV_8UC4, (void*)originalImg.bits(), originalImg.bytesPerLine());

    cv::Mat bgrMat;
    cv::cvtColor(srcMat, bgrMat, cv::COLOR_RGBA2BGR);
    bgrMat.convertTo(bgrMat, CV_32F, 1.0 / 255.0);

    QString tempPath = QDir::temp().filePath("EDSR_x4.pb");
    if (!QFile::exists(tempPath))
    {
        QFile::copy(modelPath, tempPath);
        QFile::setPermissions(tempPath, QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup);
    }

    // Попытка загрузки модели
    DnnSuperResImpl model;
    try
    {
        model.readModel(tempPath.toStdString());
        model.setModel("edsr", 4);
        qDebug() << "Модель успешно загружена и установлена";

    }
    catch (const cv::Exception &e)
    {
        qDebug() << "Ошибка загрузки модели:" << e.what();
        return;
    }

    // Upsample
    cv::Mat upscaledMat;
    try
    {
        qDebug() << "Попытка апскейла изображения...";
        cv::Mat upscaledFloat;
        model.upsample(bgrMat, upscaledFloat);
        upscaledFloat.convertTo(upscaledMat, CV_8U, 255.0);
        qDebug() << "Апскейл успешно завершен";
    }
    catch (const cv::Exception &e)
    {
        qDebug() << "Ошибка апскейла:" << e.what();
        return;
    }

    // Конверт в rgb
    cv::Mat resultMat;
    cv::cvtColor(upscaledMat, resultMat, cv::COLOR_BGR2RGB);

    QImage scaledImg((const uchar*)resultMat.data, resultMat.cols, resultMat.rows, resultMat.step, QImage::Format_RGB888);
    if (scaledImg.isNull())
    {
        qDebug() << "Ошибка создания scaled QImage";
        return;
    }
    scaledImg = scaledImg.copy();

    saveImage(url.toString(), originalImg, ImgType::Source);
    saveImage(url.toString(), scaledImg, ImgType::Scaled);

    emit finished(url, scaledImg);
}
