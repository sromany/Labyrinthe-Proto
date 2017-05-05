#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"
/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l, int x, int y) : Mover (x, y, l, 0)
{
	_pv = 3;
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
	int next_step = _l -> data (next_x,	next_y);
	
	if(next_x == _l->_treasor._x && next_y == _l->_treasor._y ){
		partie_terminee (true);
		return false;
	}			
		
	if (next_step == EMPTY || next_step == CHASSEUR){
		_x += dx;
		_y += dy;
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = EMPTY;
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = CHASSEUR;
		return true;
	}			
	return false;
}


/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	int next_x = (int)((_fb -> get_x () + dx) / Environnement::scale);
	int next_y = (int)((_fb -> get_y () + dy) / Environnement::scale);

	char value_of_next = _l->data (next_x, next_y);
	
	// on bouge que dans le vide!
	if (value_of_next == EMPTY)
	{
		//~ message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	

	for(int i = 1; i <_l->_nguards; i++){
		if((int)(_l->_guards[i]->_x / Environnement::scale) == next_x 
			&& (int)(_l->_guards[i]->_y / Environnement::scale) == next_y){
			if(((Gardien *) ((Labyrinthe *)_l)->_guards[i])->_pv > 0){
				((Gardien *) ((Labyrinthe *)_l)->_guards[i])->_pv--;
				_l->_guards[i]->tomber();
			}else{
				_l->_guards[i]->rester_au_sol();
			}
		}
	}

	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	//~ message ("Booom...");
	return false;
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

