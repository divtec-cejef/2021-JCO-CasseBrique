/**
  \file
  \brief    Déclaration de la classe Brick.
  \author   CHENGAE
  \date     Novembre 2021
*/
#include "brick.h"

#include "gamescene.h"
#include "resources.h"
#include "sprite.h"

#include <QDebug>
#include <QGraphicsScale>

Brick::Brick(QGraphicsItem* pParent) : Sprite (BrickBreaker::imagesPath() + "plate.png", pParent) {

}
