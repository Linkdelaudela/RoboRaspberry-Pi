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
	struct sockaddr_in adressecli;
	int sock;
	char *data, buf[20];
		
	init_sockaddr(&adressecli,"127.0.0.1",1028);
	
	IplImage *image;
	CvCapture *capture;
	CV_FOURCC('M', 'J', 'P', 'G');
	capture = cvCreateCameraCapture(1);
	
	puts("malloc :");
	data=malloc(921600);
	puts("malloc OK");
	
	
	
	puts("creation socket");
	if( (sock=socket(PF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(ERR_SOCKET);
	};
	puts("OK");
	
	
	if(connect(sock,( struct sockaddr *) &adressecli,sizeof(adressecli)) ==0)
			printf( "connexion réussie \n");
	else 
	{
		printf("connexion echouee \n");
		exit(ERR_CONNECT);
	}
	
	
	
	
	while(1)
	{
		 // On récupère une image
	   puts("-----------lecture-------------");
	  image = cvQueryFrame(capture);
	 
	   
	   
	   sprintf(buf,"%d",image->imageSize);
	  
	   
	   puts("strcpy :");
	   strcpy(data, image->imageData);
	   puts("OK");
	   printf("%d\n", image->imageSize);
	   printf("%d\n", image->width);
	   printf("%d\n", image->height);


		
		puts("envoi taille image");
		if(send(sock, buf, 20, 0)==-1)
		{
			perror("pb write\n");
			exit(ERR_WRITE);
		}
		puts("OK");

		puts("envoi image");
		if(send(sock,data, image->imageSize,0)==-1)
		{
			perror("pb write\n");
			exit(ERR_WRITE);
		}
		else
		{
			puts("envoyé !");
		}
		
		//cvWaitKey(10);
		
	}
	free(data);
	cvReleaseCapture(&capture);
	
	close(sock);

	
	
	return 0;
}
	
