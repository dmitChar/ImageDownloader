#ifndef COMPAREWINDOW_H
#define COMPAREWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>

class CompareWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CompareWindow(QWidget *parent = nullptr);
    void setImages(const QImage &orig, const QImage &upscaled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QImage origImg;
    QImage upscaledImg;
    int sliderPos;
    bool dragging;
    bool hoverOnHandle;

    bool isOnHandle(int x) const;
};
#endif // COMPAREWINDOW_H
