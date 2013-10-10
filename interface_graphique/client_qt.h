#ifndef CLIENT_QT_H
#define CLIENT_QT_H

#include <QMainWindow>

#include <QWidget>
#include <QList>
#include <QInputDialog>
#include <QStyle>
#include <QMouseEvent>

#include <QTableWidgetItem>
#include <QLabel>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QGridLayout>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

class QAction;
class QTableWidget;

class TableSQL;

class Client_qt : public QMainWindow
{
        Q_OBJECT
public:
        Client_qt(QGridLayout *grille2);
    void chargeTitre(QString &son,QString &titre, QString &artiste, QString &album, QString &annee, QString &table);
    void chargePlaylist(QString &playliste);

    QSize sizeHint() const
    {
            return QSize(800, 500);
    }

private:



        void setupMenus();
        void setupUi();
        void setupActions();
public:
        int fd_socket;
        struct sockaddr_in socket_addr;

private slots:

		// méthodes reliées aux boutons
         void actionBut1();
         void actionBut2();
         void actionBut3();
         void actionBut4();

		// appui sur bouton
         void goAimbot();

		// méthode appelée par notre timer
         void update();
         
         // relachement d'un bouton de direction
         void arretAppui();


private:

        QAction *exitAction;
        QAction *aProposAction;
        QGridLayout *grille;
        int idBouton;
        QTimer * timer;
        QPushButton *buton5;





};

#endif 
