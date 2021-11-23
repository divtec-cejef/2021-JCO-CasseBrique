/**
  \file
  \brief    Méthodes utilitaires permettant d'accéder aux ressources.
  \author   CHENGAE
  \date     Octobre 2021
  \version  1.0
*/
#ifndef RESOURCES_H
#define RESOURCES_H

#include <QString>

//!
//! Espace de noms contenant les fonctions utilitaires pour les ressources.
//!
namespace BrickBreaker {
    QString resourcesPath();
    QString imagesPath();
    QString imagesPath(QString subFolder);
}
#endif // RESOURCES_H
