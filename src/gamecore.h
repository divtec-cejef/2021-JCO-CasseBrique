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

    void initGame();
    void restartGame();
    void tick(long long elapsedTimeInMilliseconds);

signals:
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);
    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

    void notifyOnPause();
    void notifyOnResume();

private:
    /***** Game Scènes *****/
    GameCanvas* m_pGameCanvas = nullptr;
    GameScene* m_pSceneStart = nullptr;
    GameScene* m_pSceneGame = nullptr;
    GameScene* m_pSceneMenu = nullptr;
    GameScene* m_pSceneWin = nullptr;
    GameScene* m_pSceneLoss = nullptr;


    /***** Fonctions *****/
    // Scènes
    void createSceneStart();
    void createSceneGame();
    void createSceneMenu();
    void createSceneWin();
    void createSceneLoss();
    void changeCurrentScene(GameScene* pScene);

    // Eléments du jeux
    void setupBoucingArea();
    void createBricks();
    void createPlate();
    void createBall();
    void createLife();


    /***** Sprites *****/
    Sprite* m_pLogoTitle = nullptr;
    Sprite* m_pLogoGame = nullptr;
    Sprite* m_pLogoMenu = nullptr;
    Sprite* m_pLogoWin = nullptr;
    Sprite* m_pLogoLoss = nullptr;
    Sprite* m_pBTStartStart = nullptr;
    Sprite* m_pBTStartExit = nullptr;
    Sprite* m_pBTMenuResume = nullptr;
    Sprite* m_pBTMenuNewGame = nullptr;
    Sprite* m_pBTMenuExit = nullptr;
    Sprite* m_pBTWinNewGame = nullptr;
    Sprite* m_pBTWinExit = nullptr;
    Sprite* m_pBTLossNewGame = nullptr;
    Sprite* m_pBTLossExit = nullptr;
    Sprite* m_pPlate = nullptr;
    Sprite* m_pBall = nullptr;


    /***** Booléen *****/
    bool m_pOnClick = false;
    bool m_pIsWaiting = true;


    /***** Int *****/
    int m_pPlayerLife = 0;
    int m_pCounterBall = 0;
    int m_pCounterBricks = 0;


    /***** Listes *****/
    QList<Sprite*> m_pPlayerLifeList = {};
    QList<QString> m_pBrickColors = {"Blue", "Cyan", "Gray", "Green", "Orange", "Pink", "Red", "Yellow"};

private slots:
    void onBallDestroyed();
    void onBrickDestroyed();
};


#endif // GAMECORE_H
