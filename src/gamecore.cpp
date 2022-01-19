/**
  \file
  \brief    Fichier qui contient toute la logique du jeu.
  \author   CHENGAE
  \date     Octobre 2021
 */
#include "gamecore.h"

#include <cmath>
#include <random>
#include <ctime>
#include <windows.h>

#include <QColor>
#include <QtCore>
#include <QCoreApplication>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScale>
#include <QPainter>
#include <QSettings>
#include <QString>
#include <QTime>
#include <QTimer>

#include "ball.h"
#include "bouncingspritehandler.h"
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
const int PLAYER_LIFES = 3;
const QPoint BRICK_SIZE(65, 20);
const QPointF BOUNCING_AREA_POS(0, 0);
const QPointF BOUNCING_AREA_SIZE(SCENE_WIDTH, SCENE_HEIGHT);

//! Initialise le contrôleur de jeu.
//! \param pGameCanvas  GameCanvas pour lequel cet objet travaille.
//! \param pParent      Pointeur sur le parent (afin d'obtenir une destruction automatique de cet objet).
GameCore::GameCore(GameCanvas* pGameCanvas, QObject* pParent) : QObject(pParent) {

    // Initialise le générateur pseudo-aléatoire.
    std::srand(std::time(nullptr));

    // Mémorise l'accès au canvas (qui gère le tick et l'affichage d'une scène).
    m_pGameCanvas = pGameCanvas;

    // Crée les scènes de menu.
    createSceneStart();
    createSceneMenu();
    createSceneWin();
    createSceneLoss();

    // Définis la scène actuelle (Scène de démarrage).
    pGameCanvas->setCurrentScene(m_pSceneStart);

    // Initalise tout les éléments du du jeu.
    initGame();

    // Démarre le tick pour que les animations qui en dépendent fonctionnent correctement.
    m_pGameCanvas->startTick();
}

//! Destructeur de GameCore : efface les scènes
GameCore::~GameCore() {
    delete m_pSceneGame;
    m_pSceneGame = nullptr;
}

//! Initialise les éléments du jeu.
void GameCore::initGame() {    
    createSceneGame();  // Création de la scène de jeu.
    setupBoucingArea(); // Création des murs.
    createBricks();     // Création des blocs à détruire.
    createPlate();      // Création du plateau.
    createBall();       // Création de la balle.
    createLife();       // Création de l'UI.
}

//! Reinitialise les éléments du jeu et change la scène actuelle.
void GameCore::restartGame() {
    m_pCounterBall = 0;
    initGame();
    changeCurrentScene(m_pSceneGame);
}


//! Cadence.
//! Gère le déplacement de la Terre qui tourne en cercle.
//! \param elapsedTimeInMilliseconds  Temps écoulé depuis le dernier appel.
void GameCore::tick(long long elapsedTimeInMilliseconds) {
    if (m_pIsWaiting) {
        m_pBall->setPos(m_pPlate->left() + ((m_pPlate->width() / 2.0) - (m_pBall->width() / 2.0)), m_pPlate->top() - m_pBall->height());

        if (m_pOnClick) {
            m_pBall->registerForTick();
            m_pOnClick = false;
            m_pIsWaiting = false;
            m_pBall = nullptr;
        }
    }

    if (m_pCounterBricks == 0 && m_pPlayerLife > 0) {
        changeCurrentScene(m_pSceneWin);
    }

    if (m_pCounterBricks > 0 && m_pPlayerLife == 0) {
        changeCurrentScene(m_pSceneLoss);
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
        changeCurrentScene(m_pSceneMenu);
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

        /***** Scène de départ *****/
        if (m_pGameCanvas->currentScene() == m_pSceneStart) {
            // Vérifie si il est positionner sur le bouton Start.
            if (m_pBTStartStart == m_pSceneStart->spriteAt(mousePosition)) {
                changeCurrentScene(m_pSceneGame);

            // Vérifie si il est positionner sur le bouton Exit.
            } else if (m_pBTStartExit == m_pSceneStart->spriteAt(mousePosition)) {
                exit(0);
            }


        /***** Scène de menu *****/
        } else if (m_pGameCanvas->currentScene() == m_pSceneMenu) {
            // Vérifie si il est positionner sur le bouton Resume.
            if (m_pBTMenuResume == m_pSceneMenu->spriteAt(mousePosition)) {
                changeCurrentScene(m_pSceneGame);
                emit notifyOnResume();

            // Vérifie si il est positionner sur le bouton New Game.
            } else if (m_pBTMenuNewGame == m_pSceneMenu->spriteAt(mousePosition)) {
                restartGame();

            // Vérifie si il est positionner sur le bouton Exit.
            } else if (m_pBTMenuExit == m_pSceneMenu->spriteAt(mousePosition)) {
                exit(0);
            }


        /***** Scène de jeu *****/
        } else if (m_pGameCanvas->currentScene() == m_pSceneGame) {
            // Vérifie si la balle est lancée.
            // Si non : la balle est lancée.
            if (m_pIsWaiting)
                m_pOnClick = true;


        /***** Scène de victoire *****/
        } else if (m_pGameCanvas->currentScene() == m_pSceneWin) {
            // Vérifie si il est positionner sur le bouton New Game.
            if (m_pBTWinNewGame == m_pSceneWin->spriteAt(mousePosition)) {
                restartGame();

            // Vérifie si il est positionner sur le bouton Exit.
            } else if (m_pBTWinExit == m_pSceneWin->spriteAt(mousePosition)) {
                exit(0);
            }


        /***** Scène de partie perdu *****/
        } else if (m_pGameCanvas->currentScene() == m_pSceneLoss) {
            // Vérifie si il est positionner sur le bouton New Game.
            if (m_pBTLossNewGame == m_pSceneLoss->spriteAt(mousePosition)) {
                restartGame();

            // Vérifie si il est positionner sur le bouton Exit.
            } else if (m_pBTLossExit == m_pSceneLoss->spriteAt(mousePosition)) {
                exit(0);
            }
        }
    }
}

//! Traite le relâchement d'un bouton de la souris.
//! \param mousePosition
//! \param buttons
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}


//! Met en place les bordures autour de la zone de jeu.
//! Met en place le rectangle autour de la zone de jeu.
void GameCore::setupBoucingArea() {
    // Création des bordures de délimitation de la zone et placement.
    QPixmap border = QPixmap(BrickBreaker::imagesPath() + "border.png");
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

//! Créer le plateau que le joueur contrôle.
//! Positionne le plateau et l'ajoute à la scène de jeu.
//! Ajoute le plateau à la cadence et envoie une notifications lorsque la souris est bougée.
void GameCore::createPlate() {
    Plate* pPlate = new Plate;
    pPlate->setPos((m_pSceneGame->width()/2.0)-(pPlate->width()/2.0), m_pSceneGame->height()-100.0);
    m_pSceneGame->addSpriteToScene(pPlate);
    pPlate->registerForTick();
    connect(this, &GameCore::notifyMouseMoved, pPlate, &Plate::onMouseMoved);
    m_pPlate = pPlate;
}

//! Créer les briques avec des couleurs aléatoires.
//! Positionne les briques selon une liste et les ajoutes à la scéne de jeu.
//! Lorsque des briques grises sont générés, elles sont indéstructiblent.
void GameCore::createBricks() {
    int spaceLines = 0;
    int spaceColumns = 0;
    QList<int> brickBuilder = {8, 12, 10};

    int minRandomColor = 1;
    int maxRandomColor = m_pBrickColors.length();

    m_pCounterBricks = 0;

    for (int j = 0; j < brickBuilder.length(); j++) {
        m_pCounterBricks += brickBuilder[j];

        for (int i = 0; i < brickBuilder[j]; i++) {
            QString color = m_pBrickColors[(rand() % maxRandomColor + minRandomColor) - 1];

            // Ajout d'un sprite (brique à casser) et lui attribut un "id".
            Sprite* pBrick = new Sprite(BrickBreaker::imagesPath() + "brick" + color + ".png");
            pBrick->setPos(((m_pSceneGame->width() - (brickBuilder[j] * BRICK_SIZE.x())) / 2) + spaceLines, 50 + spaceColumns);
            pBrick->setData(0, "brick");
            pBrick->setScale(0.5);
            if (color == "Gray") {
                pBrick->setData(1, "unbreakable");
                m_pCounterBricks--;
            }
            m_pSceneGame->addSpriteToScene(pBrick);
            connect(pBrick, &Sprite::destroyed, this, &GameCore::onBrickDestroyed);
            spaceLines += BRICK_SIZE.x();
        }

        // Réinitialise les valeurs et ajoutes une marge de 65 pour l'espacement des blocs.
        spaceLines = 0;
        spaceColumns += BRICK_SIZE.y();
    }
}

//! Créer une balle qui rebondit.
//! Positionne la balle et l'ajoute à la scène de jeu.
//! Envoie une notifications lorsque la balle est détruite, que le jeu est mit en pause et qu'il n'est plus en pause.
void GameCore::createBall() {
    Ball* pBall = new Ball;
    m_pCounterBall++;
    m_pSceneGame->addSpriteToScene(pBall);
    connect(pBall, &Ball::destroyed, this, &GameCore::onBallDestroyed);
    connect(this, &GameCore::notifyOnResume, pBall, &Ball::onResumeTick);
    connect(this, &GameCore::notifyOnPause, pBall, &Ball::onPauseTick);
    m_pBall = pBall;

    m_pIsWaiting = true;
}

//! Créer les coeurs qui représente les vies.
//! Positionne les coeurs et les ajoutes à la scène de jeu.
void GameCore::createLife() {
    m_pPlayerLife = PLAYER_LIFES;
    m_pPlayerLifeList = {};

    int margin = BORDER_SIZE + 5;

    for(int i = 0; i < PLAYER_LIFES; i++) {
        Sprite* heart = new Sprite(BrickBreaker::imagesPath("GameUI") + "heart.png");
        heart->setScale(0.2);

        int posX = 0;
        if (i>0) {
            posX = m_pPlayerLifeList[i - 1]->right();
        }

        m_pPlayerLifeList.append(heart);
        m_pSceneGame->addSpriteToScene(heart, posX, -heart->height() - margin);
    }
}


//! Création de la scène de démarrage du jeu.
//! Crée le titre et les boutons de la scène.
//! Positionne le titre et les boutons et les ajoutes à la scène de démarrage.
void GameCore::createSceneStart() {
    // Créé la scène de démarrage du jeu.
    m_pSceneStart = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // Créé le titre et les boutons avec leurs images.
    m_pLogoTitle = new Sprite(BrickBreaker::imagesPath() + "logoTitle.png");
    m_pBTStartStart = new Sprite(BrickBreaker::imagesPath("GameUI") + "start.png");
    m_pBTStartExit = new Sprite(BrickBreaker::imagesPath("GameUI") + "exit.png");

    // Ajoute et positionne les sprites précédement crées.
    m_pSceneStart->addSpriteToScene(m_pLogoTitle, (SCENE_WIDTH / 2) - (m_pLogoTitle->width() / 2), (SCENE_HEIGHT / 4) - (m_pLogoTitle->height() / 2));
    m_pSceneStart->addSpriteToScene(m_pBTStartStart, (SCENE_WIDTH / 2) - (m_pBTStartStart->width() / 2), m_pLogoTitle->bottom() + (m_pBTStartStart->height() / 2));
    m_pSceneStart->addSpriteToScene(m_pBTStartExit, (SCENE_WIDTH / 2) - (m_pBTStartExit->width() / 2), m_pBTStartStart->bottom());
}

//! Création de la scène de jeu.
//! Ajoute une image de fond à la scène.
void GameCore::createSceneGame() {
    // Créé la scène de base.
    m_pSceneGame = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // Définie l'image de fond de la scène.
    m_pSceneGame->setBackgroundImage(QImage(BrickBreaker::imagesPath() + "background.jpg"));

    // Créé le titre, l'ajoute et le positionne.
    m_pLogoGame = new Sprite(BrickBreaker::imagesPath() + "logoTitle.png");
    m_pLogoGame->setScale(0.7);
    m_pSceneGame->addSpriteToScene(m_pLogoGame, (SCENE_WIDTH / 2) - (m_pLogoGame->width() / 2), -m_pLogoGame->height() - (BORDER_SIZE * 1.5));
}

//! Création de la scène du menu du jeu.
//! Crée le titre et les boutons de la scène.
//! Positionne le titre et les boutons et les ajoutes à la scène.
void GameCore::createSceneMenu() {
    // Créé la scène de base.
    m_pSceneMenu = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // Créé le titre et les boutons avec leurs images.
    m_pLogoMenu = new Sprite(BrickBreaker::imagesPath("GameUI") + "menu.png");
    m_pBTMenuResume = new Sprite(BrickBreaker::imagesPath("GameUI") + "resume.png");
    m_pBTMenuNewGame = new Sprite(BrickBreaker::imagesPath("GameUI") + "newGame.png");
    m_pBTMenuExit = new Sprite(BrickBreaker::imagesPath("GameUI") + "exit.png");

    // Ajoute et positionne les sprites précédement crées.
    m_pSceneMenu->addSpriteToScene(m_pLogoMenu, (SCENE_WIDTH / 2) - (m_pLogoMenu->width() / 2), (SCENE_HEIGHT / 5) - m_pLogoMenu->height() / 2);
    m_pSceneMenu->addSpriteToScene(m_pBTMenuResume, (SCENE_WIDTH / 2) - (m_pBTMenuResume->width() / 2), m_pLogoMenu->bottom() + (m_pBTMenuResume->height() / 2));
    m_pSceneMenu->addSpriteToScene(m_pBTMenuNewGame, (SCENE_WIDTH / 2) - (m_pBTMenuNewGame->width() / 2), m_pBTMenuResume->bottom());
    m_pSceneMenu->addSpriteToScene(m_pBTMenuExit, (SCENE_WIDTH / 2) - (m_pBTMenuExit->width() / 2), m_pBTMenuNewGame->bottom());
}

//! Création de la scène de victoire.
//! Crée le titre et les boutons de la scène.
//! Positionne le titre et les boutons et les ajoutes à la scène.
void GameCore::createSceneWin() {
    // Créé la scène de base.
    m_pSceneWin = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // Créé le titre et les boutons avec leurs images.
    m_pLogoWin = new Sprite(BrickBreaker::imagesPath("GameUI") + "victory.png");
    m_pBTWinNewGame = new Sprite(BrickBreaker::imagesPath("GameUI") + "newGame.png");
    m_pBTWinExit = new Sprite(BrickBreaker::imagesPath("GameUI") + "exit.png");

    // Ajoute et positionne les sprites précédement crées.
    m_pSceneWin->addSpriteToScene(m_pLogoWin, (SCENE_WIDTH / 2) - (m_pLogoWin->width() / 2), (SCENE_HEIGHT / 5) - m_pLogoWin->height() / 2);
    m_pSceneWin->addSpriteToScene(m_pBTWinNewGame, (SCENE_WIDTH / 2) - (m_pBTWinNewGame->width() / 2), m_pLogoWin->bottom() + (m_pBTWinNewGame->height() / 2));
    m_pSceneWin->addSpriteToScene(m_pBTWinExit, (SCENE_WIDTH / 2) - (m_pBTWinExit->width() / 2), m_pBTWinNewGame->bottom());
}

//! Création de la scène de partie perdu.
//! Crée le titre et les boutons de la scène.
//! Positionne le titre et les boutons et les ajoutes à la scène.
void GameCore::createSceneLoss() {
    // Créé la scène de base et indique au canvas qu'il faut l'afficher.
    m_pSceneLoss = m_pGameCanvas->createScene(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // Créé le titre et les boutons avec leurs images.
    m_pLogoLoss = new Sprite(BrickBreaker::imagesPath("GameUI") + "gameover.png");
    m_pBTLossNewGame = new Sprite(BrickBreaker::imagesPath("GameUI") + "newGame.png");
    m_pBTLossExit = new Sprite(BrickBreaker::imagesPath("GameUI") + "exit.png");

    // Ajoute et positionne les sprites précédement crées.
    m_pSceneLoss->addSpriteToScene(m_pLogoLoss, (SCENE_WIDTH / 2) - (m_pLogoLoss->width() / 2), (SCENE_HEIGHT / 5) - m_pLogoLoss->height() / 2);
    m_pSceneLoss->addSpriteToScene(m_pBTLossNewGame, (SCENE_WIDTH / 2) - (m_pBTLossNewGame->width() / 2), m_pLogoLoss->bottom() + (m_pBTLossNewGame->height() / 2));
    m_pSceneLoss->addSpriteToScene(m_pBTLossExit, (SCENE_WIDTH / 2) - (m_pBTLossExit->width() / 2), m_pBTLossNewGame->bottom());
}

//! Change la scène actuelle et la visibilité du curseur de la souris.
//! \param pScene prochaine scène à afficher.
//! Si la prochaine scène est la scène de jeu : cache le curseur.
//! Si la prochaine scène est une autre scène : affiche le curseur.
void GameCore::changeCurrentScene(GameScene *pScene) {
    if (pScene == m_pSceneGame) {
        //BrickBreaker::hideMouseCursor();
        m_pGameCanvas->setCurrentScene(pScene);

    } else {
        //BrickBreaker::showMouseCursor();
        m_pGameCanvas->setCurrentScene(pScene);
    }
}


//! Désincrémente le compteur de balle lorsqu'une balle est détruite
//! et vérifie si il reste encore des balles en jeu sinon enlève une
//! vie au joueur et recrée une nouvelle balle.
void GameCore::onBallDestroyed() {
    m_pCounterBall--;

    if (m_pCounterBall == 0) {
        m_pPlayerLife--;

        if (m_pPlayerLifeList.size() > 0) {
            Sprite* heart = m_pPlayerLifeList[m_pPlayerLife];
            if (heart) {
                heart->clearAnimationFrames();
                heart->addAnimationFrame(BrickBreaker::imagesPath("GameUI") + "heartbroken.png");
            }
        }
        createBall();
    }
}

//! Désincrémente le compteur si une brique est détruite.
void GameCore::onBrickDestroyed() {
    m_pCounterBricks--;
}

