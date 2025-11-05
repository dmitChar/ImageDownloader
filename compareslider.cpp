#include "CompareSlider.h"
#include <QPainter>
#include <QStyleOption>

CompareSlider::CompareSlider(QWidget *parent)
    : QWidget(parent), dividerX(width() / 2)
{
    setMinimumSize(900, 600);
    setMouseTracking(true);
}

void CompareSlider::setImages(const QImage &left, const QImage &right)
{
    leftPixmap = QPixmap::fromImage(left);
    rightPixmap = QPixmap::fromImage(right);
    update();
}

void CompareSlider::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Нарисовать правое изображение (апскейл)
    if (!rightPixmap.isNull())
        p.drawPixmap(rect(), rightPixmap);

    // Создать обрезанную область для левого изображения (оригинал)
    if (!leftPixmap.isNull())
    {
        QRect clipRect(0, 0, dividerX, height());
        p.setClipRect(clipRect);
        p.drawPixmap(rect(), leftPixmap);
    }

    // Нарисовать разделительную линию
    p.setClipping(false);
    p.setPen(QPen(Qt::white, 2));
    p.drawLine(dividerX, 0, dividerX, height());

    // Нарисовать ползунок
    const int handleWidth = 10;
    QRect handle(dividerX - handleWidth / 2, height()/2 - 25, handleWidth, 50);
    p.setBrush(Qt::white);
    p.drawRoundedRect(handle, 3, 3);
}

void CompareSlider::mousePressEvent(QMouseEvent *event)
{
    if (abs(event->pos().x() - dividerX) < 10)
        dragging = true;
}

void CompareSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging)
    {
        dividerX = std::clamp(event->pos().x(), 0, width());
        update();
    }
}

void CompareSlider::resizeEvent(QResizeEvent *)
{
    dividerX = width() / 2;
    update();
}
