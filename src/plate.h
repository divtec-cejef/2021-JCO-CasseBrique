/**
  \file
  \brief    Déclaration de la classe Plate.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef PLATE_H
#define PLATE_H

#include "sprite.h"

class Plate : public Sprite {
    // Nécessaire de déclarer cette macro pour que la classe puisse
    // bénéficier du mécanisme de signaux/slots.
    Q_OBJECT

public:
    Plate(QGraphicsItem* pParent = nullptr);

    void tick(long long elapsedTimeInMilliseconds);

public slots:
    void onMouseMoved(QPointF newMousePosition);

private:
    QPointF m_velocity;

};

#endif // PLATE_H
