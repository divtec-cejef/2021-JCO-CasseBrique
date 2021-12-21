/**
  \file
  \brief    Fichier qui contient toute la logique du jeu.
  \author   CHENGAE
  \date     Octobre 2021
 */
#include "gamecore.h"

#include <cmath>

#include <QColor>
#include <QtCore>
#include <QCoreApplication>
#include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QSettings>
#include <QString>

#include "ball.h"
#include "bonus.h"
#include "bouncingspritehandler.h"
#include "brick.h"
#include "gamescene.h"
#include "gamecanvas.h"
#include "plate.h"
#include "resources.h"
#include "sprite.h"
#include "utilities.h"

// Initialisation des constantes.
const int SCENE_WIDTH = 1280;
const int SCENE_HEIGHT = 720;
const int BORDER_SIZE = 10;
const QPointF BOUNCING_AREA_POS(0, 0);
const QPointF BOUNCING_AREA_SIZE(SCENE_WIDTH, SCENE_HEIGHT);

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène)
    m_pGameCanvas = pGameCanvas;

    init();

    // Définis la scène actuelle (Menu démarrage).
    pGameCanvas->setCurrentScene(m_pSceneStart);

    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pSceneGame;
    m_pSceneGame = nullptr;
}

//! Initialise les éléments du jeu.
void GameCore::init() {
    // Création des différentes scène du jeu.
    createSceneStart();
    createSceneGame();
    createSceneMenu();
    createSceneWin();
    createSceneLoss();

    // Création des boutons.
    createButton();

    // Création des murs.
    setupBoucingArea();

    // Création des blocs à détruire.
    createBricks();

    // Création du plateau.
    createPlate();

    // Création de la balle.
    createBall();

    // Création des textes.
    createText();
}

//! Reinitialise les éléments du jeu.
void GameCore::restart() {

}

//! Cadence.
//! Gère le déplacement de la Terre qui tourne en cercle.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void GameCore::tick(long long elapsedTimeInMilliseconds) {

    if (m_isWaiting) {
        m_pBall->setPos(m_pPlate->left() + ((m_pPlate->width() / 2.0) - (m_pBall->width() / 2.0)), m_pPlate->top() - m_pBall->height());

        if (m_onClick) {
            m_pBall->registerForTick();
            m_onClick = false;
            m_isWaiting = false;
            m_pBall = nullptr;
        }
    }
}

//! Traite la pression d'une touche.
//! \param key Numéro de la touche (voir les constantes Qt)
void GameCore::keyPressed(int key) {
    emit notifyKeyPressed(key);

    switch (key) {
    // A la pression de la touche ESC, affiche le menu.
    case Qt::Key_Escape:
        emit notifyOnPause();
        m_pGameCanvas->setCurrentScene(m_pSceneMenu);
        break;

    case Qt::Key_B:
        if (!m_isWaiting)
            createBall();
        break;
    }
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
//! \param mousePosition
//! \param buttons
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);

    // Au clique gauche de la souris.
    if (buttons.testFlag(Qt::LeftButton)) {

        // Si la scène actuelle est la scène de départ.
        if (m_pGameCanvas->currentScene() == m_pSceneStart) {
            // Vérifie si il est positionner sur le bouton Start.
            if (m_pButtonStart == m_pSceneStart->spriteAt(mousePosition)) {
                m_pGameCanvas->setCurrentScene(m_pSceneGame);
            }

        // Si la scène actuelle est le menu.
        } else if (m_pGameCanvas->currentScene() == m_pSceneMenu) {

            // Vérifie si il est positionner sur le bouton Resume.
            if (m_pButtonResume == m_pSceneMenu->spriteAt(mousePosition)) {
                m_pGameCanvas->setCurrentScene(m_pSceneGame);
                emit notifyOnResume();

            // Vérifie si il est positionner sur le bouton NewGame.
            } else if (m_pButtonNewGame == m_pSceneMenu->spriteAt(mousePosition)) {
                m_pGameCanvas->setCurrentScene(m_pSceneGame);
                emit notifyOnResume();

            // Vérifie si il est positionner sur le bouton Exit.
            } else if (m_pButtonExit == m_pSceneMenu->spriteAt(mousePosition)) {
                QCoreApplication::quit();
            }

        // Si la scène actuelle est le jeu.
        } else if (m_pGameCanvas->currentScene() == m_pSceneGame) {
            m_onClick = true;
        }
    }
}

//! Traite le relâchement d'un bouton de la souris.
//! \param mousePosition
//! \param buttons
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}


void GameCore::setCounterBricks(int counterBricks) {
    this->m_counterBricks = counterBricks;
}


//! Désincrémente le compteur de balle lorsqu'une balle est détruite
//! et vérifie si il reste encore des balles en jeu sinon enlève une
//! vie au joueur et recrée une nouvelle balle.
void GameCore::onBallDestroyed() {
    m_counterBall--;

    if (m_counterBall == 0) {
        m_playerLife--;
        m_pTextGameLife->setText(QString("Vie : ") + QString::number(m_playerLife));
        createBall();
    }
}

//! Désincrémente le compteur si une brique est détruite.
void GameCore::onBrickDestroyed() {
    m_counterBricks--;
    m_pTextGameBrick->setText(QString("Briques : ") + QString::number(m_counterBricks));
}


//! Créer le plateau que le joueur contrôle.
void GameCore::createPlate() {
    Plate* pPlate = new Plate;
    pPlate->setPos((m_pSceneGame->width()/2.0)-(pPlate->width()/2.0), m_pSceneGame->height()-100.0);
    m_pSceneGame->addSpriteToScene(pPlate);
    pPlate->registerForTick();
    connect(this, &GameCore::notifyKeyPressed, pPlate, &Plate::onKeyPressed);
    connect(this, &GameCore::notifyKeyReleased, pPlate, &Plate::onKeyReleased);
    connect(this, &GameCore::notifyMouseMoved, pPlate, &Plate::onMouseMoved);
    m_pPlate = pPlate;
}

//! Met en place les bordures autour de la zone de jeu.
void GameCore::setupBoucingArea() {
    // Création des bordures de délimitation de la zone et placement.
    QPixmap border(BrickBreaker::imagesPath() + "border.png");
    border = border.scaled(BORDER_SIZE, BORDER_SIZE);

    // Création d'une image faite d'une suite horizontale de bordure.
    QPixmap horizontalWall(BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE), BORDER_SIZE);
    QPainter painterHW(&horizontalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.x() + (2 * BORDER_SIZE) / BORDER_SIZE); col++)
        painterHW.drawPixmap(col * BORDER_SIZE, 0, border);

    // Création d'une image faite d'une suite verticale de bordure.
    QPixmap verticalWall(BORDER_SIZE, BOUNCING_AREA_SIZE.y());
    QPainter painterVW(&verticalWall);
    for (int col = 0; col < (BOUNCING_AREA_SIZE.y() / BORDER_SIZE); col++)
        painterVW.drawPixmap(0, col * BORDER_SIZE, border);

    // Ajout de 3 sprites (utilisant les murs horizontaux et verticaux) pour délimiter une zone de rebond.
    m_pSceneGame->addSpriteToScene(new Sprite(horizontalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y() - BORDER_SIZE);
    m_pSceneGame->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() - BORDER_SIZE, BOUNCING_AREA_POS.y());
    m_pSceneGame->addSpriteToScene(new Sprite(verticalWall), BOUNCING_AREA_POS.x() + BOUNCING_AREA_SIZE.x(), BOUNCING_AREA_POS.y());

    // Trace un rectangle tout autour des limites de la scène.
    m_pSceneGame->addRect(m_pSceneGame->sceneRect(), QPen(Qt::white));
}

//! Créer une balle qui rebondit.
void GameCore::createBall() {
    Ball* pBall = new Ball(BrickBreaker::imagesPath() + "ball.png");
    m_counterBall++;
    m_pSceneGame->addSpriteToScene(pBall);
    connect(pBall, &Ball::destroyed, this, &GameCore::onBallDestroyed);
    connect(this, &GameCore::notifyOnResume, pBall, &Ball::onResumeTick);
    connect(this, &GameCore::notifyOnPause, pBall, &Ball::onPauseTick);
    m_pBall = pBall;

    m_isWaiting = true;
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
            Sprite* pBrick = new Sprite(BrickBreaker::imagesPath() + "brick.png");
            pBrick->setPos(((m_pSceneGame->width() - (column * BRICK_SIZE.x())) / 2) + m_spaceLines, 50 + m_spaceColumns);
            pBrick->setData(0, "brick");
            m_pSceneGame->addSpriteToScene(pBrick);
            connect(pBrick, &Sprite::destroyed, this, &GameCore::onBrickDestroyed);
            m_spaceLines += BRICK_SIZE.x();
        }
        // Réinitialise les valeurs et ajoutes une marge de 65 pour l'espacement des blocs.
        m_spaceLines = 0;
        m_spaceColumns += BRICK_SIZE.y();
    }
}


//! Créer les boutons.
void GameCore::createButton() {
    m_pButtonStart = new Sprite(BrickBreaker::imagesPath("GameUI") + "start.png");

    m_pButtonResume = new Sprite(BrickBreaker::imagesPath("GameUI") + "resume.png");
    m_pButtonNewGame = new Sprite(BrickBreaker::imagesPath("GameUI") + "newGame.png");
    m_pButtonExit = new Sprite(BrickBreaker::imagesPath("GameUI") + "exit.png");

    m_pSceneStart->addSpriteToScene(m_pButtonStart, (SCENE_WIDTH / 2) - (m_pButtonStart->width() / 2), (SCENE_HEIGHT / 2) - m_pButtonStart->height() / 2);

    m_pSceneMenu->addSpriteToScene(m_pButtonResume, (SCENE_WIDTH / 2) - (m_pButtonResume->width() / 2), (SCENE_HEIGHT / 2) - (m_pButtonResume->height() / 2) - m_pButtonNewGame->height());
    m_pSceneMenu->addSpriteToScene(m_pButtonNewGame, (SCENE_WIDTH / 2) - (m_pButtonNewGame->width() / 2), (SCENE_HEIGHT / 2) - (m_pButtonNewGame->height() / 2));
    m_pSceneMenu->addSpriteToScene(m_pButtonExit, (SCENE_WIDTH / 2) - (m_pButtonExit->width() / 2), (SCENE_HEIGHT / 2) - (m_pButtonExit->height() / 2) + m_pButtonNewGame->height());
}

//! Créer les textes de l'UI (User's Interface).
void GameCore::createText() {
    m_pTextGameLife = m_pSceneGame->createText(QPointF(0, -50), QString("Vie : ") + QString::number(m_playerLife), 30);
    m_pTextGameBrick = m_pSceneGame->createText(QPointF(SCENE_WIDTH / 2, -50), QString("Briques : ") + QString::number(m_counterBricks), 30);
}

//! Création de la scène de démarrage du jeu.
void GameCore::createSceneStart() {
    m_pSceneStart = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

//! Création de la scène de jeu
void GameCore::createSceneGame() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneGame = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

//! Création de la scène du menu du jeu.
void GameCore::createSceneMenu() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneMenu = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

//! Création de la scène de jeu
void GameCore::createSceneWin() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneWin = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

//! Création de la scène de jeu
void GameCore::createSceneLoss() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneLoss = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
}

