/**
  \file
  \brief    Déclaration de la classe GameCore.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef GAMECORE_H
#define GAMECORE_H

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
    void notifyKeyPressed(int key);
    void notifyKeyReleased(int key);
    void notifyMouseMoved(QPointF newMousePosition);
    void notifyMouseButtonPressed(QPointF mousePosition, Qt::MouseButtons buttons);
    void notifyMouseButtonReleased(QPointF mousePosition, Qt::MouseButtons buttons);

private:
    GameCanvas* m_pGameCanvas = nullptr;
    GameScene* m_pSceneGame = nullptr;

    Sprite* m_pPlate = nullptr;
    Sprite* m_pBall = nullptr;
    Sprite* m_pBricks = nullptr;

    bool m_onClick = false;
    bool m_isDead = false;
    bool m_isRestart = false;
    bool m_isWaiting = true;

    int m_spaceLines = 0;
    int m_spaceColumns = 0;
    int m_counterBricks = 0;

    void createSceneWin();
    void createSceneLoss();
    void createSceneGame();
    void createSceneMenu();
    void setupBoucingArea();
    void createBricks();
    void createPlate();
    void createBall();

private slots:
    void onSpriteDestroyed();
};


#endif // GAMECORE_H
