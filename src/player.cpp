#include "player.h"
#include "gamescene.h"

#include "resources.h"

const int LINE_COUNT = 4;
const int FRAME_COUNT = 12;

const int FRAME_WIDTH = 95;
const int FRAME_HEIGHT = 158;
const int SCALE_RATIO = 1;

const int PLAYER_VELOCITY = 80; // pixels par seconde

enum Player_Animation {
    WALKING_DOWN = 0,
    WALKING_LEFT,
    WALKING_RIGHT,
    WALKING_UP
};

Player::Player(QGraphicsItem* pParent) : Sprite(pParent) {
    configureAnimation();

    m_keyUpPressed    = false;
    m_keyDownPressed  = false;
    m_keyLeftPressed  = false;
    m_keyRightPressed = false;
    m_ballVelocity = QPointF(0,0);

}

//! Cadence.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void Player::tick(long long elapsedTimeInMilliseconds) {

    // Calcul de la distance parcourue par la balle, selon sa vitesse et le temps écoulé.
    QPointF ballDistance = elapsedTimeInMilliseconds * m_ballVelocity / 1000.;

    // Positionne la bounding box de la balle à sa prochaine position.
    QRectF nextRect = this->globalBoundingBox().translated(ballDistance);

    // Si la prochaine position reste dans les limites de la scène, la balle
    // y est positionnée. Sinon, elle reste sur place.
    if (parentScene()->isInsideScene(nextRect)) {
        this->setPos(this->pos() + ballDistance);
    }
}

//! Une touche a été appuyée.
//! \param key Code de la touche appuyée.
void Player::onKeyPressed(int key) {
    switch (key)  {
    case Qt::Key_W:    m_keyUpPressed    = true;  updateBallVelocity(); break;
    case Qt::Key_S:  m_keyDownPressed  = true;  updateBallVelocity(); break;
    case Qt::Key_D: m_keyRightPressed = true;  updateBallVelocity(); break;
    case Qt::Key_A:  m_keyLeftPressed  = true;  updateBallVelocity(); break;
    }
}

//! Une touche a été relâchée.
//! \param key Code de la touche relâchée.
void Player::onKeyReleased(int key) {
    switch (key)  {
    case Qt::Key_W:    m_keyUpPressed    = false;  updateBallVelocity(); break;
    case Qt::Key_S:  m_keyDownPressed  = false;  updateBallVelocity(); break;
    case Qt::Key_D: m_keyRightPressed = false;  updateBallVelocity(); break;
    case Qt::Key_A:  m_keyLeftPressed  = false;  updateBallVelocity(); break;
    }

}

//! Met à jour le vecteur de vitesse de la balle en fonction des touches appuyées.
void Player::updateBallVelocity()  {
    int XVelocity = 0;
    int YVelocity = 0;
    if (m_keyUpPressed) {
        YVelocity = -PLAYER_VELOCITY;
        this->setActiveAnimation(WALKING_UP);
    }
    if (m_keyDownPressed) {
        YVelocity = PLAYER_VELOCITY;
        this->setActiveAnimation(WALKING_DOWN);
    }

    if (m_keyRightPressed) {
        XVelocity = PLAYER_VELOCITY;
        this->setActiveAnimation(WALKING_RIGHT);
    }

    if (m_keyLeftPressed) {
        XVelocity = -PLAYER_VELOCITY;
        this->setActiveAnimation(WALKING_LEFT);
    }

    m_ballVelocity = QPoint(XVelocity, YVelocity);

    if (XVelocity == 0 && YVelocity == 0)
        this->stopAnimation();
    else {
        this->startAnimation();
    }

}


//! Découpe la spritesheet pour en extraire les étapes d'animation et
//! les ajouter au sprite.
void Player::configureAnimation() {
    // Chargement de la spritesheet
    QImage spriteSheet(GameFramework::imagesPath() + "player.png");

    // Découpage de la spritesheet
    for (int animationIndex = 0; animationIndex < LINE_COUNT; animationIndex++) {

        while (this->animationCount() <= animationIndex)
            this->addAnimation();

        this->setActiveAnimation(animationIndex);

        for (int frameIndex = 0; frameIndex < FRAME_COUNT; frameIndex++) {
            QImage sprite = spriteSheet.copy(frameIndex * FRAME_WIDTH,
                                             animationIndex * FRAME_HEIGHT,
                                             FRAME_WIDTH, FRAME_HEIGHT);

            this->addAnimationFrame(QPixmap::fromImage(sprite.scaled(FRAME_WIDTH * SCALE_RATIO,
                                                                     FRAME_HEIGHT * SCALE_RATIO,
                                                                     Qt::IgnoreAspectRatio,
                                                                     Qt::SmoothTransformation)));
        }
    }

    this->setAnimationSpeed(100);
    this->setActiveAnimation(0);
}
