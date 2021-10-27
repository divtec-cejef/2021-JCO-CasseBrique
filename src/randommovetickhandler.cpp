/**
  \file
  \brief    Définition de la classe RandomMoveTickHandler.
  \author   JCO
  \date     août 2020
*/
#include "randommovetickhandler.h"

#include <cstdlib>
#include <ctime>
#include <QtMath>
#include <QPointF>
#include <QRectF>

#include "sprite.h"
#include "gamescene.h"

bool RandomMoveTickHandler::s_seeded = false;

const double DEFAULT_SPRITE_VELOCITY = 250.0;
const int MOVE_MINIMAL_DURATION = 400;
const int MOVE_MAXIMAL_DURATION = 2000;
const int MAXIMAL_ANGLE_CHANGE = 120; // Changement de direction maximal autorisé

//! Constructeur.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
RandomMoveTickHandler::RandomMoveTickHandler(Sprite* pParentSprite) : SpriteTickHandler (pParentSprite)
{
    // Initialisation, si pas encore fait, du générateur pseudo-aléatoire
    if (!s_seeded) {
        std::srand(std::time(nullptr));
        s_seeded = true;
    }

    m_spriteVelocity = DEFAULT_SPRITE_VELOCITY;
}

//! Constructeur.
//! \param ignoreCollision      Indique si le déplacement n'a pas à éviter les collisions.
//! \param destroyOnCollision   Indique si le sprite géré doit être détruit en cas de collision.
//! \param pParentSprite Sprite dont le déplacement doit être géré.
RandomMoveTickHandler::RandomMoveTickHandler(bool ignoreCollision, bool destroyOnCollision, Sprite* pParentSprite) : RandomMoveTickHandler(pParentSprite) {
    m_ignoreCollision = ignoreCollision;
    m_destroyOnCollision = destroyOnCollision;
}

//! Initialisation du gestionnaire.
void RandomMoveTickHandler::init() {

}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé.
void RandomMoveTickHandler::tick(long long elapsedTimeInMilliseconds) {
    // Calcul de la distance parcourue par le sprite, selon sa vitesse et le temps écoulé.
    double distance = elapsedTimeInMilliseconds * m_spriteVelocity / 1000.;

    // Détermine, selon l'angle, le déplacement en x et en y
    double moveX = qCos(m_moveAngle) * distance;
    double moveY = qSin(m_moveAngle) * distance;

    QPointF spriteMovement(moveX, moveY);

    bool collision = false;

    // Détermine la prochaine position du sprite
    QRectF nextSpriteRect = m_pParentSprite->globalBoundingBox().translated(spriteMovement);

    // Si les collisions ne sont pas ignorées, on vérifie si le déplacement provoquerait une
    // collision afin de modifier la trajectoire le cas échéant.
    if (!m_ignoreCollision) {

        // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
        auto collidingSprites = m_pParentSprite->parentScene()->collidingSprites(nextSpriteRect);

        // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
        collidingSprites.removeAll(m_pParentSprite);

        collision = !collidingSprites.isEmpty();
    }

    m_moveDuration -= elapsedTimeInMilliseconds;

    if (m_moveDuration < 0 ||
        !m_pParentSprite->parentScene()->isInsideScene(nextSpriteRect) ||
        collision) {
        initNextMove();

    } else {
        m_pParentSprite->setPos(m_pParentSprite->pos() + spriteMovement);
    }

    // Si la destruction sur collision est activée, on vérifie si le sprite
    // est en collision. Si c'est le cas, on le détruit.
    if (m_destroyOnCollision && !m_pParentSprite->parentScene()->collidingSprites(m_pParentSprite).isEmpty()) {
        // Il ne faut en aucun cas effacer immédiatement le sprite avec delete, car l'effacement immédiat du
        // sprite provoque la destruction immédiate de ce handler, alors qu'il est en cours d'exécution. Cela
        // provoquerait alors un crash.
        // deleteLater() permet d'efface le sprite plus tard, une fois que le code qui le concerne a été
        // complètement exécuté.
        m_pParentSprite->deleteLater();
    }
}

//! Change la vitesse de déplacement du sprite.
//! \param velocityInPixelPerSecond  Nouvelle vitesse de déplacement du sprite.
void RandomMoveTickHandler::setSpriteVelocity(double velocityInPixelPerSecond) {
    m_spriteVelocity = velocityInPixelPerSecond;
}

//! Détermine si ce sprite ignore les collisions lors de son déplacement.
//! Si c'est le cas, il ne change pas sa trajectoire en cas de collision.
//! Sinon, en cas de collision, il change sa trajectoire.
//! \param ignoreCollision Indique si les collisions doivent être ignorées.
void RandomMoveTickHandler::setIgnoreCollisionEnabled(bool ignoreCollision) {
    m_ignoreCollision = ignoreCollision;
}

//! \return un booléen qui indique si ce sprite ignore les collisions.
bool RandomMoveTickHandler::isIgnoreCollisionEnable() const {
    return m_ignoreCollision;
}

//! Indique si le sprite piloté par ce gestionnaire doit être détruit en cas de collision.
//! \param destroyOnCollision  Indique si une collision provoque la destruction du sprite.
void RandomMoveTickHandler::setDestroyOnCollisionEnabled(bool destroyOnCollision) {
    m_destroyOnCollision = destroyOnCollision;
}

//! \return un booléen qui indique si une collision provoque la destruction du sprite
//! piloté par ce gestionnaire.
bool RandomMoveTickHandler::isDestroyedOnCollision() const {
    return m_destroyOnCollision;
}

//! Détermine les paramètres du prochain mouvement aléatoire que va réaliser ce sprite :
//! - Durée du mouvement
//! - Angle de déplacement
void RandomMoveTickHandler::initNextMove() {
    m_moveDuration = std::rand() % (MOVE_MAXIMAL_DURATION - MOVE_MINIMAL_DURATION) + MOVE_MINIMAL_DURATION;

    int newAngleDelta = std::rand() % MAXIMAL_ANGLE_CHANGE - (MAXIMAL_ANGLE_CHANGE / 2);
    m_moveAngle = m_moveAngle + qDegreesToRadians(static_cast<double>(newAngleDelta));
}
