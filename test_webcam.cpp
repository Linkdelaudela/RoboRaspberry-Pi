#include <stdio.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
 
int main() {
 
	// Touche clavier
	char key;
	// Image
	 IplImage *image;
	// Capture vidéo
	CvCapture *capture;
 
	// Ouvrir le flux vidéo
	//capture = cvCreateFileCapture("/path/to/your/video/test.avi"); // chemin pour un fichier
	capture = cvCreateCameraCapture(0);
 
	// Vérifier si l'ouverture du flux est ok
	if (!capture) {
 
	   printf("Ouverture du flux vidéo impossible !\n");
	   return 1;
 
	}
 
	// Définition de la fenêtre
	cvNamedWindow("Projet SI", CV_WINDOW_AUTOSIZE);
 
	// Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
	while(key != 'q' && key != 'Q') {
 
	   // On récupère une image
	   puts("lecture");
	   image = cvQueryFrame(capture);
 
	   // On affiche l'image dans une fenêtre
	   cvShowImage( "Projet SI", image);
 
	   // On attend 10ms
	   key = cvWaitKey(10);
 
	}

	//on libere la fenetre graphique, et le peripherique de capture. 
	cvReleaseCapture(&capture);
	cvDestroyWindow("Projet SI");
 
	return 0;
 
}
