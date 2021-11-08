/**
  \file
  \brief    Fichier qui contient toute la logique du jeu.
  \author   CHENGAE
  \date     Octobre 2021
 */
#include "gamecore.h"

// #include <QColor>
// #include <QtCore>
// #include <QCoreApplication>
// #include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QSettings>
// #include <QString>

#include "bouncingspritehandler.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "plate.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"

// Initialisation des constantes.
const int SCENE_WIDTH = 1280;
const int SCENE_HEIGHT = SCENE_WIDTH / GameFramework::screenRatio();
const int BORDER_SIZE = 10;
const QPointF BOUNCING_AREA_SIZE(SCENE_WIDTH, SCENE_HEIGHT);
const QPointF BOUNCING_AREA_POS(0, 0);

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;

    // Trace un rectangle tout autour des limites de la scène.
    m_pSceneGame->addRect(m_pSceneGame->sceneRect(), QPen(Qt::white));

    // Création des différentes scène de jeu.
    createSceneGame();

    // Création des murs.
    setupBoucingArea();

    // Création des blocs à détruire.
    createBricks();

    // Création du plateau.
    createPlate();

    // Création de la balle.
    createBall();

    //Définis la scène actuelle du jeu.
    pGameCanvas->setCurrentScene(m_pSceneGame);

    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pSceneGame;
    m_pSceneGame = nullptr;
}

//! Cadence.
//! Gère le déplacement de la Terre qui tourne en cercle.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void GameCore::tick(long long elapsedTimeInMilliseconds) {

    if (m_isWaiting) {
        m_pBall->setPos(m_pPlate->left() + ((m_pPlate->width() / 2.0) - (m_pBall->width() / 2.0)), m_pPlate->top() - m_pBall->height());

        if (m_onClick) {
            m_isWaiting = false;
            m_pBall->registerForTick();
        }
    }
}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void GameCore::keyPressed(int key) {
    emit notifyKeyPressed(key);
}

//! Traite le relâchement d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void GameCore::keyReleased(int key) {
    emit notifyKeyReleased(key);
}

//! La souris a été déplacée.
//! Pour que cet événement soit pris en compte, la propriété MouseTracking de GameView
//! doit être enclenchée avec GameCanvas::startMouseTracking().
void GameCore::mouseMoved(QPointF newMousePosition) {
    emit notifyMouseMoved(newMousePosition);
}

//! Traite l'appui sur un bouton de la souris.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    if (buttons.testFlag(Qt::LeftButton)) {
        m_onClick = true;
    }
}

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}

//! Désincrémente le compteur si une brique est détruite.
void GameCore::onSpriteDestroyed() {
    m_counterBricks--;
}


//! Met en place le plateau.
void GameCore::createPlate() {
    Plate* pPlate = new Plate;
    pPlate->setPos((m_pSceneGame->width()/2.0)-(pPlate->width()/2.0), m_pSceneGame->height()-100.0);
    m_pSceneGame->addSpriteToScene(pPlate);
    connect(this, &GameCore::notifyKeyPressed, pPlate, &Plate::keyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlate, &Plate::keyReleased);
    connect(this, &GameCore::notifyMouseMoved, pPlate, &Plate::mouseMoved);
    m_pPlate = pPlate;
}

//! Met en place les bordures autour de la zone de jeu.
void GameCore::setupBoucingArea() {

    // Création des bordures de délimitation de la zone et placement
    QPixmap smallBorder(GameFramework::imagesPath() + "border.png");
    smallBorder = smallBorder.scaled(BORDER_SIZE, BORDER_SIZE);

    // Création d'une image faite d'une suite horizontale de bordure
    QPixmap horizontalWall(BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE), BORDER_SIZE);
    QPainter painterHW(&horizontalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE) / BORDER_SIZE); col++)
        painterHW.drawPixmap(col * BORDER_SIZE,0, smallBorder);

    // Création d'une image faite d'une suite verticale de bordure
    QPixmap verticalWall(BORDER_SIZE, BOUNCING_AREA_SIZE.y());
    QPainter painterVW(&verticalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.y() / BORDER_SIZE); col++)
        painterVW.drawPixmap(0, col * BORDER_SIZE, smallBorder);

    // Ajout de 3 sprites (utilisant les murs horizontaux et verticaux) pour délimiter une zone de rebond.
    m_pSceneGame->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y() - BORDER_SIZE);
    m_pSceneGame->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y());
    m_pSceneGame->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() + BOUNCING_AREA_SIZE.x(), BOUNCING_AREA_POS.y());


}

//! Met en place la balle qui rebondit.
void GameCore::createBall() {
    Sprite* pBall = new Sprite(GameFramework::imagesPath() + "ball.png");
    m_pSceneGame->addSpriteToScene(pBall);
    m_pBall = pBall;
    pBall->setTickHandler(new BouncingSpriteHandler);
    pBall->setPos(((m_pSceneGame->width() / 2.0) - (pBall->width() / 2.0)), m_pPlate->top() - pBall->height());
}

//! Créer les briques à détruire.
void GameCore::createBricks() {
    const QPoint BRICK_SIZE(50, 20);
    int line = 2;
    int column = 8;

    m_counterBricks = line * column;

    for (int j = 0; j < line; j++) {

        for (int i = 0; i < column; i++) {
            // Ajout d'un sprite (brique à casser) et lui attribut un "id".
            Sprite* pBrickSprite = new Sprite(GameFramework::imagesPath() + "brick.png");
            m_pSceneGame->addSpriteToScene(pBrickSprite);
            pBrickSprite->setPos(((m_pSceneGame->width() - (column * BRICK_SIZE.x())) / 2) + m_spaceLines, 50 + m_spaceColumns);
            m_spaceLines += BRICK_SIZE.x();
            pBrickSprite->setData(0, "brick-a-detruire");
            connect(pBrickSprite, &Sprite::destroyed, this, &GameCore::onSpriteDestroyed);
        }
        // Réinitialise les valeurs et ajoutes une marge de 65 pour l'espacement des blocs.
        m_spaceLines = 0;
        m_spaceColumns += BRICK_SIZE.y();
    }
}


//! Création de la scène de jeu.
void GameCore::createSceneGame() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneGame = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}
