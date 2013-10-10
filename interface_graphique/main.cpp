#include <QApplication>
#include <QTextCodec>
#include "client_qt.h"
#include <QPushButton>
#include <QGridLayout>
#include <QStyle>
#include <QAction>
#include <stdio.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

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

#include <pthread.h>

using namespace std;

enum { OK, ERR_BIND, ERR_LISTEN, ERR_SOCKET, ERR_ACCEPT, ERR_OUV_FICHIER, ERR_NOM_FICHIER, ERR_WRITE, ERR_CONNECT };

void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port)
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





void* my_thread_process (void * arg)
{
    // ON VA FAIRE L'AFFICHAGE DE vidéo reçue ici

    struct sockaddr_in adresseser, adressecli;
            struct sockaddr adress;
            int taille_image=921600, sock,pos;
            char *data, buf[20]="",buf2[20]="";
            socklen_t t_adresseser;
            IplImage *img;
            int mtu=1500, nb_paquets, reste, i, numIm=0;

            pthread_t th1;
        void *ret;

            //initialisationTCP();

            bzero(&adressecli,sizeof(adressecli));
            init_sockaddr(&adressecli,"127.0.0.1",1029);

            if( (sock=socket(AF_INET,SOCK_DGRAM,0)) == -1 )
            {
                 perror("Probleme socket() client");
                 exit(ERR_SOCKET);
            };

            strcpy(buf,"appli client ok");

            // on envoi une trame pour que le serveur nous connaisse
            if((sendto(sock, buf, 20, 0, (const sockaddr*) &adressecli,  sizeof(adressecli)))==-1);	// ne pas se préoccuper du FOUTU warning a la CON sur adressecli
            {
                                    //perror("pb send0\n");
                                    //cout<<"Probleme send"<<endl;
                                    //exit(ERR_WRITE);
            }

            cvNamedWindow("Projet SI/Client", CV_WINDOW_AUTOSIZE);
            img = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);


            data=(char*)malloc(mtu+20);
            img->imageData=(char*)malloc(921600);
            int oku=0;
            while(1)
            {

                            //cout<<"-----------reception-----------"<<endl;

                            recvfrom(sock,buf,20,MSG_WAITALL,&adress,(socklen_t *__restrict) sizeof(adressecli));


                            sscanf(buf,"%d",&taille_image);


                            if(taille_image==921600)
                            {
                            }
                            else
                                    continue;


                            // le send to et le receive to ne sont pas bloquants, c'est à dire qu'un sendto correspond à un seul envoi de paquets, il
                            // va donc falloir se baser sur le mtu du réseau: typiquement 1500 (le mtu correspond au nombre d'octets transmissibles d'un coup

                            // on va donc faire les calculs en fonction de la taille d'une image et du MTU, de meme cote serveur

                            nb_paquets=taille_image/mtu;
                            reste=taille_image%mtu;


                    strcpy(img->imageData,"\0");

                    // ici on recoit tous les paquets petit bout par petit bout :-)  (sauf le reste)
                    strcpy(buf,"\0");

                    for(i=0;i<nb_paquets;++i)
                    {


                                    // changement de programme, on envoi la position du paquet en meme temps que le paquet lui-meme
                                     if(!(recvfrom(sock,data,mtu+20,MSG_WAITALL,&adress,(socklen_t *__restrict) sizeof(adresseser) )))
                                    {
                                            perror("erreur recvfrom 2");
                                    }


                                    memcpy(buf2, data+mtu, 20);
                                    sscanf(buf2,"%d",&pos);


                                    if(pos<-1)
                                            continue;

                                    if(pos<=614 && pos>=0)
                                            memcpy(img->imageData+pos*mtu, data, mtu);

                                    pos=-1;

                    }


                    if(!(recvfrom(sock,data,reste+20, MSG_WAITALL,&adress,(socklen_t *__restrict) sizeof(adresseser) )))
                    {
                                            perror("erreur recvfrom 2");
                    }


                    memcpy(buf2, data+reste, 20);
                    sscanf(buf2,"%d",&pos);


                    if(pos<=614 && pos >= 0)	// avant on avait mis && <0 ...
                            memcpy(img->imageData+pos*mtu, data, reste);

                    //on teste la taille de la trame recu...


                            ++numIm;
                            if(numIm>=1)
                            {
                                    numIm=0;
                                    //detect_and_draw(img);

                            }

                            cvShowImage( "Projet SI/Client", img);



                            cvWaitKey(10);


            }

            free(data);
            cvDestroyWindow("Projet SI/Client");

}


// REGLER CE PROBLEME DE THREAD QUI SE LANCE PAS TOUT A FAIT COMME IL FAUT

int main(int argc, char *argv[])
{
    pthread_t th1;
    void *ret;


    if(pthread_create (&th1, NULL, my_thread_process, NULL) < 0) {
        fprintf (stderr, "pthread_create error for thread 1\n");
        exit (1);
    }


    usleep(200);    // le temps que le serveur soit prêt à accepter la connection entrante

    // 2 SOCKETS TCP, ON EST PAS SUR LE MEME RESEAU DANS LA REALITE


    // ON VA PLUTOT FAIRE L'AFFICHAGE GRAPHIQUE DANS LE MAIN
    QApplication app(argc, argv);

   QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
   app.setApplicationName("Pomme Player");

   QWidget fenetre;
   QGridLayout *grille = new QGridLayout;

   fenetre.setLayout(grille);
    fenetre.show();


    Client_qt player(grille);
   socklen_t socket_addr_len = sizeof(struct sockaddr_in);
    if ((connect(player.fd_socket,(struct sockaddr*)&player.socket_addr, socket_addr_len)) == -1)
    {
        perror ("connect");
        exit(errno);
    }

     return app.exec();


    struct sockaddr_in adressecli;	// type udp
    struct sockaddr adress;
    int sock;
    int mtu=1500, nb_paquets, reste, i, taille_image, pos=0;

    char *data, buf[20],buf2[20];


    puts("creation socket");

(void)pthread_join (th1, &ret);










	
	
	// REGARDER CA !!!! Pour utiliser les raccourcis claviers pour dirigier robot, pas assez approfondi
	
	/* pour l'instant j'ai un évènement : EVT_KEY_DOWN(myGLCanvas::onKeyDown)
Code :

    void myGLCanvas::onKeyDown(wxKeyEvent &event)
    {
    int key = event.GetKeyCode();
   
   if(key=='Q')
     CCamera::instance()->moveLeftRight(speed*60.0);
    if(key=='D')
     CCamera::instance()->moveLeftRight(-speed*60.0);
    if(key=='Z')
    CCamera::instance()->moveFrontBack(speed*60.0);
  if(key=='S')
    CCamera::instance()->moveFrontBack(-speed*60.0);
   }
  */

   // return app.exec();
}
