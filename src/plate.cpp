/**
  \file
  \brief    Déclaration de la classe Plate.
  \author   CHENGAE
  \date     Octobre 2021
*/
#include "plate.h"

#include "gamescene.h"
#include "resources.h"
#include "sprite.h"

#include <QDebug>
#include <QGraphicsScale>

//Déclaration des constantes.
const int PLATE_VELOCITY = 150; // pixels par seconde
const int PLATE_HEIGHT = 100;

//! Construit et initialise un plateau.
//! \param pParent  Objet propiétaire de cet objet.
Plate::Plate(QGraphicsItem* pParent) : Sprite(BrickBreaker::imagesPath() + "plate.png", pParent) {
    this->setData(0, "plate");
    m_keyLeftPressed  = false;
    m_keyRightPressed = false;
    m_velocity = QPointF(0,0);
}

//! Cadence.
//! Gère le déplacement du plateau et de ces colisions.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void Plate::tick(long long elapsedTimeInMilliseconds) {

    // Calcul de la distance parcourue par le plateau, selon sa vitesse et le temps écoulé.
    QPointF distance = elapsedTimeInMilliseconds * m_velocity / 1000.;

    // Positionne la bounding box du plateau à sa prochaine position.
    QRectF nextRect = this->globalBoundingBox().translated(distance);

    // Récupère tous les sprites de la scène que toucherait ce sprite à sa prochaine position
    auto collidingSprites = this->parentScene()->collidingSprites(nextRect);

    // Supprimer le sprite lui-même, qui collisionne toujours avec sa boundingbox
    collidingSprites.removeAll(this);

    bool collision = collidingSprites.isEmpty();

    // Si la prochaine position reste dans les limites de la scène, le plateau
    // y est positionnée. Sinon, il reste sur place.
    if (this->parentScene()->isInsideScene(nextRect) || collision)
        this->setPos(this->pos() + distance);


}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void Plate::onKeyPressed(int key) {
    switch (key) {
    // Si le joueur presse la flèche de gauche.
    case Qt::Key_Left:
        m_keyLeftPressed = true;
        //updateVelocity();
        break;

    // Si le joueur presse la fléche de droite.
    case Qt::Key_Right:
        m_keyRightPressed = true;
        //updateVelocity();
        break;
    }
}

//! Traite le relâchement d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void Plate::onKeyReleased(int key) {
    switch (key) {
    // Si le joueur relache la flèche de gauche.
    case Qt::Key_Left:
        m_keyLeftPressed = false;
        //updateVelocity();
        break;

    // Si le joueur relache la fléche de droite.
    case Qt::Key_Right:
        m_keyRightPressed = false;
        //updateVelocity();
        break;
    }
}

//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void Plate::onMouseMoved(QPointF newMousePosition) {
    qreal positionX = this->width()/2.0; //Centre du plateau

    // Le plateau suit la souris uniquement à l'interieur du la zone de jeux et
    // le plateau s'arrète au bordure de la zone.
    if (newMousePosition.x() > 0 && newMousePosition.x() < m_pParentScene->width() &&
            newMousePosition.y() > 0 && newMousePosition.y() < m_pParentScene->height()) {

        if (newMousePosition.x() > m_pParentScene->width() - this->width() / 2.0)
            positionX = m_pParentScene->width() - this->width() / 2.0;

        else if (newMousePosition.x() > this->width() / 2.0)
            positionX = newMousePosition.x();

        this->setPos(positionX - this->width() / 2.0, m_pParentScene->height() - PLATE_HEIGHT);
    }
}

//! Met à jours la vitesse du plateau.
void Plate::updateVelocity() {
    int velocity = 0;

    if (m_keyLeftPressed)
        velocity = -PLATE_VELOCITY;

    if (m_keyRightPressed)
        velocity =  PLATE_VELOCITY;

    m_velocity = QPoint(velocity, 0);
}

