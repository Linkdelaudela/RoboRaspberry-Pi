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
	struct sockaddr adress;
	int taille_image=921600, sock,pos;
	char *data, buf[20]="",buf2[20]="";
	socklen_t t_adresseser;
	IplImage *img;
	int mtu=1500, nb_paquets, reste, i;
	
	cvNamedWindow("Projet SI/Serveur", CV_WINDOW_AUTOSIZE);
		
		
	t_adresseser = sizeof(adresseser);
	
	/* Creation de la socket serveur */
	if ( (sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
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
	
	cvNamedWindow("Projet SI/Serveur", CV_WINDOW_AUTOSIZE);
	img = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
	
	
	data=malloc(mtu+20);
	img->imageData=malloc(921600);
	while(1)
	{
			//puts("-----------reception-----------");
			
			recvfrom(sock,buf,20,MSG_WAITALL,&adress,(socklen_t *__restrict) sizeof(adresseser));
			
			//puts("read OK");
			
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
			
			//printf("%d\n",nb_paquets);
		
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
		
		//printf("taille1 : %d\n",strlen(img->imageData));

		if(!(recvfrom(sock,data,reste+20, MSG_WAITALL,&adress,(socklen_t *__restrict) sizeof(adresseser) )))
		{
					perror("erreur recvfrom 2");
		}
		
		//printf("taille2 : %d\n",strlen(img->imageData));
		
		memcpy(buf2, data+reste, 20);
		sscanf(buf2,"%d",&pos);
		//printf("pos: %d\n", pos);
		
		//printf("data: %d\n",strlen(data));
		//printf("i: %d\n",i);
		
		
		if(pos<=614 && pos >= 0)	// avant on avait mis && <0 ...
			memcpy(img->imageData+pos*mtu, data, reste);
			
		//on teste la taille de la trame recu... 
			
		
	   
			//puts("showimage");
			//printf(" MTN %d\n",strlen(img->imageData));
			//exit (0);
			cvShowImage( "Projet SI/Serveur", img);
			
			cvWaitKey(10);
	
			
	}					
				
	free(data);
	cvDestroyWindow("Projet SI/Serveur");
	
 close(sock);
 return 0;
}
			
				
				
				
				
				
				
			
					
				
				
				
				
				
				
				
				
				
				
				
				
				 


