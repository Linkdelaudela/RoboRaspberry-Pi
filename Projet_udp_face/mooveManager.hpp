#ifndef MOOVEMANAGER_HPP
#define MOOVEMANAGER_HPP
#include "wheels.hpp"

//ATTENTION: min+max/2 doit correspondre a la valeur d'arret du servomoteur
//dans notre cas arret = 1500
//il n'est pas grave de prendre des marges peut importante
//par exemple dans notre cas: min_reel = 992, max_reel = 2000, valeur d'arret= 1500
//de plus les deux servomoteurs ne sont pas necessairement calibre de la meme façon
//il semble donc judicieux de cree un MAX et un MIN par servo

#define MAX_SERV 2010
#define MIN_SERV 990

class CommandManager 
{
    private:
        Wheels *roues;          //contient les fonctions bas-niveaux pour acceder au servomoteurs
        int servoLeft;          //broche du servo de gauche
        int servoRight;         //broche du servo de droite
        int horiz;              //valeur de 0 a 100 indicant l'orientation (0 a gauche max, 50= tout droit, 100 a droite max)
        int run;                //valeur de 0 a 100 indicant la vitesse d'avancement (0 recule a fond, 50 stop, 100 avance a fond)
        int plageB,plageT;		//valeur indiquant l'ecart gauche/droite du moteur de base en degree et la valeur vertical/horizontal de la tete
        int nbPasB, nbPasT;		//valeur definissant le nombre de pas que chaque moteur est capable d'executer
		int valPasB,valPasT;	//valeur definissant la valeur en degree d'un pas pour chaque moteur
		int posMotB,posMotT;	//valeur indiquant la position de chaque moteurs pour eviter qu'ils ne fassent de tours complets
      

    public:
        CommandManager();
        ~CommandManager();
        
        //Fonctions servant au controle de chaque moteur dans le cas de la tourelle
        void rotB(int,int);
        void rotT(int,int);
                
        //met à jour la position des servomoteurs en fonction de horiz et run
        void majWheel();       

        //methodes servant aux mouvements dans le cas des roues du robot
        void left(int perc);
        void right(int perc);
        void forward(int perc);
        void backward(int perc);
        void stop();
        void reset();
    };

#endif // MOOVEMANAGER_HPP
