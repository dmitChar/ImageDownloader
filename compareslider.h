#pragma once
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>

class CompareSlider : public QWidget
{
    Q_OBJECT
public:
    explicit CompareSlider(QWidget *parent = nullptr);

    void setImages(const QImage &left, const QImage &right);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap leftPixmap;
    QPixmap rightPixmap;
    int dividerX;     // положение линии разделителя
    bool dragging = false;
};
