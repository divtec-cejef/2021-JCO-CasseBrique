/**
  \file
  \brief    Fichier qui contient toute la logique du jeu.
  \author   CHENGAE
  \date     Octobre 2021
 */
#include "gamecore.h"

#include <cmath>

#include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QSettings>

#include "blueball.h"
#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "plate.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"

const int SCENE_WIDTH = 1280;
const int SCENE_HEIGHT = 1080;

const QPointF BOUNCING_AREA_POS(0, 0);

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;

    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pScene = pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_WIDTH / GameFramework::screenRatio());
    pGameCanvas->setCurrentScene(m_pScene);

    // Trace un rectangle blanc tout autour des limites de la scène.
    m_pScene->addRect(m_pScene->sceneRect(), QPen(Qt::white));

    //Initialise les variables
    m_plateSize = "3";
    m_mouseButtonPressed = false;
    m_plateColor = "White";
    m_ballColor = "White";

    // Instancier et initialiser les sprite ici :
    Sprite* pBrickRed = new Sprite(GameFramework::imagesPath() + "brickRed.png");
    m_pScene->addSpriteToScene(pBrickRed);
    pBrickRed->setPos((m_pScene->width() / 2.0) - (pBrickRed->width() / 2.0), 100.0);

    Sprite* pBrickGreen = new Sprite(GameFramework::imagesPath() + "brickGreen.png");
    m_pScene->addSpriteToScene(pBrickGreen);
    pBrickGreen->setPos((m_pScene->width() / 2.0) - (pBrickGreen->width() / 2.0) - 50.0, 100.0);

    Sprite* pBrickBlue = new Sprite(GameFramework::imagesPath() + "brickBlue.png");
    m_pScene->addSpriteToScene(pBrickBlue);
    pBrickBlue->setPos((m_pScene->width() / 2.0) - (pBrickBlue->width() / 2.0) + 50.0, 100.0);

    // Création du plateau.
    setupPlate(m_plateColor);

    // Création des murs.
    setupBoucingArea();

    // Création de la balle.
    setupBall(m_ballColor);

    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    // Attention : il est important que l'enclenchement du tick soit fait vers la fin de cette fonction,
    // sinon le temps passé jusqu'au premier tick (ElapsedTime) peut être élevé et provoquer de gros
    // déplacements, surtout si le déboggueur est démarré.
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pScene;
    m_pScene = nullptr;
}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
//!
void GameCore::keyPressed(int key) {
    emit notifyKeyPressed(key);

    switch(key) {
    case Qt::Key_Space:
        setupBall(m_ballColor);
        break;
    case Qt::Key_W:
        m_ballColor = "White";
        break;
    case Qt::Key_R:
        m_ballColor = "Red";
        break;
    case Qt::Key_G:
        m_ballColor = "Green";
        break;
    case Qt::Key_B:
        m_ballColor = "Blue";
        break;
    }
}

//! Traite le relâchement d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void GameCore::keyReleased(int key) {
    emit notifyKeyReleased(key);
}

//! Cadence.
//! Gère le déplacement de la Terre qui tourne en cercle.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void GameCore::tick(long long elapsedTimeInMilliseconds) {

}

//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void GameCore::mouseMoved(QPointF newMousePosition) {
    emit notifyMouseMoved(newMousePosition);

    qreal positionX = m_pPlate->width()/2.0; //Centre du plateau

    // Le plateau suit la souris uniquement à l'interieur du la zone de jeux et
    // le plateau s'arrète au bordure de la zone.
    if (newMousePosition.x() > 0 && newMousePosition.x() < m_pScene->width() &&
            newMousePosition.y() > 0 && newMousePosition.y() < m_pScene->height()) {

        if (newMousePosition.x() > m_pScene->width() - m_pPlate->width()/2.0)
            positionX = m_pScene->width() - m_pPlate->width()/2.0;

        else if (newMousePosition.x() > m_pPlate->width()/2.0)
            positionX = newMousePosition.x();

        m_pPlate->setPos(positionX - m_pPlate->width()/2.0, m_pScene->height() - 100);

        if (m_mouseButtonPressed == false)
            m_pBall->setX(positionX - (m_pBall->width()/2.0));
    }
}

//! Traite l'appui sur un bouton de la souris.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    if (buttons.testFlag(Qt::LeftButton)) {
        m_pBall->registerForTick();
        m_mouseButtonPressed = true;
    }
}

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}

//! Met en place le plateau.
//! \param color couleur du plateau.
void GameCore::setupPlate(QString color) {
    Sprite* pPlate = new Sprite(GameFramework::imagesPath() + "plate" + color + m_plateSize + ".png");
    m_pScene->addSpriteToScene(pPlate);
    m_pPlate = pPlate;
    pPlate->setPos((m_pScene->width()/2.0)-(pPlate->width()/2.0), m_pScene->height()-100.0);

    /*
    for (int FrameNumber = 1; FrameNumber <= 5; ++FrameNumber)  {
        pPlate->addAnimationFrame(QString(GameFramework::imagesPath() + "plate" + color + "%" + m_plateSize + ".png").arg(FrameNumber));
    }
    pPlate->setAnimationSpeed(1000);  // Passe à la prochaine image de la marche toutes les 100 ms
    pPlate->startAnimation();
    */
}

void GameCore::setupBoucingArea() {
    const int BORDER_SIZE = 10;
    const QPointF BOUNCING_AREA_SIZE(m_pScene->width(), m_pScene->height());

    // Création des briques de délimitation de la zone et placement
    QPixmap smallBrick(GameFramework::imagesPath() + "border.png");
    smallBrick = smallBrick.scaled(BORDER_SIZE, BORDER_SIZE);

    // Création d'une image faite d'une suite horizontale de briques
    QPixmap horizontalWall(BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE), BORDER_SIZE);
    QPainter painterHW(&horizontalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE) / BORDER_SIZE); col++)
        painterHW.drawPixmap(col * BORDER_SIZE,0, smallBrick);

    // Création d'une image faite d'une suite verticale de briques
    QPixmap verticalWall(BORDER_SIZE, BOUNCING_AREA_SIZE.y());
    QPainter painterVW(&verticalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.y() / BORDER_SIZE); col++)
        painterVW.drawPixmap(0, col * BORDER_SIZE, smallBrick);

    // Ajout de 4 sprites (utilisant les murs horizontaux et verticaux) pour délimiter
    // une zone de rebond.
    m_pScene->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y() - BORDER_SIZE);
    //m_pScene->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_SIZE.y());

    m_pScene->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y());
    m_pScene->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() + BOUNCING_AREA_SIZE.x(), BOUNCING_AREA_POS.y());
}

//! Met en place la balle qui rebondit.
//! \param color couleur de la balle.
void GameCore::setupBall(QString color) {
    Sprite* pBall = new Sprite(GameFramework::imagesPath() + "ball" + color + ".png");
    m_pScene->addSpriteToScene(pBall);
    m_pBall = pBall;
    pBall->setTickHandler(new BouncingSpriteHandler);
    pBall->setPos(((m_pScene->width() / 2.0) - (pBall->width() / 2.0)), m_pPlate->top() - pBall->height());

    //pBall->registerForTick();
}

