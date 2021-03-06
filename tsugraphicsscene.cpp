#include "tsugraphicsscene.h"

tsugraphicsscene::tsugraphicsscene(QWidget *parent)
    : QWidget(parent)
{

}

tsugraphicsscene::~tsugraphicsscene()
{
    qDebug("tsugraphicsscene destroyed");
}

void tsugraphicsscene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
 //    qDebug("mousePressEvent");
    // Ignore deselect all on right click
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(event);
}

