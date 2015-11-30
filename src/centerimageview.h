#ifndef CENTERIMAGEVIEW_H
#define CENTERIMAGEVIEW_H
#include <QGraphicsView>
#include <QMouseEvent>
class CenterImageView : public QGraphicsView
{
    Q_OBJECT
public:
    CenterImageView(QWidget *parent=0);
    QPoint last_click;

signals:
    void mousePress(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);


private:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


};

#endif // CENTERIMAGEVIEW_H
