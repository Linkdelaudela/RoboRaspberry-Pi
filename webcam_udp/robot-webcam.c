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

#define BUFFER_SIZE 256

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


int main (void)
{
	puts("Début");
	//struct sockaddr_in adressecli;	// type tcp
	
	//adress
	
	struct sockaddr_in adressecli;	// type udp
	int sock;
	int mtu=1500, nb_paquets, reste, i;
	
	char *data, buf[20];
	
	bzero(&adressecli,sizeof(adressecli));
	
	init_sockaddr(&adressecli,"127.0.0.1",1028);
	
	IplImage *image;
	CvCapture *capture;
	capture = cvCreateCameraCapture(1);
	
	puts("malloc :");
	data=malloc(mtu+20);
	puts("malloc OK");
	
	
	
	puts("creation socket");
	if( (sock=socket(AF_INET,SOCK_DGRAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(ERR_SOCKET);
	};
	puts("OK");
	

	//cvNamedWindow("Projet SI/Client", CV_WINDOW_AUTOSIZE);
	
	while(1)
	{
		 // On récupère une image
	  // puts("-----------lecture-------------");
	  image = cvQueryFrame(capture);
	 
	   
	   
	   sprintf(buf,"%d",image->imageSize);
	   //printf("%d\n",strlen(image->imageData));
	   
		
		if(sendto(sock, buf, 20, 0, &adressecli,  sizeof(adressecli))==-1);	// ne pas se préoccuper du FOUTU warning a la CON sur adressecli
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
		
			
			if(sendto(sock, data, mtu+20, 0, &adressecli,  sizeof(adressecli))==-1)	// la meme
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
			
			//printf("data: %d\n",strlen(data));
			//printf("i: %d\n",i);
			
			
			
			if(sendto(sock, data, reste+20, 0, &adressecli,  sizeof(adressecli))==-1)	// la meme
			{
				//perror("pb send2\n");
				//exit(ERR_WRITE);
			}
			else
			{
				//puts("envoyé !");
			}
			
			//cvShowImage( "Projet SI/Client", image);
		//cvWaitKey(10);
		
	}
	free(data);
	cvReleaseCapture(&capture);
	
	close(sock);

	
	
	return 0;
}
	
