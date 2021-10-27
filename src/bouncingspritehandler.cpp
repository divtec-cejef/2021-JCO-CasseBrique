/**
  \file
  \brief    Définition de la classe BouncingSpriteHandler.
  \author   JCO
  \date     septembre 2018
*/
#include "bouncingspritehandler.h"

#include "sprite.h"
#include "gamescene.h"

#include <QDebug>
#include <cmath>

const int INITIAL_VELOCITY = 200;

//! Constructeur.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
BouncingSpriteHandler::BouncingSpriteHandler(Sprite* pParentSprite) : SpriteTickHandler (pParentSprite) {
    setSpriteVelocity(INITIAL_VELOCITY ,INITIAL_VELOCITY);
}

//! Change le vecteur de vitesse de déplacement du sprite.
//! \param spriteVelocity Vecteur de vitesse de déplacement du sprite.
void BouncingSpriteHandler::setSpriteVelocity(QPointF spriteVelocity) {
    m_spriteVelocity = QPointF(spriteVelocity);
}

//! Change le vecteur de vitesse de déplacement du sprite.
//! \param xVelocity Vitesse de déplacement en X.
//! \param yVelocity Vitesse de déplacement en Y.
void BouncingSpriteHandler::setSpriteVelocity(double xVelocity, double yVelocity) {
    m_spriteVelocity = QPointF(xVelocity ,yVelocity);
}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé,
//! vérifie si il doit rebondir et le positionne à son nouvel emplacement.
void BouncingSpriteHandler::tick(long long elapsedTimeInMilliseconds) {
    QPointF spriteMovement = m_spriteVelocity * elapsedTimeInMilliseconds / 1000.;

    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = m_pParentSprite->globalBoundingBox().translated(spriteMovement);

    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = m_pParentSprite->parentScene()->collidingSprites(nextSpriteRect);

    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(m_pParentSprite);

    if (!collidingSprites.isEmpty())  {
        // On ne considère que la première collision (au cas où il y en aurait plusieurs)
        Sprite* pCollidingSprite = collidingSprites[0];

        // Technique très approximative pour simuler un rebond en simplifiant
        // la façon de déterminer le vecteur normal de la surface du rebond.
        float overlapLeft = m_pParentSprite->right() - pCollidingSprite->left();
        float overlapRight = pCollidingSprite->right() - m_pParentSprite->left();
        float overlapTop = m_pParentSprite->bottom() - pCollidingSprite->top();
        float overlapBottom = pCollidingSprite->bottom() - m_pParentSprite->top();

        bool ballFromLeft(std::abs(overlapLeft) < std::abs(overlapRight));
        bool ballFromTop(std::abs(overlapTop) < std::abs(overlapBottom));

        float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

        if(std::abs(minOverlapX) < std::abs(minOverlapY))
            m_spriteVelocity.setX(ballFromLeft ? -INITIAL_VELOCITY :INITIAL_VELOCITY);
        else
            m_spriteVelocity.setY(ballFromTop ? -INITIAL_VELOCITY : INITIAL_VELOCITY);

        spriteMovement = m_spriteVelocity * elapsedTimeInMilliseconds / 1000.;
    }

    m_pParentSprite->setPos(m_pParentSprite->pos() + spriteMovement);
}


