/**
  \file
  \brief    Déclaration de la classe Plate.
  \author   CHENGAE
  \date     Octobre 2021
*/
#include "plate.h"
#include "gamescene.h"

#include "resources.h"

#include <QDebug>

//! Construit et initialise un plateau.
//! \param pParent  Objet propiétaire de cet objet.
Plate::Plate(QGraphicsItem* pParent) : Sprite(GameFramework::imagesPath() + "plate.png", pParent) {

}

//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void Plate::mouseMoved(QPointF newMousePosition) {
    qreal positionX = this->width()/2.0; //Centre du plateau

        // Stop le plateau si il tape la bordure
        if (newMousePosition.x() > m_pParentScene->width() - this->width()/2.0)
            positionX = m_pParentScene->width() - this->width()/2.0;

         else if (newMousePosition.x() > this->width()/2.0)
            positionX = newMousePosition.x();

        this->setPos(positionX - this->width()/2.0, m_pParentScene->height() - 100);
}
