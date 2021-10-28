/**
  \file
  \brief    Déclaration de la classe Plate.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef PLATE_H
#define PLATE_H

#include <QObject>

#include "sprite.h"

class Plate : public Sprite {

public:
    Plate(QGraphicsItem* pParent = nullptr);

    void mouseMoved(QPointF newMousePosition);
};

#endif // PLATE_H
