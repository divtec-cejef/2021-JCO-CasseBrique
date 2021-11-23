/**
  \file
  \brief    Déclaration de la classe Ball.
  \author   CHENGAE
  \date     Novembre 2021
*/
#include "ball.h"

#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"

#include <cmath>

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScale>
#include <QPainter>

const int INITIAL_VELOCITY = 200;

//! Constructeur
Ball::Ball(const QPixmap& rPixmap, QGraphicsItem* pParent) : Sprite(rPixmap, pParent) {
    this->setData(0, "ball");
    setSpriteVelocity(INITIAL_VELOCITY, INITIAL_VELOCITY);
}

//! Change le vecteur de vitesse de déplacement du sprite.
//! \param spriteVelocity Vecteur de vitesse de déplacement du sprite.
void Ball::setSpriteVelocity(QPointF spriteVelocity) {
    m_spriteVelocity = QPointF(spriteVelocity);
}

//! Change le vecteur de vitesse de déplacement du sprite.
//! \param xVelocity Vitesse de déplacement en X.
//! \param yVelocity Vitesse de déplacement en Y.
void Ball::setSpriteVelocity(double xVelocity, double yVelocity) {
    m_spriteVelocity = QPointF(xVelocity ,yVelocity);
}

QPointF Ball::getSpriteVelocity() {
    return m_spriteVelocity;
}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé,
//! vérifie si il doit rebondir et le positionne à son nouvel emplacement.
void Ball::tick(long long elapsedTimeInMilliseconds) {
    QPointF spriteMovement = m_spriteVelocity * elapsedTimeInMilliseconds / 1000.;

    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = this->globalBoundingBox().translated(spriteMovement);

    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = this->parentScene()->collidingSprites(nextSpriteRect);

    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(this);

    if (!collidingSprites.isEmpty()) {
        // On ne considère que la première collision (au cas où il y en aurait plusieurs)
        Sprite* pCollidingSprite = collidingSprites[0];

        // Technique très approximative pour simuler un rebond en simplifiant
        // la façon de déterminer le vecteur normal de la surface du rebond.
        float overlapLeft = this->right() - pCollidingSprite->left();
        float overlapRight = pCollidingSprite->right() - this->left();
        float overlapTop = this->bottom() - pCollidingSprite->top();
        float overlapBottom = pCollidingSprite->bottom() - this->top();

        bool ballFromLeft(std::abs(overlapLeft) < std::abs(overlapRight));
        bool ballFromTop(std::abs(overlapTop) < std::abs(overlapBottom));

        float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

        if(std::abs(minOverlapX) < std::abs(minOverlapY))
            m_spriteVelocity.setX(ballFromLeft ? -INITIAL_VELOCITY : INITIAL_VELOCITY);
        else
            m_spriteVelocity.setY(ballFromTop ? -INITIAL_VELOCITY : INITIAL_VELOCITY);

        spriteMovement = m_spriteVelocity * elapsedTimeInMilliseconds / 1000.;

        // Parcours la liste et supprime ceux qui sont entrés en collision
        for(int i = 0; i < collidingSprites.size(); i++) {
            if (collidingSprites.at(i)->data(0).toString() == "brick-to-destroy") {
                collidingSprites.at(i)->deleteLater();
            }
        }
    }

    if (!this->parentScene()->isInsideScene(nextSpriteRect) && collidingSprites.isEmpty()) {
        this->deleteLater();
    }

    this->setPos(this->pos() + spriteMovement);
}

void Ball::onResumeTick() {
    this->registerForTick();
}

void Ball::onPauseTick() {
    this->unregisterFromTick();
}

