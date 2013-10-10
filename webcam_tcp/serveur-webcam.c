#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

enum { VIDE, ATTENTE_NF, ATTENTE_DATA };

struct client {
	unsigned status;
	char nomfichier[32];
	int fd;
	int sock;
};

#define MAX_CLIENT 16

#define TAILLE_MAX 50

#define BUFFER_SIZE 4096

enum { OK, ERR_BIND, ERR_LISTEN, ERR_SOCKET, ERR_ACCEPT, ERR_OUV_FICHIER, ERR_NOM_FICHIER, ERR_WRITE };

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
	
	struct sockaddr_in adresseser;
	int taille_image=921600, sock,sockaccept, nbRecu;
	char *data, buf[20]="";
	socklen_t t_adresseser;
	IplImage *img;
	
	cvNamedWindow("Projet SI/Serveur", CV_WINDOW_AUTOSIZE);
		
		
	t_adresseser = sizeof(adresseser);
	
	/* Creation de la socket serveur */
	if ( (sock=socket(PF_INET, SOCK_STREAM, 0)) == -1 ) 
	{
		perror( "socket echec\n "); 
		exit(ERR_SOCKET);
	}
	init_sockaddr(&adresseser,"127.0.0.1",1028);
	
	/* On lie la socket a l'OS*/
	if ( bind( sock, ( struct sockaddr * ) &adresseser, sizeof(adresseser)) == -1 )
	{
		perror( "bind echec\n "); 
		exit(ERR_BIND);
	}
	puts("bind ok");

	puts("listen");
	/* Definition de la file d'attente */
	if ( listen(sock, 10) == -1 )
	{
		perror( "listen echec\n "); 
		exit(ERR_LISTEN);
	}
	
	puts("listen ok");
	
	if ( (sockaccept = accept(sock, ( struct sockaddr * ) &adresseser, &t_adresseser)) == -1 ) 
	{
			perror("Accept");							
			exit(ERR_ACCEPT);
	}
	
	cvNamedWindow("Projet SI/Serveur", CV_WINDOW_AUTOSIZE);
	img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
	
	
	data=malloc(taille_image);
	while(1)
	{
			puts("-----------reception-----------");
			puts("read taille");
			if((nbRecu=recv(sockaccept,buf,20,MSG_WAITALL ))==-1)	//On lit sur la socket du client les donnees
			{
				perror("erreur read");
			}
			puts("read OK");
			
			sscanf(buf,"%d\n",&taille_image);
			printf("%d\n", taille_image);
			usleep(100);
	
			puts("realloc");
			data=realloc((char *) data,taille_image);
			puts("OK");
			
			
			if(taille_image==921600)
			{
			}
			else
				continue;
				
			if(!recv(sockaccept,data,taille_image,MSG_WAITALL ))
			{
				perror("erreur read/recv");
			}
			
				
			if(nbRecu==0)
				continue;
			
			
			puts("strcpy");			
			strcpy(img->imageData, data);
			
			//on teste la taille de la trame recu... 
			if(strlen(data)!=921600)
				continue;
			
			puts("showimage");
			cvShowImage( "Projet SI/Serveur", img);
			
			cvWaitKey(1);
	
			
	}					
				
	free(data);
	cvDestroyWindow("Projet SI/Serveur");
	
 close(sock);
 return 0;
}
			
				
				
				
				
				
				
			
					
				
				
				
				
				
				
				
				
				
				
				
				
				 


