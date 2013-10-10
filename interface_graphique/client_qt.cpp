#include "client_qt.h"
#include <QtGui>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <taglib/fileref.h>
#include <taglib/tstring.h>
#include <taglib/tag.h>
#include <QUrl>
#include <time.h>


using namespace std;
using namespace TagLib;

void init_sockaddr2 (struct sockaddr_in *name, const char *hostname, uint16_t port)
{
    struct hostent *hostinfo;

    name->sin_family = AF_INET; /*adresses IPV4*/

    name->sin_port=htons(port); /*gere little/big Endian*/

    hostinfo= gethostbyname(hostname);

    if (hostinfo == NULL)	/*Si resolution de nom en adresse IP echoue*/
    {
        fprintf(stderr, "Unknown host %s.\n", hostname);
        exit(2);
    }

    name->sin_addr = *(struct in_addr *) hostinfo->h_addr_list[0];	/*copie adresse ip*/
}


/* Constructeur de notre jukebox*/
Client_qt::Client_qt(QGridLayout *grille2)
    : grille(grille2)
{

    idBouton = 0;
    timer = new QTimer(this);
    timer->setInterval(100);    // réglage de l'intervalle à 10 envoi/secc

    setupActions();
    setupMenus();
    setupUi();

    if( (fd_socket=socket(AF_INET,SOCK_STREAM,0)) == -1 )
    {
        perror("Probleme socket() client");
        exit(1);
    };

    init_sockaddr2(&socket_addr,"127.0.0.1",4019);




}




/* Permet de mettre en plasce toutes les actions comme play,  etc..
Fait egalement els différents connect */

void Client_qt::setupActions()
{
    QPushButton *buton1 = new QPushButton(QIcon("./flecheHaut.ico"),"");
    QPushButton *buton2 = new QPushButton(QIcon("./flecheGauche.ico"),"");
    QPushButton *buton3 = new QPushButton(QIcon("./flecheBas.ico"),"");
    QPushButton *buton4 = new QPushButton(QIcon("./flecheDroite.jpg"),"");

    buton5 = new QPushButton(QIcon("./croix.jpg"),"");


    grille->addWidget(buton1,1,2);
    grille->addWidget(buton2,2,1);
    grille->addWidget(buton5,2,2);
    grille->addWidget(buton4,2,3);

    grille->addWidget(buton3,3,2);
    grille->setEnabled(true);

    // le timer appelera cette méthode à un rythme continu (périodicité fixe)
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    connect( buton1, SIGNAL(pressed()),this, SLOT(actionBut1()));
    connect( buton1, SIGNAL(clicked()),this, SLOT(arretAppui()));

    connect( buton2, SIGNAL(pressed()),this, SLOT(actionBut2()));
    connect( buton2, SIGNAL(clicked()),this, SLOT(arretAppui()));

    connect( buton3, SIGNAL(pressed()),this, SLOT(actionBut3()));
    connect( buton3, SIGNAL(clicked()),this, SLOT(arretAppui()));

    connect( buton4, SIGNAL(pressed()),this, SLOT(actionBut4()));
    connect( buton4, SIGNAL(clicked()),this, SLOT(arretAppui()));

    connect( buton5, SIGNAL(clicked()),this, SLOT(goAimbot()));

}




/* Mets en place les menus de notre jukebox*/
void Client_qt::setupMenus()
{
  
}




/* Mets en place notre Interface utilisateur avec tous les widgets*/
void Client_qt::setupUi()
{
    QWidget fenetre;
    QGridLayout *grille = new QGridLayout(this);
    QPushButton *buton = new QPushButton(QIcon ("croix.jpg"),"",this);
    grille->addWidget(buton,1,1);
    grille->setEnabled(true);
    //fenetre.setLayout(grille);
    //fenetre.show();
    setWindowTitle("Interface client");
}



/* Méthode permettant d'envoyer les commandes en fonction de l'id du bouton appuyé 
 * appelée périodiquement par notre timer
 * */

void Client_qt::update()
{
    cout<<"envoi commande "<<idBouton<<endl;
    char chaine[4]="";
    //strcpy(chaine,"h124");

    if(idBouton==1)
        strcpy(chaine,"h");
    if(idBouton==2)
        strcpy(chaine,"g");
    if(idBouton==3)
        strcpy(chaine,"b");
    if(idBouton==4)
        strcpy(chaine,"d");

    strcat(chaine,"10");    // rotation de 10° par envoi (ce qui est beaucoup

    if(send(fd_socket,chaine, 5, 0)==-1)
    {
        perror("pb write\n");
        //exit(ERR_WRITE);
    }
}

// une fois que le bouton est "clické", c'est à dire qu'on a enfin relâché l'appui sur le bouton, on arrête le timer,
// ce qui a pour effet l'arrêt de l'envoi de consigne
void Client_qt::arretAppui()
{
    timer->stop();
}



// fonction liée à l'appui sur le bouton 1
// le principe est le même pour chaque bouton, si je presse le bouton (un appui sans relacher),
// ça fixe la valeur de l'id du bouton appuyé, puis ça démarre un timer, qui a pour rôle d'appeler
// la méthode "update" périodiquement
void Client_qt::actionBut1()
{
    idBouton = 1;
    timer->start(); // réglage de la valeur en mili secc
}

void Client_qt::actionBut2()
{
    idBouton = 2;
    timer->start();
}

void Client_qt::actionBut3()
{
    idBouton = 3;
    timer->start();
}

void Client_qt::actionBut4()
{
    idBouton = 4;
    timer->start();
}


// appelé lors de l'activation du bouton centrale de l'interface graphique, 
// et a bien sûr pour but, de démarrer ou arrêter le mode correspondant
void Client_qt::goAimbot()
{


	// les lignes suivantes peuvent êtres décommebtées, elles ont uniquement pour but de changer
	// l'image affichée sur le bouton de l'aimbot
   /* static bool aimbotActif=0;
    //cout<<"go aimbot";
    if(aimbotActif==0)
    {
        aimbotActif=1;
        buton5->setIcon(QIcon("./randomIconEnable.ico"));
    }
    else
    {
        aimbotActif=0;
        buton5->setIcon(QIcon("./croix.jpg"));
    }*/

    char chaine[4]="";
    strcpy(chaine,"a000");
    if(send(fd_socket,chaine, 5, 0)==-1)
    {
        perror("pb write\n");
        //exit(ERR_WRITE);
    }

}





