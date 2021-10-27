#include "utilities.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

namespace GameFramework {

    //! \return le rapport entre la largeur de l'écran et sa hauteur.
    double screenRatio() {
        return static_cast<double>(screenSize().width()) / screenSize().height();
    }

    //! \return la taille en pixels de l'écran.
    QSize screenSize() {
        //QDesktopWidget* pDefaultScreen = QApplication::desktop();
        QScreen* pDefaultScreen = QGuiApplication::screens().first();
        QRect ScreenRect = pDefaultScreen->geometry(); // pDefaultScreen->screenGeometry();
        return ScreenRect.size();
    }

    //! Cache le curseur de la souris.
    //! \see showMouseCursor()
    void hideMouseCursor() {
        qApp->setOverrideCursor(QCursor(Qt::BlankCursor));
    }

    //! Rend apparent le curseur de la souris.
    //! \see hideMouseCursor()
    void showMouseCursor() {
        qApp->restoreOverrideCursor();
    }


}
