#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"

class Player : public Sprite
{
    Q_OBJECT

public:
    Player(QGraphicsItem* pParent = nullptr);

    virtual void tick(long long elapsedTimeInMilliseconds);

public slots:
    void onKeyPressed(int key);
    void onKeyReleased(int key);

private:
    void configureAnimation();
    void updateBallVelocity();

    QPointF m_ballVelocity;
    bool m_keyUpPressed;
    bool m_keyDownPressed;
    bool m_keyLeftPressed;
    bool m_keyRightPressed;

};

#endif // PLAYER_H
