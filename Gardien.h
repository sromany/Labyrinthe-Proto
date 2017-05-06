#ifndef GARDIEN_H
#define GARDIEN_H

#include <stdio.h>
#include <time.h>
#include "Mover.h"
#include "Sound.h"

#define ATTAQUE 50
#define DEFENSE 25
#define PATROUILLE -25

// Déclencheurs pour l'action fire des gardiens 
typedef struct{
	bool hit, trigger;	
}FireMechanics;

// Intervale de temps entre les tirs des gardiens 
typedef struct{
	clock_t start, stop;
}Delay;

class Labyrinthe;

class Gardien : public Mover {
	//#s	
	private:
		void setAngle();
	//#e
	public:
		FireMechanics fm;
		Delay fd;
		/*
		 *	Le son...
		 */
		static Sound*	_guard_fire;	// bruit de l'arme du gardien.
		static Sound*	_guard_hit;	    // cri du gardien touché.
		static Sound*	_wall_hit;		// on a tapé un mur.
		int _pv;
    int _mode;

		//#s
                static int _distance_max;
                static float _potentiel_max;
                float _potentiel;
                bool _defense;
		//#e

		

	public:
		Gardien (Labyrinthe* l, int x, int y, const char* modele);

		// mon gardien pense très mal!
		void update (void);	

		void compute_potentiel();
		void seekHunter();
		bool targetHunter();

		// et ne bouge pas!
		bool move (double dx, double dy);
		// ne sait pas tirer sur un ennemi.
		void fire (int angle_vertical);
		// quand a faire bouger la boule de feu...
		bool process_fireball (float dx, float dy);
};

#endif

