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
    sliderPos = width() / 2;
    update();
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
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    QImage scaledOrig = origImg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QImage scaledUpscaled = upscaledImg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    int centerY = (scaledOrig.height() - height()) / 2;
    int offsetX = (scaledOrig.width() - width()) / 2;

    QRect leftRect(offsetX, centerY, sliderPos, height());
    QRect rightRect(offsetX + sliderPos, centerY, width() - sliderPos, height());

    p.drawImage(QRect(0, 0, sliderPos, height()), scaledOrig, leftRect);
    p.drawImage(QRect(sliderPos, 0, width() - sliderPos, height()), scaledUpscaled, rightRect);

    // линия-разделитель
    QPen linePen(Qt::white, hoverOnHandle || dragging ? 3 : 2);
    p.setPen(linePen);
    p.drawLine(sliderPos, 0, sliderPos, height());

    // ползунок
    QPoint center(sliderPos, height() / 2);
    QPolygon leftArrow({QPoint(center.x() - 10, center.y()),
                        QPoint(center.x() - 3, center.y() - 7),
                        QPoint(center.x() - 3, center.y() + 7)});
    QPolygon rightArrow({QPoint(center.x() + 10, center.y()),
                         QPoint(center.x() + 3, center.y() - 7),
                         QPoint(center.x() + 3, center.y() + 7)});
    p.setBrush(Qt::white);
    p.drawPolygon(leftArrow);
    p.drawPolygon(rightArrow);
}

void CompareWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && isOnHandle(event->pos().x())) {
        dragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CompareWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        sliderPos = qBound(0, event->pos().x(), width());
        update();
    } else {
        // подсветка при наведении
        bool onHandle = isOnHandle(event->pos().x());
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
