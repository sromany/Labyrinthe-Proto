#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"
#include <cmath>
/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l, int x, int y) : Mover (x, y, l, 0)
{
	_pv = 3;
	first = true;
	pas_encore_lose = true;
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}
/*
 *	Tente un deplacement.
 */
bool Chasseur::move_aux (double dx, double dy)
{
	int next_x = (int)((_x + dx) / Environnement::scale);
	int next_y = (int)((_y + dy) / Environnement::scale);

	if(next_x == _l->_treasor._x && next_y == _l->_treasor._y ){
		if(pas_encore_lose){
			if(first){
				((Labyrinthe *)_l)->_win->play();
				first = false;
			}
		partie_terminee (true);	
		}	
		return false;
	}

	return ((Labyrinthe *)_l)->update(dx, dy, this);

}


/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	//~ message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, _angle);
}


/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */
bool Chasseur::process_fireball (float dx, float dy)
{
    // calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ());
	float	y = (_y - _fb -> get_y ());
	float	dist2 = x*x + y*y;
	float	dmax2 = (float)((_l -> width ())*(_l -> width ())
                       + (_l -> height ())*(_l -> height ())) * Environnement::scale;

	int next_x = (int)((_fb -> get_x () + dx) / Environnement::scale);
	int next_y = (int)((_fb -> get_y () + dy) / Environnement::scale);

	// on bouge que dans le vide!
	if (((Labyrinthe *)_l)->isFree(next_x,next_y))
	{
		//~ message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}

	// collision...
	// calculer la distance maximum en ligne droite.

    //#s
    // Update the labyrinth
    //((Labyrinthe*) _l)->update(next_x, next_y,);
    //#e

	for(int i = 1; i <_l->_nguards; i++){
            if((int)(_l->_guards[i]->_x / Environnement::scale) == next_x
            && (int)(_l->_guards[i]->_y / Environnement::scale) == next_y){
		((Gardien *) ((Labyrinthe *)_l)->_guards[i])->_pv--;
		((Gardien *) ((Labyrinthe *)_l)->_guards[i])->_guard_hit->play (exp(1. - dist2/dmax2));
		_l->_guards[i]->tomber();
                if(((Gardien *) ((Labyrinthe *)_l)->_guards[i])->_pv <= 0)
                    ((Labyrinthe *)_l)->free(_l->_guards[i]);
            }
	}




	// faire exploser la boule de feu avec un bruit fonction de la distance.
	// On peut éventuellement utiliser les coordonnées de la colisions pour jouer
	// le son en fonction des panoramiques droite gauche
	_wall_hit -> play (exp(.4 - dist2/dmax2));
	//~ message ("Booom...");
	return false;
}
