// Sample Application to demonstrate how Face detection can be done as a part of your source code.

// Include header files
#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

// fonction de capture (enregistre les "faces" dans une image)
void img2ppm(char *img, int num, int x, int y)
{
        FILE *fic;
        static char chaine[20];

        sprintf(chaine,"image%d",num);
        fic=fopen(chaine,"w");
        fprintf(fic,"P6\n");
        fprintf(fic,"#bla bla pas drole\n");
        fprintf(fic,"%d %d\n",x,y);
        fprintf(fic,"255\n");

        fwrite(img,1,x*y*3,fic);
        //fflush(stdout);
}

// Create a string that contains the exact cascade name
const char* cascade_name =
    "./haarcascade_frontalface_alt.xml";
/*    "haarcascade_profileface.xml";*/


// Function prototype for detecting and drawing an object from an image
void detect_and_draw( IplImage* image );

// Main function, defines the entry point for the program.
int main( int argc, char** argv )
{

    // Create a sample image
    //IplImage *img = cvLoadImage("image.png",1);
    
    IplImage *img;
    //IplImage *image;
	CvCapture *capture;
	capture = cvCreateCameraCapture(0);
	
	cvNamedWindow("Projet SI", CV_WINDOW_AUTOSIZE);
	
	char key;
	
	int j=0;
	
	while(key != 'q' && key != 'Q') {
 
		++j;
	   // On récupère une image
	   //puts("lecture");
	   img = cvQueryFrame(capture);
 
		if(j>=0)
		{
			j=0;
			detect_and_draw(img);
		}
		
	   // On affiche l'image dans une fenêtre
	   cvShowImage( "Projet SI", img);
 
	   // On attend 10ms
	   key = cvWaitKey(10);
 
	}
	
	//img = cvQueryFrame(capture);
    
    // Call the function to detect and draw the face positions
    //detect_and_draw(img);

    // Wait for user input before quitting the program
    //cvWaitKey();

    // Release the image
    cvReleaseImage(&img);
    //sleep(2);
    // Destroy the window previously created with filename: "result"
    cvDestroyWindow("Projet SI");
    
    

    // return 0 to indicate successfull execution of the program
    return 0;
}

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img )
{

    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    static int scale = 1;

    // Create a new image based on the input image
    //IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );

    // Create two points to represent the face locations
    static CvPoint pt1, pt2;
    static int i;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
    
    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Create a new named window with title: result
    //cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        
        // c'est cette fonction qui est ultra longue, en même temps 26000 lignes à parcourir...
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );
		
        // Loop the number of faces found.
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            // Find the dimensions of the face,and scale it if necessary

            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;
            char *face=malloc((pt2.x-pt1.x)*(pt2.y-pt1.y));

            int k;  // nombre de ligne

            for(k=0;k<r->height;k++)
            {
                memcpy(face+k*(r->width),img->imageData+(r->y+k)*(640)+r->x,r->width);   // dernier paramètre: taille d'une ligne
                //printf("on passe: %d\n",k);
            }
			
            // Draw the rectangle in the input image
            cvRectangle( img, pt1, pt2, CV_RGB(0,0,255), 3, 8, 0 );
            img2ppm(face,0,r->width,r->height);
            printf("taille: %d\n largeur: %d hauteur: %d\n pos: %d\n",strlen(face),pt2.x-pt1.x,pt2.y-pt1.y,pt1.y);
            //exit(0);
          
        }
    }

	//printf("nombre de visages: %d\n",i);
    // Show the image in the window named "result"
    //cvShowImage( "result", img );
	
    // Release the temp image created.
    //cvReleaseImage( &temp );
}
