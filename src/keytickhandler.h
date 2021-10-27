/**
  \file
  \brief    Déclaration de la classe KeyTickHandler.
  \author   JCO
  \date     août 2020
*/

#ifndef KEYTICKHANDLER_H
#define KEYTICKHANDLER_H

#include <QObject>
#include <QPointF>

#include "spritetickhandler.h"

//! \brief Classe qui gère le déplacement par d'un sprite au moyen des flèches
//! du clavier.
//!

class GameCore;

class KeyTickHandler : public QObject, public SpriteTickHandler
{
    Q_OBJECT

public:
    KeyTickHandler(GameCore* pGameCore, Sprite* pParentSprite = nullptr);

    virtual void init();
    virtual void tick(long long elapsedTimeInMilliseconds);

    void setSpriteVelocity(double velocityInPixelPerSecond);

    void setRightKey(int key) { m_rightKey = key; }
    void setLeftKey(int key)  { m_leftKey = key; }
    void setUpKey(int key)    { m_upKey = key; }
    void setDownKey(int key)  { m_downKey = key; }

    void setKeys(int rightKey, int leftKey, int upKey, int downKey);

private:
    void updateSpriteVelocity();

    QPointF m_currentSpriteVelocity = QPointF(0,0);
    double m_velocity = 250.0;
    bool m_keyUpPressed = false;
    bool m_keyDownPressed = false;
    bool m_keyLeftPressed = false;
    bool m_keyRightPressed = false;

    int m_rightKey = Qt::Key_Right;
    int m_leftKey = Qt::Key_Left;
    int m_upKey = Qt::Key_Up;
    int m_downKey = Qt::Key_Down;

private slots:
    void onKeyPressed(int key);
    void onKeyReleased(int key);

};

#endif // KEYTICKHANDLER_H
