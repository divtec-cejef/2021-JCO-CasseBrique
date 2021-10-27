/**
  \file
  \brief    Déclaration de la classe BouncingSpriteHandler.
  \author   JCO
  \date     septembre 2018
*/
#ifndef BOUNCINGSPRITEHANDLER_H
#define BOUNCINGSPRITEHANDLER_H

//! \brief Classe qui gère le déplacement par rebond d'un sprite.
//!

#include <QPointF>

#include "spritetickhandler.h"

class BouncingSpriteHandler : public SpriteTickHandler
{
public:
    BouncingSpriteHandler(Sprite* pParentSprite = nullptr);

    void setSpriteVelocity(QPointF spriteVelocity);
    void setSpriteVelocity(double xVelocity, double yVelocity);

    void tick(long long elapsedTimeInMilliseconds);

private:
    QPointF m_spriteVelocity;

};

#endif // BOUNCINGSPRITEHANDLER_H
