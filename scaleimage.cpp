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

QString ScaleImage::getImgPath(QString name, ImgType type)
{
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
    }

    QString fileName = prefix + name;
    return (dir.filePath(subDir + QDir::separator() + fileName));
}

bool ScaleImage::saveImage(const QString &path, const QImage &img)
{
    if (path.isEmpty() || img.isNull())
    {
        qDebug() << "Ошибка: Пустой путь или изображение";
        emit scaleError(url, "Ошибка: Пустой путь или изображение");
        return false;
    }

    if (img.save(path))
    {
        qDebug() << "Изображение успешно сохранено" << path;
        return true;
    }

    qDebug() << "Ошибка сохранения изображения" << path;
    emit scaleError(url, "Ошибка сохранения изображения");
    return false;

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

    originalImg = originalImg.convertToFormat(QImage::Format_RGB888);
    cv::Mat srcMat(originalImg.height(), originalImg.width(), CV_8UC3, (void*)originalImg.bits(), originalImg.bytesPerLine());

    cv::Mat bgrMat;
    cv::cvtColor(srcMat, bgrMat, cv::COLOR_RGB2BGR);
    bgrMat.convertTo(bgrMat, CV_32F);

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
        emit scaleError(url, "Ошибка загрузки модели:");
        return;
    }

    // Upsample
    cv::Mat upscaledFloat;
    try
    {
        qDebug() << "Попытка upsample изображения в потоке" <<QThread::currentThreadId();
        model.upsample(bgrMat, upscaledFloat);
        qDebug() << "Upsample успешно завершен";
    }
    catch (const cv::Exception &e)
    {
        qDebug() << "Ошибка апскейла:" << e.what();
        emit scaleError(url, "Ошибка апскейла");
        return;
    }

    cv::Mat upscaledMat;
    upscaledFloat.convertTo(upscaledMat, CV_8U);

    // Конверт в RGB для QImage
    cv::Mat resultMat;
    cv::cvtColor(upscaledMat, resultMat, cv::COLOR_BGR2RGB);

    QImage scaledImg((const uchar*)resultMat.data, resultMat.cols, resultMat.rows, resultMat.step, QImage::Format_RGB888);

    if (scaledImg.isNull())
    {
        qDebug() << "Ошибка создания scaled QImage";
        emit scaleError(url, "Ошибка создания scaled QImage");
        return;
    }
    scaledImg = scaledImg.copy();

    QString path1 = getImgPath(url.toString(), ImgType::Source);
    QString path2 = getImgPath(url.toString(), ImgType::Scaled);

    if (saveImage(path1, originalImg) && saveImage(path2, scaledImg))
    {
        emit upscaleFinished(url, path1, path2);
    }
}
