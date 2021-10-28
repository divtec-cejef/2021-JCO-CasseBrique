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

    // Instancier et initialiser les sprite ici :
    //Sprite* pPlate = new Sprite(GameFramework::imagesPath() + "plate.png");
    //m_pScene->addSpriteToScene(pPlate);
    //m_pPlate = pPlate;
    //pPlate->setPos((m_pScene->width()/2.0)-(pPlate->width()/2.0), m_pScene->height()-100.0);

    // Cacher le curseur de la souris
    GameFramework::hideMouseCursor();

    // Création du plateau
    setupPlate("Green");

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

        // Stop le plateau si il tape la bordure
        if (newMousePosition.x() > m_pScene->width() - m_pPlate->width()/2.0)
            positionX = m_pScene->width() - m_pPlate->width()/2.0;

         else if (newMousePosition.x() > m_pPlate->width()/2.0)
            positionX = newMousePosition.x();

        m_pPlate->setPos(positionX - m_pPlate->width()/2.0, m_pScene->height() - 100);
}

//! Traite l'appui sur un bouton de la souris.
void GameCore::mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonPressed(mousePosition, buttons);
}

//! Traite le relâchement d'un bouton de la souris.
void GameCore::mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons) {
    emit notifyMouseButtonReleased(mousePosition, buttons);
}

void GameCore::setupPlate(QString color) {
    /*
    Plate* pPlate = new Plate;
    pPlate->setPos((m_pScene->width()/2.0)-(pPlate->width()/2.0), m_pScene->height()-100.0);
    pPlate->setZValue(1);
    m_pScene->addSpriteToScene(pPlate);
    connect(this, &GameCore::notifyMouseMoved, pPlate, &Plate::mouseMoved);
    m_pPlate = pPlate;
    */
    Sprite* pPlate = new Sprite(GameFramework::imagesPath() + "plate" + color + ".png");
    m_pScene->addSpriteToScene(pPlate);
    m_pPlate = pPlate;
    pPlate->setPos((m_pScene->width()/2.0)-(pPlate->width()/2.0), m_pScene->height()-100.0);
}

