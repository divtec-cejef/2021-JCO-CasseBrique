/**
  \file
  \brief    Déclaration de la classe Plate.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef PLATE_H
#define PLATE_H

#include "sprite.h"

class Plate : public Sprite {

public:
    Plate(QGraphicsItem* pParent = nullptr);

    virtual void tick(long long elapsedTimeInMilliseconds);

public slots:
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(QPointF newMousePosition);

private:
    QPointF m_velocity;
    bool m_keyLeftPressed  = false;
    bool m_keyRightPressed = false;

    void updateVelocity();
    void resetKeyState();
};

#endif // PLATE_H
