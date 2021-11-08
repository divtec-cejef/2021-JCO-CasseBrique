/**
  \file
  \brief    Déclaration de la classe Brick.
  \author   CHENGAE
  \date     Octobre 2021
*/
#include "brick.h"

#include <QDebug>

#include "gamescene.h"
#include "resources.h"

const QPoint BRICK_SIZE(50,20);

Brick::Brick() {

}

//! Retire de la liste des sprite le sprite qui va être détruit.
void Brick::onSpriteDestroyed() {
    m_counterBrick--;
}


void Brick::createLevel(int level) {
    int line = 0;
    int column = 0;

    switch (level) {
    case 1:
        line = 2;
        column = 8;
        break;
    case 2:
        line = 3;
        column = 10;
        break;
    }

    m_counterBrick = line * column;

    int brickPosX = 0;
    int brickPosY = 0;

    for (int i = 0; i < line; i++) {

        for (int j = 0; j < column; j++) {
            // Ajout d'un sprite d'un sprite cube (obstacle à casser) et lui attribut un "id"
            Sprite* pBlock = new Sprite(GameFramework::imagesPath() + "brick.png");
            m_pParentScene->addSpriteToScene(pBlock);
            pBlock->setPos(((m_pParentScene->width() - (column * BRICK_SIZE.x())) / 2) + brickPosX, 50 + brickPosY);
            brickPosX += BRICK_SIZE.x();
            pBlock->setData(0, "brick-a-detruire");
            connect(pBlock, &Brick::destroyed, this, &Brick::onSpriteDestroyed);
        }
        // Réinitialise les valeurs et ajoutes une marge de 65 pour l'espacement des blocs.
        brickPosX = 0;
        brickPosY += BRICK_SIZE.y();
    }
}

