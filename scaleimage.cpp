#include "scaleimage.h"
#include <opencv2/dnn_superres.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace dnn_superres;

ScaleImage::ScaleImage(const QUrl &url, QSize size, const QByteArray &d, QObject *parent)
    : QObject{parent},
    url(url),
    targetSize(size),
    data(d)
{
    setAutoDelete(true);

}

QString HashUrl(const QUrl &url)
{
    QByteArray hash = QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Sha1);
    return QString(hash.toHex());
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

    emit finished(this->url, result);
}
