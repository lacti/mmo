#pragma once

class graphics_view : public QGraphicsView
{
public:
    graphics_view(QGraphicsScene* scene)
		: QGraphicsView(scene)
    {
        setWindowTitle(QString::fromWCharArray(L"MMO (엠엠오)"));

        //setWindowFlags(Qt::FramelessWindowHint);
    }

protected:
    void resizeEvent(QResizeEvent *event) {
        if (scene())
        {
            scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        }

        QGraphicsView::resizeEvent(event);
    }
};
