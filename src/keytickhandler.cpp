/**
  \file
  \brief    Définition de la classe KeyTickHandler.
  \author   JCO
  \date     août 2020
*/
#include "keytickhandler.h"

#include "sprite.h"
#include "gamescene.h"
#include "gamecore.h"

//! Constructeur
//! \param pGameCore        Pointeur sur le GameCore qui signal l'appui sur les touches du clavier.
//! \param pParentSprite    Sprite dont le déplacement doit être géré.
KeyTickHandler::KeyTickHandler(GameCore* pGameCore, Sprite* pParentSprite) : SpriteTickHandler(pParentSprite)
{
    connect(pGameCore, &GameCore::notifyKeyPressed, this, &KeyTickHandler::onKeyPressed);
    connect(pGameCore, &GameCore::notifyKeyReleased, this, &KeyTickHandler::onKeyReleased);
}

//! Initialisation du gestionnaire.
void KeyTickHandler::init() {

}

//! Cadence : détermine le mouvement que fait le sprite durant le temps écoulé.
void KeyTickHandler::tick(long long elapsedTimeInMilliseconds) {

    // Calcul de la distance parcourue par le sprite, selon sa vitesse et le temps écoulé.
    QPointF distance = elapsedTimeInMilliseconds * m_currentSpriteVelocity / 1000.;

    // Positionne la bounding box du sprite à sa prochaine position.
    QRectF nextRect = m_pParentSprite->globalBoundingBox().translated(distance);

    // Si la prochaine position reste dans les limites de la scène, le sprite
    // y est positionné. Sinon, il reste sur place.
    if (m_pParentSprite->parentScene()->isInsideScene(nextRect)) {
        m_pParentSprite->setPos(m_pParentSprite->pos() + distance);
    }

}

//! Change la vitesse de déplacement du sprite.
//! \param velocityInPixelPerSecond  Nouvelle vitesse de déplacement du sprite.
//!
void KeyTickHandler::setSpriteVelocity(double velocityInPixelPerSecond) {
    m_velocity = velocityInPixelPerSecond;
}

//! Détermine les touches qui serviront au déplacement du sprite.
//! \param rightKey Touche pour le déplacement à droite.
//! \param leftKey  Touche pour le déplacement à gauche.
//! \param upKey    Touche pour le déplacement en haut.
//! \param downKey  Touche pour le déplacement en bas.
//!
void KeyTickHandler::setKeys(int rightKey, int leftKey, int upKey, int downKey) {
    setRightKey(rightKey);
    setLeftKey(leftKey);
    setUpKey(upKey);
    setDownKey(downKey);
}

//! Met à jour la vitesse de déplacement du sprite en fonction des touches
//! qui sont appuyées.
void KeyTickHandler::updateSpriteVelocity() {
    double XVelocity = 0;
    double YVelocity = 0;
    if (m_keyUpPressed)    YVelocity = -m_velocity ;
    if (m_keyDownPressed)  YVelocity = m_velocity ;
    if (m_keyRightPressed) XVelocity = m_velocity ;
    if (m_keyLeftPressed)  XVelocity = -m_velocity ;

    m_currentSpriteVelocity = QPointF(XVelocity, YVelocity);

}

//! Une touche a été appuyée.
//! \param key Code de la touche appuyée.
void KeyTickHandler::onKeyPressed(int key) {
    if (key == m_upKey)
        m_keyUpPressed = true;
    else if (key == m_downKey)
        m_keyDownPressed = true;
    else if (key == m_rightKey)
        m_keyRightPressed = true;
    else if (key == m_leftKey)
        m_keyLeftPressed = true;

    updateSpriteVelocity();
}

//! Une touche a été relâchée.
//! \param key Code de la touche relâchée.
void KeyTickHandler::onKeyReleased(int key) {
    if (key == m_upKey)
        m_keyUpPressed = false;
    else if (key == m_downKey)
        m_keyDownPressed = false;
    else if (key == m_rightKey)
        m_keyRightPressed = false;
    else if (key == m_leftKey)
        m_keyLeftPressed = false;

    updateSpriteVelocity();
}
