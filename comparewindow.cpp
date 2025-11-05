#include "CompareWindow.h"


CompareWindow::CompareWindow(QWidget *parent)
    : QWidget(parent), sliderPos(0), dragging(false), hoverOnHandle(false)
{
    setMinimumSize(400, 300);
    setMouseTracking(true);
}

void CompareWindow::setImages(const QImage &orig, const QImage &upscaled)
{
    origImg = orig;
    upscaledImg = upscaled;
    updateImageRect();
    sliderPos = imageRect.center().x();
    update();
}

void CompareWindow::updateImageRect()
{
    if (origImg.isNull()) {
        imageRect = QRect();
        return;
    }

    QSize scaledSize = origImg.size().scaled(size(), Qt::KeepAspectRatio);

    int x = (width() - scaledSize.width()) / 2;
    int y = (height() - scaledSize.height()) / 2;

    imageRect = QRect(QPoint(x, y), scaledSize);
}

bool CompareWindow::isOnHandle(int x) const
{
    return std::abs(x - sliderPos) <= 10;
}

void CompareWindow::paintEvent(QPaintEvent *)
{
    if (origImg.isNull() || upscaledImg.isNull())
        return;

    QPainter p(this);

    QImage scaledOrig = origImg.scaled(size(), Qt::KeepAspectRatio);
    QImage scaledUpscaled = upscaledImg.scaled(size(), Qt::KeepAspectRatio);

    int xOffset = (width() - scaledOrig.width()) / 2;
    int yOffset = (height() - scaledOrig.height()) / 2;

    p.setClipRect(0, 0, sliderPos, height());
    p.drawImage(xOffset, yOffset, scaledOrig);

    p.setClipRect(sliderPos, 0, width() - sliderPos, height());
    p.drawImage(xOffset, yOffset, scaledUpscaled);

    p.setClipping(false);

    QPen linePen(Qt::white, hoverOnHandle || dragging ? 3 : 2);
    p.setPen(linePen);
    p.drawLine(sliderPos, 0, sliderPos, height());

    QPoint center(sliderPos, height() / 2);
    QPolygon leftArrow({QPoint(center.x() - 10, center.y()),
                        QPoint(center.x() - 3, center.y() - 7),
                        QPoint(center.x() - 3, center.y() + 7)});
    QPolygon rightArrow({QPoint(center.x() + 10, center.y()),
                         QPoint(center.x() + 3, center.y() - 7),
                         QPoint(center.x() + 3, center.y() + 7)});
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawPolygon(leftArrow);
    p.drawPolygon(rightArrow);
}

void CompareWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && isOnHandle(event->pos().x()))
    {
        dragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CompareWindow::mouseMoveEvent(QMouseEvent *event)
{
    int mouseX = event->pos().x();

    if (dragging)
    {
        //ограничиваем движение границами изображения
        sliderPos = qBound(imageRect.left(), mouseX, imageRect.right());
        update();
    }
    else
    {
        bool onHandle = imageRect.contains(event->pos()) && isOnHandle(mouseX);
        if (onHandle != hoverOnHandle) {
            hoverOnHandle = onHandle;
            setCursor(onHandle ? Qt::OpenHandCursor : Qt::ArrowCursor);
            update();
        }
    }
}

void CompareWindow::mouseReleaseEvent(QMouseEvent *)
{
    dragging = false;
    setCursor(Qt::ArrowCursor);
}

void CompareWindow::leaveEvent(QEvent *)
{
    hoverOnHandle = false;
    setCursor(Qt::ArrowCursor);
    update();
}

void CompareWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImageRect();

    if (!imageRect.isEmpty())
    {
        sliderPos = qBound(imageRect.left(), sliderPos, imageRect.right());
    }
}
