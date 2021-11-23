/**
  \file
  \brief    Déclaration de la classe Ball.
  \author   CHENGAE
  \date     Novembre 2021
*/
#ifndef BALL_H
#define BALL_H

#include "sprite.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>

class GameScene;
class SpriteTickHandler;

class Ball : public Sprite {
    // Nécessaire de déclarer cette macro pour que la classe puisse
    // bénéficier du mécanisme de signaux/slots.
    Q_OBJECT

public:
    Ball(const QPixmap& rPixmap, QGraphicsItem* pParent = nullptr);

    void tick(long long elapsedTimeInMilliseconds);

    void setSpriteVelocity(QPointF spriteVelocity);
    void setSpriteVelocity(double xVelocity, double yVelocity);
    QPointF getSpriteVelocity();

public slots:
    void onResumeTick();
    void onPauseTick();

private:
    QPointF m_spriteVelocity;
};

#endif // BALL_H
