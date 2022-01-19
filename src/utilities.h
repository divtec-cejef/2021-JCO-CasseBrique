/**
  \file
  \brief    Méthodes utilitaires diverses.
  \author   CHENGAE
  \date     Octobre 2021
  \version  1.0
*/
#ifndef UTILITIES_H
#define UTILITIES_H

#include <QSize>

//!
//! Espace de noms contenant les fonctions utilitaires.
//!
namespace BrickBreaker {
    double screenRatio();
    QSize screenSize();

    void hideMouseCursor();
    void showMouseCursor();

}
#endif // UTILITIES_H
