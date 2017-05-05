#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Chasseur : public Mover {
	private:
		// accepte ou non un deplacement.
		bool move_aux (double dx, double dy);
	public:
		/*
		 *	Le son...
		 */
		static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
		static Sound*	_hunter_hit;	// cri du chasseur touché.
		static Sound*	_wall_hit;		// on a tapé un mur.
		int _pv;
		
	public:
		Chasseur (Labyrinthe* l, int x, int y);

		// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
		bool move (double dx, double dy) {
			return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
		}

		// le chasseur ne pense pas!
		void update (void){}; 

		// fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
		bool process_fireball (float dx, float dy);

		// tire sur un ennemi.
		void fire (int angle_vertical);
};

#endif
