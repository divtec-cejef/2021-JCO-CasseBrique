/**
  \file
  \brief    Déclaration de la classe GameCore.
  \author   CHENGAE
  \date     Octobre 2021
*/
#ifndef GAMECORE_H
#define GAMECORE_H

#include <QCursor>
#include <QObject>
#include <QPointF>

class GameCanvas;
class GameScene;
class Sprite;

//! \brief Classe qui gère la logique du jeu.
//!
//! Dans son état actuel, cette classe crée une scène vide, délimite
//! sa surface avec une ligne blanche puis démarre la cadence du jeu.
class GameCore : public QObject
{
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

private:
    void setupPlate(QString color);
    void setupBall(QString color);
    void setupBoucingArea();

    GameCanvas* m_pGameCanvas;
    GameScene* m_pScene;

    Sprite* m_pPlate;
    Sprite* m_pBall;

    QString m_plateSize;
    QString m_plateColor;
    QString m_ballColor;
    bool m_mouseButtonPressed;

private slots:

};


#endif // GAMECORE_H
