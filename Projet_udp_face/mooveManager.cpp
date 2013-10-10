#include "protocol.h"
#include "mooveManager.hpp"
#include <iostream>

#include <unistd.h>
#include <cmath>

using namespace std;

CommandManager::CommandManager()
{
	// Cette solution est entièrement dynamique, 2 paramètres majeurs: plage de rotation, et nombre de pas
    roues = new Wheels();
    servoLeft = 0;
    servoRight = 5;
    run = 50; //50% = immobile
    horiz = 50;
    plageB=180;		// plage de la base
    plageT=90;		// plage de la tête
    nbPasB=plageB;
    nbPasT=plageT;
    valPasB=plageB/nbPasB;		// La valeur d'un pas est définie par la plage, et le nombre de pas demandé
    valPasT=plageT/nbPasT;		//
    posMotB=plageB/2; // initialisation milieu
    posMotT=0;
    
}

CommandManager::~CommandManager()
{
    delete roues;
}

void CommandManager::rotB(int nbpas, int sens)
{
	
	if( sens == 1)
	{  
		run=0;
		if(nbPasB-posMotB < nbpas)
			nbpas = nbPasB-posMotB;
	}
	
    if(sens == 0)
    {
		run=100;
		if(posMotB < nbpas)
			nbpas = posMotB;
	}
	
	int sign;
    if((run-50)<0)
         sign = -1;
    else
        sign = 1;
        
    //RGpc : Rotation a gauche en pourcent
    int RGpc = ((2*(50-horiz)>=0 ?2*(50-horiz) :0) + abs((double)2*(run-50)))/2;
    
    // lancement des moteurs 
    roues->setTarget((MIN_SERV+MAX_SERV)/2 + sign*RGpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100, servoLeft);
    
//    usleep(5166*valPasB*nbpas);

	// Nous avons préférer cette solution à celle juste au dessus, qui consistant à faire la temporisation en une fois,
	// car nous avons envisagé l'arrêt brutal du programme, c'est à dire qui se le programme s'arrête pendant une rotation,
	// le robot va vouloir se remettre à sa position d'origine en fonction de la position qu'il pense être sa position actuelle,
	// car on lui aura dis "tu es à cette position", mais on aura pas fini la rotation pour atteindre cette position.
	
	// Nous avons donc maintenant une position au degré près, ce qui est nettement mieux. 
	for(int i=0;i<nbpas;++i){
		usleep(5166);	// temps pour effectuer un degré
		if(sens == 0)
			--posMotB;
		if(sens == 1)
			++posMotB;
	}
    stop();	// arrêt des moteurs
	
}

void CommandManager::rotT(int nbpas, int sens)
{
	
    if( sens == 0)
    {  
		run=0;
		cout<<"nbPasT" <<nbPasT<<" posMotT"<<posMotT<<" nbpas"<<nbpas<<endl;
		if(nbPasT-posMotT < nbpas)
			nbpas = nbPasT-posMotT;
			
		
	}
	
    if(sens == 1)  
    {
		run=100;
		if(posMotT < nbpas)
				nbpas = posMotT;
	}
	
	int sign;
    if((run-50)<0)
         sign = 1;
    else
        sign = -1;
        
    //RDpc : Rotation a gauche en pourcent
    int RDpc = ((2*(horiz-50)>=0 ?2*(horiz-50) :0) + abs((double)2*(run-50)))/2;
    roues->setTarget((MIN_SERV+MAX_SERV)/2 + (-1)*sign*RDpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100, servoRight);
    
    cout<<"nbPas "<<nbpas<<endl;
    for(int i=0;i<nbpas;++i){
		usleep(5166);
		if(sens == 1)
			--posMotT;
		if(sens == 0)
			++posMotT;
	}
    stop();
	
}

void CommandManager::majWheel()
{
    //RDpc : Rotation a droite en pourcent
    int RDpc = ((2*(horiz-50)>=0 ?2*(horiz-50) :0) + abs((double)2*(run-50)))/2;
    //RGpc : Rotation a gauche en pourcent
    int RGpc = ((2*(50-horiz)>=0 ?2*(50-horiz) :0) + abs((double)2*(run-50)))/2;
    //calcul du signe de (run-50)
    int sign;
    //CAN BE BETTER
    if((run-50)<0)
         sign = -1;
    else
        sign = 1;

    #ifdef DEBUG_WHEEL
        cout << "horiz: " << horiz << endl;
        cout << "run: " << run << endl;
        cout << "RDpc: " << RDpc << endl;
        cout << "RGpc: " << RGpc << endl;
        cout << "signe: " << sign << endl;
        cout << "stG: " << (MIN_SERV+MAX_SERV)/2 + sign*RGpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100 << endl;
        cout << "stD: " << (MIN_SERV+MAX_SERV)/2 + sign*RDpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100 << endl;
    #endif

    roues->setTarget((MIN_SERV+MAX_SERV)/2 + sign*RGpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100, servoLeft);
    //ATTENTION les roues étant positionnées en parallele sur le robot le sens avance pour l'un correspond
    //au recule pour l'autre, il faut donc multiplier le deplacement d'une des roues par -1
    roues->setTarget((MIN_SERV+MAX_SERV)/2 + (-1)*sign*RDpc*((MIN_SERV+MAX_SERV)/2-MIN_SERV)/100, servoRight);
}

void CommandManager:: left(int inc)
{
    if((horiz-=inc)<0)   horiz=0;
    majWheel();
}

void CommandManager::right(int inc)
{
    if((horiz+=inc)>100)   horiz=100;
    majWheel();
}

void CommandManager::forward(int inc)
{
    if((run+=inc)>100)   run=100;
    majWheel();
}

void CommandManager::backward(int inc)
{
    if((run-=inc)<0)   run=0;
    majWheel();
}

void CommandManager::stop()
{
    run = 50;
    horiz= 50;
    roues->setTarget(0, servoLeft);
    roues->setTarget(0, servoRight);

}

// fonction de réinitialisation de position de la tourelle
void CommandManager::reset()
{
	// pour la base, on doit le remettre au milieu
	if( posMotB < plageB/2)
		rotB(plageB/2-posMotB,1);
	if( posMotB > plageB/2)
		rotB(posMotB-plageB/2,0);
	
	// et pour la tete, on doit la remettre à la position horizontale
	// donc on redescend d'autant qu'on est monté
	rotT(posMotT,0);
	
	stop();
}

