/**
  \file
  \brief    Déclaration de la classe Ball.
  \author   CHENGAE
  \date     Novembre 2021
*/
#include "ball.h"

//#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"

#include <cmath>

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScale>
#include <QPainter>

const int INITIAL_VELOCITY_X = 0;
const int INITIAL_VELOCITY_Y = 200;

//! Constructeur
Ball::Ball(const QPixmap& rPixmap, QGraphicsItem* pParent) : Sprite(rPixmap, pParent) {
    this->setData(0, "ball");
    setSpriteVelocity(INITIAL_VELOCITY_X, INITIAL_VELOCITY_Y);
    m_spriteVelocityX = INITIAL_VELOCITY_X;
    m_spriteVelocityY = INITIAL_VELOCITY_Y;
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
    m_spriteVelocity = QPointF(xVelocity, yVelocity);
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

        for(int i = 0; i < collidingSprites.size(); i++) {
            // Test si le sprite en collision est le plateau, si oui : la vélocité est modifiée.
            if (collidingSprites.at(i)->data(0).toString() == "plate") {
                Sprite* plate = collidingSprites.at(i);

                int leftPlate = plate->left();
                int rightPlate = plate->right();
                int midPlate = plate->width() / 100;
                int midBall = this->left() + (this->width() / 2);

                bool ballHitLeft = (midBall < leftPlate + (midPlate * 50));

                if(midBall < leftPlate + (midPlate * 15) || midBall > rightPlate - (midPlate * 15)) {
                    m_angle = 50;

                } else if (midBall < leftPlate + (midPlate * 40) || midBall > rightPlate - (midPlate * 40))  {
                    m_angle = 50;
                }

                m_spriteVelocityX += (ballFromLeft ? (ballHitLeft ? m_angle : -m_angle) : (ballHitLeft ? -m_angle : m_angle));


            } else {
                m_angle = 0;
            }
        }

        if(std::abs(minOverlapX) < std::abs(minOverlapY)) {
            m_spriteVelocity.setX(ballFromLeft ? -m_spriteVelocityX : m_spriteVelocityX);
        } else {
            m_spriteVelocity.setY(ballFromTop ? -m_spriteVelocityY : m_spriteVelocityY);

            if(m_angle != 0.0)
                m_spriteVelocity.setX(ballFromLeft ? -m_spriteVelocityX : m_spriteVelocityX);
        }

        spriteMovement = m_spriteVelocity * elapsedTimeInMilliseconds / 1000.;

        // Parcours la liste des sprites avec des collisions.
        for(int i = 0; i < collidingSprites.size(); i++) {
            // Test si le sprite en collision est une brique, si oui : elle est détruite.
            if (collidingSprites.at(i)->data(0).toString() == "brick") {
                collidingSprites.at(i)->deleteLater();
            }
        }
    }

    // Test si la balle est à l'intérieur de la zone de jeux, si non : elle est détruite.
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

