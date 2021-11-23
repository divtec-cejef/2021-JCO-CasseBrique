/**
  \file
  \brief    Déclaration de la classe Brick.
  \author   CHENGAE
  \date     Novembre 2021
*/
#ifndef BRICK_H
#define BRICK_H

#include "sprite.h"

class Brick : public Sprite {

public:
    Brick(QGraphicsItem* pParent = nullptr);
};

#endif // BRICK_H
