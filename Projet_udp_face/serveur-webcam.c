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

#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include <unistd.h>
#include <pthread.h>
#include <signal.h>

int sockClient, sockRobot, sockServeur, sockServeur2, opt;
int sock2;

int aimbot=1;
struct sockaddr_in adresseser2;
struct sockaddr adress2;

// prémuni des problèmes de fermeture brutale du programme
void gestionnaire (int numero)
{
	close(sockRobot);
	close(sockClient);
	close(sockServeur);
	close(sockServeur2);
	exit(0);
}

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

// fonction de capture (enregistre les "faces" dans une image)
void img2ppm(char *img, int num)
{
	FILE *fic;
	static chaine[20];
	
	sprintf(chaine,"image%d",num);
	fic=fopen(chaine,"w");
	fprintf(fic,"P6\n");
	fprintf(fic,"#bla bla pas drole\n");
	fprintf(fic,"%d %d\n",640,480);
	fprintf(fic,"255\n");
	
	fwrite(img,1,640*480*3,fic);
	//fflush(stdout);
}



struct sockaddr_in adresse_robot, adresse_client;
	int sockTCP, sockTCP2,opt;
	
	

struct sockaddr_in adresse_robot;
	

void initialisationTCP2(void)
{
	
	printf("euh, coucou?\n");
	init_sockaddr(&adresse_robot,"127.0.0.1",1552);
	init_sockaddr(&adresse_client,"127.0.0.1",4019);
	aimbot=1;
	
	// LE SERVEUR ACCEPT donc changer ça 
	
	if( (sockServeur=socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(1);
	};
	
	if( (sockServeur2=socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		perror("Probleme socket() client");
		exit(1);
	};
	
	if((bind(sockServeur,(struct sockaddr *)&adresse_robot,sizeof( adresse_robot)))==-1) 
	{
		perror("probleme bind()");
		exit(ERR_BIND);
	}
	
	if((bind(sockServeur2,(struct sockaddr *)&adresse_client,sizeof( adresse_client)))==-1) 
	{
		perror("probleme bind()");
		exit(ERR_BIND);
	}
	
	unsigned longueur = (socklen_t * )sizeof(adresse_robot);
	
	// d'abord on accepte la connexion du client graphique
	if(listen(sockServeur2,2)==-1)
	{
		perror("probleme listen()");
		exit(ERR_LISTEN);
	}
	
	if( (sockClient = accept(sockServeur2,(struct sockaddr *)&adresse_client,&longueur)) ==-1)
    {
			perror("probleme accept sock Client()");
			exit(ERR_ACCEPT);
	}
	
	// puis on accepte le robot
	if(listen(sockServeur,2)==-1)
	{
		perror("probleme listen()");
		exit(ERR_LISTEN);
	}	
	
	
	if( (sockRobot = accept(sockServeur,(struct sockaddr *)&adresse_robot,&longueur)) ==-1)
    {
			perror("probleme accept()");
			exit(ERR_ACCEPT);
	}
		
}

void *thread_moteurs (void * arg) {
        char buf[20];
        char chaine[4];
        int i=0;
			
			// ici nous allons récupérer l'adresse du client graphique
			
		int longueur = sizeof(adresseser2);
		printf("recv debut\n");
        recvfrom(sock2,buf,20,MSG_WAITALL, (const struct sockaddr *) &adress2 ,(socklen_t *) &longueur);
        printf("message: %s\n",buf);
        printf("recv fin\n");
        
		if((sendto(sock2, buf, 20, 0, (const struct sockaddr *) &adress2, sizeof(adress2)))==-1);	// ne pas se préoccuper du FOUTU warning a la CON sur adressecli
			{
				perror("pb send0 INIT\n\n\n");
				//exit(ERR_WRITE);
			}	
			
			    
        // GO INITIALISER LES SOCKETS, PUIS GO ENVOYYYYYYYER
        initialisationTCP2();
        
        // ici on va lire les commandes envoyées par notre aplication client, puis les rediriger sur le robot ;)
        //...
        int last;
        char cmd;
        while(1)
        {
			// on réceptionne les commandes envoyées par le client/utilisateur
			last =recv(sockClient, chaine, 5,0);

			if(last<=0)
				continue;

			// on va juste traiter si c'est le mode aimbot ou non
			memcpy(&cmd,chaine,1);
			
			if(cmd=='a')	// si la commande servait juste à activer le mode aimbot
			{
				if(aimbot==1)
					aimbot=0;
				else
					aimbot=1;
				continue;	// et on ne transmet pas la consigne au robot car elle était destinée uniquement au serveur
			}
			
			// puis on les transmet au robot
			if(send(sockRobot,chaine, 5, 0)==-1)
			{
				perror("pb write\n");
				exit(ERR_WRITE);
			}
		}
        
        pthread_exit (NULL);
        
}

// Create a string that contains the exact cascade name
const char* cascade_name =
    "./haarcascade_frontalface_alt.xml";

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img )
{
	static int init=0;
    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    static int scale = 1;

    // Create a new image based on the input image
    IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    static int i;

    // Load the HaarClassifierCascade
   if(aimbot==1)
    {
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
    
    // Allocate the memory storage
    storage = cvCreateMemStorage(0);


    // Clear the memory storage which was used before
    cvClearMemStorage( storage );
CvPoint  pt11, pt22;
    // Find whether the cascade is loaded, to find the faces. If yes, then:
    
    
    if( cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );
        // Loop the number of faces found.
        int grosseTete=0;
        
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;
            
            // pour le suivi de visage, on ne garde que la face la plus "grosse", 
            // c'est à dire dans la plupart des cas la plus proche
            // se référer au programme de test face tracking pour afficher plusieurs visages
            if( (pt2.x-pt1.x) < grosseTete)
				continue;
			else
				grosseTete = pt2.x-pt1.x;

		}
		
		//if(grosseTete==0)
		//	return;
			
            // Draw the rectangle in the input image
            int k=4;
            // ces points sont pour l'aimbot
            
            pt11.x = (pt2.x-pt1.x)/2 + pt1.x;
            pt22.x = pt11.x+k;
            pt11.y = (pt2.y-pt1.y)/4 + pt1.y ;
            pt22.y = pt11.y+k;
            
            
            

            // Draw the rectangle in the input image
            int distanceX=0;
            int distanceY=0;
            distanceX=pt11.x-640/2;	// distance par rapport au centre
            distanceY=pt11.y-480/2;	// distance par rapport au centre
            int okX=0;
            int okY=0;
            int coeffOk = 30;
            
            CvPoint zone1;
            zone1.x=(640/2)-coeffOk;
            zone1.y=(480/2)-coeffOk;
            CvPoint zone2;
            zone2.x=(640/2)+coeffOk;
            zone2.y=(480/2)+coeffOk;
            char direction=0;
            int nbDegres=0;
            char chaine[4]="";	// chaine qui va être envoyée
           
            
            if( (distanceX > -coeffOk) & (distanceX < coeffOk))	// 10 est le coefficient de marge ;)
            {
				okX=1;
			}
			else
			{
				if(distanceX<0)
				{
					chaine[0]='g';
					nbDegres=distanceX/10;	
					printf("GO GAUCHE!!\n");
				}
				else
				{
					chaine[0]='d';
					nbDegres=distanceX/10;	
					printf("GO DROITE !!! \n");
				}
			}
				
			if( (distanceY > -coeffOk) & (distanceY < coeffOk))	// 10 est le coefficient de marge ;)
            {
				okY=1;
			}
			else
			{
				if(distanceY>0)
				{
					chaine[0]='b';
					nbDegres=distanceY/10;	
					printf("faut descendre!!!\n");
				}
				else
				{
					chaine[0]='h';
					nbDegres=distanceX/10;	
					printf("faut monter !!! \n");
				}
			}
			nbDegres*=10;
			nbDegres-=15;
			if(nbDegres<0)
				nbDegres = -nbDegres;
				
			nbDegres/=10;
			
			if( (okX==1) && (okY==1))
				printf("PERFECT !!!\n");
			else 
			{	if(okX==1)
					printf("BON sur HORIZONTAL\n");
				else if(okY==1)
						printf("BON sur VERTICAL\n");
						
				sprintf(chaine+1,"%d",nbDegres);
				printf("chaine: %s\n",chaine);
			}
				
            
            // une fois qu'on a établi notre commande de correction de visée, il nous reste à l'envoyer
            if(nbDegres!=0)
            {
				printf("on va envoyer: %s\n", chaine);
				if(strlen(chaine)>5)
					return;
					
				// A REMETTRE QUAND CONNECTION AVEC ROBOT
				if(send(sockRobot,chaine, 5, 0)==-1)
				{
					perror("pb write\n");
					exit(ERR_WRITE);
				}
			}
            
            cvRectangle( img, pt11, pt22, CV_RGB(0,255,0), 3, 8, 0 );
            
            
            // quand on passe dans le cadre ça devient rouge :p
            if( (okX==1) && (okY==1))
				cvRectangle( img, zone1, zone2, CV_RGB(255,0,0), 3, 8, 0 );
			else
				cvRectangle( img, zone1, zone2, CV_RGB(0,0,255), 3, 8, 0 );
            
            //cvRectangle( img, pt1, pt2, CV_RGB(0,255,0), 3, 8, 0 );
            
            //for(k=0;k<640*3;++k)
				//img->imageData[(0*img->width*3)+k+1]=50;
				
				
				
            
            //printf("%u\n",img->imageData[(pt11.y*img->width*3)+pt11.x+0]);
            //unsigned char u=img->imageData[3];
            //sscanf(img->imageData[3],"%s",&u);
            
            
            // COOL !!!
            
            
           ++init;
	// fonction d'enregistrement de l'image ( photo face)
	//img2ppm(img->imageData,init);
				
          
        
        //int row=10, col=320, c=1;	// c=0 pour bleu, 1 pour vert et 2 pour rouge
		//img->imageData[row*640*3+col*3+c]=255;
        
        
		
		
		
		
		/* // ces points sont pour l'aimbot
            CvPoint  pt11, pt22;
            pt11.x = (pt2.x-pt1.x)/2 + pt1.x;
            pt22.x = pt11.x+k;
            pt11.y = (pt2.y-pt1.y)/4 + pt1.y ;
            pt22.y = pt11.y+k;

            // Draw the rectangle in the input image
            cvRectangle( img, pt11, pt22, CV_RGB(0,255,0), 3, 8, 0 );*/
		
    }
	}
	
	
	/* Il ne vaut mieux pas activer le mode aimbot et la détection d'objets en même temps, car le robot va devoir analyser plusieurs chose
	 en même temps */
	
    /*
    //  ICI nous allons faire un algorithme permettant de savoir si l'objet que l'on veut suivre est présent sur l'image
        int numFound=0;	// correspond au nombre de pixels dont la couleur correspond
        int xRech=10;   // correspond à notre zone de recherche
		int yRech=10;   //
		int posX=-1;	// correspond à notre premier pixel de la bonne couleur trouvé
		int posY=-1;
		
		//int i,j;
		int j;
		int nbTotalPoints=0;
		int larg=20,haut=20;	// dimension de la zone de recherche après avoir trouvé le premier point
		int tolerance=5;	// on peut aller à 5 en dessus ou en dessous de la valeur prévue
		unsigned char rouge, vert, bleu;
		
		// ICI A DECOMMENTER, on verra ça plus tard, démarrer en traçant un signe
		*/
		
		/*int nbPass=0;
		usleep(1000);
		for(i=0;i<img->height;++i)
		{
			for(j=0;j<img->width;++j)
			{
				bleu = (unsigned char)img->imageData[(i*img->width*3)+j*3+0];
				vert = (unsigned char)img->imageData[(i*img->width*3)+j*3+1];
				rouge = (unsigned char)img->imageData[(i*img->width*3)+j*3+2];
				++nbPass;
				
				
				// TOUTES les solutions suivantes (if...) sont des tests dans le but de touver la meilleure teinte de rouge
				
				//if(vert>200)
				//if((unsigned char)img->imageData[(i*img->width*3)+j*3+2]==255)	// ensuite mettre des teintes de couleur (avec masque)
				
				// ATTENTION on va vérifier la force sur chaque composante RVB (dans l'ordre: 0 pour bleu, 1 pour vert et 2 pour rouge)
					// la toute dernière condition sert à vérifier que le bleu et le vert aient la même force, sinon c'est plus du rouge
					
				//if( (rouge > 220) && (vert < 100) && (bleu < 100) && ( (vert > bleu - tolerance) && (vert < bleu + tolerance) )) 
				//if( (vert > 50) && (rouge < 50) && (bleu < 50) && ( (vert > bleu - tolerance) && (vert < bleu + tolerance) )) 
				//if( (bleu > 80) && (rouge < 50) && (vert < 50) && ( (vert > bleu - tolerance) && (vert < bleu + tolerance) ))
				if( (rouge-20 > bleu) && (rouge-20 > vert) && (rouge > 200))
				{
					img->imageData[(i*img->width*3)+j*3+0] = 20;
					img->imageData[(i*img->width*3)+j*3+1] = 20;
					img->imageData[(i*img->width*3)+j*3+2] = 20;
				
					if((j>posX+larg) && (i>posY+haut))	// si on a dépassé le cadre, alors on met une nouvelle zone de recherche
						nbTotalPoints = 0;
				
					if(nbTotalPoints==0)
					{
						// coordonnées du premier point
						posX=j;
						posY=i;
					}
					
					// si le point trouvé est bien dans notre zone de recherche
					if((j<=posX+larg) && (i<=posY+haut))
						++nbTotalPoints;
					
					
					
					// si on a trouvé le bon nombre de points et donc qu'on a une correspondance, alors on arrête car c'est bon
					if(nbTotalPoints>=larg*haut)
						break;
				}
			}
		}
		
		printf("NOMBRE PASSAGES: %d\n",nbPass);
		
		printf("nb == som: %d == %d\n",nbTotalPoints, larg*haut);
		
		static int distNeed = 100;	// ça pourrait être un define, puisqu'on va s'en servir pour comparer valeurs
		static int distX = 0;
		static int distY = 0;
		static char initSuivi = 0;
		static int nbImagesHid = 0;	// contient le nombre de fois où l'objet a "disparu"
		static CvPoint depart;
		static CvPoint arrivee;	// quand le parcourt ne sera pas encore fini, cette variable contiendra la position actuelle de l'objet
		
	
		
		// si on a trouvé un nombre minimum de points/pixels correspondants
		if(nbTotalPoints>=larg*haut)
		{
			if(initSuivi==0)
			{
				nbImagesHid = 0;
				initSuivi=1;
				// on enregistre les coordonnées du premier point
				depart.x=posX+1;
				depart.y=posY+1;
			}
			
			arrivee.x=posX+1;
			arrivee.y=posY+1;
			
			distX = arrivee.x - depart.x;
			distY = arrivee.y - depart.y;
			
			// c'est parti pour comparer les distance
			//if( (arrivee.x - depart.x <= -distNeed) || (arrivee.x - depart.x >= +distNeed) || (arrivee.y - depart.y <= -distNeed) || (arrivee.y - depart.y >= +distNeed) )
			if( ( distX <= -distNeed) || ( distX >= distNeed) || (distY <= -distNeed) || (distY >= distNeed) )
			{
				nbImagesHid = 0;
				initSuivi = 0;
				printf("GOOOOOOOOOOOOOD\n");
				static char coeffModer = 20;
				
				printf("point départ (%d,%d)\npoint arrivée (%d,%d)\n", depart.x, depart.y, arrivee.x, arrivee.y);
				
				// maintenant qu'un parcourt a été fait, nous devons trouver lequel.
				// vu que l'on a 8 ordres possibles, il faut commencer par savoir dans quel quadran on est (quart de dran) ^^
				
				// les quadrans sont organisés comme dans le cercle trigo
				printf("distY = %d\ndistX = %d\n", distY, distX);
				// quadran haut/droit
			*/	
				// après quelques problèmes de réglages, j'ai décidé de réduire à 4 ordres (dans un premier temps)
				/*
				if( (distY >= 0) && (distX >=0) )
				{
					// maintenant qu'on a trouvé le cadran, il reste à déterminer quel est l'ordre (haut, droite, diagonale)
					// on pour cela appliquer un coefficient
					if( (distY == -distNeed) && (distX<=coeffModer) )
					{
						printf("ORDRE HAUT");
					}
					else if( (distX == distNeed) && (distY<=coeffModer) )
					{
						printf("ORDRE DROITE");
					}
					
					else
					{
						printf("ORDRE DIAGONALE HAUT/DROITE");
					}
				}
				
				// quadran bas/droit
				if( (distY <= 0) && (distX >=0) )
				{
					printf("quadran bas/droit");
				}
				
				// quadran bas/gauche
				if( (distY <= 0) && (distX <=0) )
				{
					printf("quadran bas/gauche");
				}
				
				// quadran haut/gauche
				if( (distY >= 0) && (distX <=0) )
				{
					printf("quadran haut/gauche");
				}*/
				
				/*if(distY>=distNeed)
					printf("ORDRE BAS");
					
				if(distY<=-distNeed)
					printf("ORDRE HAUT");
				
				if(distX>=distNeed)
					printf("ORDRE GAUCHE");
					
				if(distX<=-distNeed)
					printf("ORDRE DROITE");
				
				printf("\n");
				//exit(0);
			}
			
			
			printf("correspondance trouvée\n");
			//exit(0);
		//}
		
		//if(posX!=-1)
		//{
			CvPoint  pt11, pt22;
            pt11.x = posX+1;
           // pt22.x = posX+100;
           pt22.x = posX+larg;
            pt11.y = posY+1;
            //pt22.y = posY+100;
            pt22.y = posY+haut;

            // Draw the rectangle in the input image
            cvRectangle( img, pt11, pt22, CV_RGB(0,0,255), 3, 8, 0 );
            usleep(10000);
		}
		else
			if(initSuivi==1)
				if(++nbImagesHid>=100)
				{
					nbImagesHid = 0;
					initSuivi = 0;
					printf("OBJET PERDUUUUU\n");
					//exit(0);
				}
	//printf("nombre de visages: %d\n",i);
    // Show the image in the window named "result"
    //cvShowImage( "result", img );
	
    // Release the temp image created.
    //cvReleaseImage( &temp );*/
}



// ICI fonction initialisation pour l'envoi de trame pour un client

void ouvrirUDPclient(void)
{
	
	
	struct sockaddr adress;
	socklen_t t_adresseser;
	
	bzero(&adresseser2,sizeof(adresseser2));
	init_sockaddr(&adresseser2,"127.0.0.1",1029);
	
	if ( (sock2=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) 
	{
		perror( "socket echec\n "); 
		exit(ERR_SOCKET);
	}
	
	
	if ( bind( sock2, ( struct sockaddr * ) &adresseser2, sizeof(adresseser2)) == -1 )
	{
		perror( "bind echec\n "); 
		exit(ERR_BIND);
	}
	
}

// FONCTION d'envoi d'une image (quand on l'a totalement reçue on appelle cette fonction, et même code que le client) 
// finalement on renvoie la trame comme et dès qu'on la reçoit
 
int main (void)
{
	struct sigaction action;
	
	ouvrirUDPclient();
	
	action.sa_handler=gestionnaire;
	sigemptyset(&action.sa_mask);
	action.sa_flags=0;
	
	if(sigaction(SIGINT,&action,NULL) != 0 )
	{
		perror("error SIGACTION");
		exit(1);
	}
	
	struct sockaddr_in adresseser;
	struct sockaddr adress;
	int taille_image=921600, sock,pos;
	char *data, buf[20]="",buf2[20]="";
	socklen_t t_adresseser;
	IplImage *img;
	int mtu=1500, nb_paquets, reste, i, numIm=0;
	
	pthread_t th1;
    void *ret;
	
	
	// on démarre le thread, qui est la 2eme moitié de notre programme
	if (pthread_create (&th1, NULL, thread_moteurs, "1") < 0) {
                fprintf (stderr, "pthread_create error for thread 1\n");
                exit (1);
        }
	
	//initialisationTCP();
	
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
			
			if(sendto(sock2, buf, 20, 0, &adress2, sizeof(adresseser2))==-1);	// ne pas se préoccuper du FOUTU warning a la CON sur adressecli
			{
				//perror("pb send0\n");
				//exit(ERR_WRITE);
			}	
			
			
			
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
			
				if(sendto(sock2, data, mtu+20, 0, &adress2,  sizeof(adresseser2))==-1)	// la meme
				{
					//perror("pb send12\n");
					//exit(ERR_WRITE);
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
		
		if(sendto(sock2, data, reste+20, 0, &adress2,  sizeof(adresseser2))==-1)	// la meme
		{
			//perror("pb send2\n");
			//exit(ERR_WRITE);
		}
		
		
		memcpy(buf2, data+reste, 20);
		sscanf(buf2,"%d",&pos);
		
		
		if(pos<=614 && pos >= 0)	// avant on avait mis && <0 ...
			memcpy(img->imageData+pos*mtu, data, reste);
			
		//on teste la taille de la trame recue... 
			
		
	   
			
			++numIm;
			if(numIm>=1)
			{
				numIm=0;
				detect_and_draw(img);	// appel de la fonction de traitement d'images
				
			}
            
            
			cvShowImage( "Projet SI/Serveur", img);
			
			
			
			cvWaitKey(10);
	
			
	}					
				
	free(data);
	cvDestroyWindow("Projet SI/Serveur");
	
 close(sock);
 return 0;
}
			
				
				
				
				
				
				
			
					
				
				
				
				
				
				
				
				
				
				
				
				
				 


