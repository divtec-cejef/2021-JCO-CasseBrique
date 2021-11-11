/**
  \file
  \brief    Déclaration de la classe GameCore.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef GAMECORE_H
#define GAMECORE_H

#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QObject>
#include <QPointF>
#include <QString>

class GameCanvas;
class GameScene;
class Sprite;

//! Classe qui gère la logique du jeu.
//! Dans son état actuel, cette classe crée une scène vide, délimite
//! sa surface avec une ligne blanche puis démarre la cadence du jeu.
class GameCore : public QObject {
    Q_OBJECT

public:
    explicit GameCore(GameCanvas* pGameCanvas, QObject *parent = nullptr);
    ~GameCore();

    void keyPressed(int key);
    void keyReleased(int key);

    void mouseMoved(QPointF newMousePosition);
    void mouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void mouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

    void tick(long long elapsedTimeInMilliseconds);

signals:
    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);

    void notifyOnStart();
    void notifyOnPause();
    void notifyOnResume();

private:
    void createSceneStart();
    void createSceneGame();
    void createSceneMenu();
    void createSceneWin();
    void createSceneLoss();
    void createButton();
    void createText();

    void setupBoucingArea();
    void createBricks();
    void createPlate();
    void createBall();


    GameCanvas* m_pGameCanvas = nullptr;
    GameScene* m_pSceneStart = nullptr;
    GameScene* m_pSceneGame = nullptr;
    GameScene* m_pSceneMenu = nullptr;
    GameScene* m_pSceneWin = nullptr;
    GameScene* m_pSceneLoss = nullptr;

    QGraphicsSimpleTextItem* m_pTextGameLife = nullptr;
    QGraphicsSimpleTextItem* m_pTextGameBrick = nullptr;


    Sprite* m_pPlate = nullptr;
    Sprite* m_pBall = nullptr;

    Sprite* m_pButtonStart = nullptr;
    Sprite* m_pButtonResume = nullptr;
    Sprite* m_pButtonNewGame = nullptr;
    Sprite* m_pButtonExit = nullptr;


    bool m_onClick = false;
    bool m_isDead = false;
    bool m_isRestart = false;
    bool m_isWaiting = true;


    int m_playerLife = 3;
    int m_spaceLines = 0;
    int m_spaceColumns = 0;
    int m_counterBall = 0;
    int m_counterBricks = 0;

private slots:
    void onBallDestroyed();
    void onBrickDestroyed();
};


#endif // GAMECORE_H
