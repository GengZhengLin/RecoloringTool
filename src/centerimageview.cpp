#include "centerimageview.h"

CenterImageView::CenterImageView(QWidget *parent): QGraphicsView(parent)
{

}

void CenterImageView::mousePressEvent(QMouseEvent *event)
{
    last_click = event->pos();
    emit mousePress(event);
}

void CenterImageView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(event);
}

void CenterImageView::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseRelease(event);
}

