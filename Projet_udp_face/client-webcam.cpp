#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <unistd.h>
#include <pthread.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>


#include "protocol.h"
#include "wheels.hpp"
#include "mooveManager.hpp"

// on créé notre commandManager ici, de sorte qu'il puisse être utilisé par tout le monde
CommandManager c;

#define BUFFER_SIZE 256


int fd_socket;

// si nous quittons le programme brutalement (Ctrl+C), cette fonction est appelée et ferme les sockets et 
// réinitialise la position des moteurs
void gestionnaire (int numero)
{
	c.stop();
	c.reset();
	
	close(fd_socket);
	exit(0);
}


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

void *thread_moteurs (void * arg) {
	
	int port, last,degre;
	struct sockaddr_in socket_addr;
	socklen_t socket_addr_len;
	char hostname[]="127.0.0.1";
	char buffer[4] ;
	char deplacement;
	char cBuf[3]; 
	port= 1528;
	printf("ici");
	
	// ouverture socket
	if( (fd_socket=socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(1);
	};
	
	
	
	init_sockaddr(&socket_addr,"127.0.0.1",1552);
	socket_addr_len = sizeof(struct sockaddr_in);
	
	printf("*** Socket Initialized on port : %d of host : %s ***\n",port,hostname);
  
	printf ("*** Ready To Connect ***\n");
	if ((connect(fd_socket,(struct sockaddr*)&socket_addr, socket_addr_len)) == -1)
	{
		perror ("connect");
		exit(errno);
	}
	printf ("*** Ready To recv ***\n");
	
	// une fois nos connexions établies, on se prépare à recevoir et traiter les commandes
	while ((last =recv(fd_socket, &buffer, 5,0)) > 0)
	{
		// si la commande est éronnée, on la jette et on en attend une autre
		if(last<=0)
			continue;
		
		// on affiche les commandes sur le terminal pour contrôl
		printf("%s",buffer);

		// on récupère le caractère relatif à l'ordre reçu
		memcpy(&deplacement,buffer,1);
		
		
		printf("%c\n",deplacement);
		memcpy(cBuf,buffer+1,3);
		
		// on récupère les degrès à parcourir (sur 3 caractères)
		degre=atoi(cBuf);
		
		// test sur le déplacement demandé
		if( deplacement == 'g') 
		{
			c.rotB(degre,0);
			printf("go gauche de %d\n",degre);
		}
		if( deplacement == 'd')
		{
			c.rotB(degre,1);
			printf("go droite de %d\n",degre);
		}
		if( deplacement == 'h')
		{
			c.rotT(degre,0);
			printf("go haut de %d\n",degre);
		}
		if( deplacement == 'b')
		{
			c.rotT(degre,1);
			printf("go bas de %d\n",degre);
		}
		
	}
		// appel de la fonction de re-positionnement des moteurs
		c.reset();
        pthread_exit (NULL);
}

int main (void)
{
	struct sigaction action;
	
	action.sa_handler=gestionnaire;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	
	if(sigaction(SIGINT,&action,NULL) != 0 )
	{
		perror("error SIGACTION");
		exit(1);
	}
	
	puts("Début");	// affichage pour être sûr que ça a bien démarré
	
	pthread_t th1;
    void *ret;
	
	struct sockaddr_in adressecli;	// type udp
	int sock;
	int mtu=1500, nb_paquets, reste, i;
	
	char *data, buf[20];
	
	if (pthread_create (&th1, NULL, thread_moteurs, NULL) < 0) {
                fprintf (stderr, "pthread_create error for thread 1\n");
                exit (1);
        }
	
	
	init_sockaddr(&adressecli,"127.0.0.1",1028);
	
	IplImage *image;
	CvCapture *capture;
	capture = cvCreateCameraCapture(0);
	
	data=(char*) malloc(mtu+20);
	
	
	
	puts("creation socket");
	if( (sock=socket(AF_INET,SOCK_DGRAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(ERR_SOCKET);
	};
	puts("OK");
	

	
	while(1)
	{
	  image = cvQueryFrame(capture);
	 
	   
	   
	   sprintf(buf,"%d",image->imageSize);
	   
		
		if(sendto(sock, buf, 20, 0, (const sockaddr*) &adressecli,  sizeof(adressecli))==-1);	// ne pas se préoccuper du FOUTU warning a la CON sur adressecli
		{
			//perror("pb send0\n");
			//exit(ERR_WRITE);
		}		
		
		// le send to et le receive to ne sont pas bloquants, c'est à dire qu'un sendto correspond à un seul envoi de paquets, il
		// va donc falloir se baser sur le mtu du réseau: typiquement 1500 (le mtu correspond au nombre d'octets transmissibles d'un coup
		
		// on va donc faire les calculs en fonction de la taille d'une image et du MTU, de meme cote serveur
		
		nb_paquets=image->imageSize/mtu;
		reste=image->imageSize%mtu;
	
		// ici on envoi tous les paquets petit bout par petit bout :-)  (sauf le reste
		for(i=0;i<nb_paquets;++i)
		{
			strcpy(data,"");
			sprintf(buf,"%d",i);
			
			// d'abord envoyer la trame, puis l'image
			
			
			memcpy(data, image->imageData+i*mtu,mtu);
			memcpy(data+mtu, buf, 20);
		
			
			if(sendto(sock, data, mtu+20, 0, (const sockaddr*) &adressecli,  sizeof(adressecli))==-1)	// la meme
			{
				//perror("pb send12\n");
				//exit(ERR_WRITE);
			}
			else
			{
				//puts("envoyé !");
			}
		}
		
			
			sprintf(buf,"%d",i);
			
			
			memcpy(data, image->imageData+i*mtu,reste);
			memcpy(data+reste, buf, 20);
			
			
			
			
			if(sendto(sock, data, reste+20, 0, (const sockaddr*) &adressecli,  sizeof(adressecli))==-1)	// la meme
			{
				//perror("pb send2\n");
				//exit(ERR_WRITE);
			}
			else
			{
				//puts("envoyé !");
			}
			
		//cvWaitKey(10);
		
	}
	free(data);
	cvReleaseCapture(&capture);
	
	(void)pthread_join (th1, &ret);
	close(sock);

	
	
	return 0;
}
	
