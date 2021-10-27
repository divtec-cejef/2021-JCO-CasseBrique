/**
  \file
  \brief    Déclaration de la classe WalkingMan.
  \author   JCO
  \date     septembre 2018
*/
#include "walkingman.h"

#include "resources.h"

//! Construit un marcheur et l'initialise.
//! \param pParent  Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
WalkingMan::WalkingMan(QGraphicsItem* pParent) : Sprite(pParent) {
    configureAnimation();
}

//! Charge les différentes images qui composent l'animation du marcheur et
//! les ajoute à ce sprite.
void WalkingMan::configureAnimation() {
    for (int FrameNumber = 1; FrameNumber <= 8; ++FrameNumber)  {
        this->addAnimationFrame(QString(GameFramework::imagesPath() + "marche%1.png").arg(FrameNumber));
    }
    this->setAnimationSpeed(100);  // Passe à la prochaine image de la marche toutes les 100 ms
    this->startAnimation();
}

