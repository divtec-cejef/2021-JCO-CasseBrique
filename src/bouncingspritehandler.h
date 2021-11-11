/**
  \file
  \brief    Déclaration de la classe BouncingSpriteHandler.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef BOUNCINGSPRITEHANDLER_H
#define BOUNCINGSPRITEHANDLER_H

//! \brief Classe qui gère le déplacement par rebond d'un sprite.
//!

#include <QPointF>

#include "spritetickhandler.h"

class BouncingSpriteHandler : public SpriteTickHandler {

public:
    BouncingSpriteHandler(Sprite* pParentSprite = nullptr);

    void setSpriteVelocity(QPointF spriteVelocity);
    void setSpriteVelocity(double xVelocity, double yVelocity);

    void tick(long long elapsedTimeInMilliseconds);

    QPointF getVelocity();
private:
    QPointF m_spriteVelocity;
};

#endif // BOUNCINGSPRITEHANDLER_H
