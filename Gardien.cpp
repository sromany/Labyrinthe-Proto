//#s
#include <vector>
#include <utility>  
#include <cmath>
#include <random>
//#e
#include "Gardien.h"
#include "Chasseur.h"
//#s
#include "Labyrinthe.h" 
using namespace std;
//#e
//Constructor
Gardien::Gardien(Labyrinthe* l, int x, int y, const char* modele) : Mover (x, y, l, modele)
{
	_pv = 3;
	_guard_fire = new Sound ("sons/guard_fire.wav");
	_guard_hit = new Sound ("sons/guard_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
	fm.hit = false;
	fm.trigger = false;
}


// Le gardien pense ici
void Gardien::update(void) {
 /*

    if (this == this->_l->_guards[1]) {
        this->_l->_totalPotenciel = 0;
        this->_l->calcDistance();
    }

    potentiel();

    this->_l->_totalPotentiel += _potentiel;

    if (_potentiel < this->_l->_seuil) {
        // _status = defense
    }

    // tracking

    // look chasseur
*/	
	// Ici si le gardien nous voit : fire && wait !
	// if(seeHunter && targetLock) = seekHunter : faire deux fonctions : champ de vision et turn for fire
	_angle = 180;
	if(this->_pv >0){
		if(fm.trigger == false)
		{ 
			
			fire(0);		
			fm.trigger = true;
			fm.hit = false;
		}else{
			int delay = 1000 * (fd.stop - fd.start)/CLOCKS_PER_SEC;
			if(fm.hit){
				if(delay >= 250){
					fm.trigger = false;
				}else{				
					fd.stop = clock();									
				}
			}
		}
	}
	message ("PV : %d", ((Chasseur *) ((Labyrinthe *)_l)->_guards[0])->_pv);
}

/*
// update pontetiel
void Gardien::potentiel() {
    _potentiel = this->_l->distance(_x, _y) / this->_l->_totalDistance;
}
*/

// et ne bouge pas!
//#s
bool Gardien::move (double dx, double dy) { 

	int x = (int)((_x + dx) / Environnement::scale);
	int y = (int)((_y + dy) / Environnement::scale);

	//~ Labyrinthe* l = (Labyrinthe*) _l;

	if (((Labyrinthe *)_l)->isAccessible(x, y)) {
		
		_x += dx;
		_y += dy;
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = EMPTY;
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = GARDIEN;
		return true;		
	}
	return false;
}
//#e

// ne sait pas tirer sur un ennemi.
void Gardien::fire (int angle_vertical) {
    // _guard_fire -> play ();
	_fb -> init (_x, _y, 10.,          /* position initiale de la boule xyz */ 
				 angle_vertical, _angle); /* angles de visée vertical - horizontal */ 
	fd.start = clock();
}


// quand a faire bouger la boule de feu...
bool Gardien::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	
	int next_x = (int)((_fb -> get_x () + dx) / Environnement::scale);
	int next_y = (int)((_fb -> get_y () + dy) / Environnement::scale);
	char value_of_next = _l->data (next_x, next_y);
	
	// on bouge que dans le vide !
	if (value_of_next == EMPTY)
	{
		// il y a la place.
		return true;
	}
	
	// Sinon collision...
	// Si la prochaine case contient du chasseur

	if((int)(_l->_guards[0]->_x / Environnement::scale) == next_x 
		&& (int)(_l->_guards[0]->_y / Environnement::scale) == next_y){
		
		if(((Chasseur *) ((Labyrinthe *)_l)->_guards[0])->_pv > 0){
		   ((Chasseur *) ((Labyrinthe *)_l)->_guards[0])->_pv--;
			message("Ouch !");
		}else{
			partie_terminee(false);
		}
	}

	fd.stop = clock();
	
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	//~ _wall_hit -> play (1. - dist2/dmax2);
	fm.hit = true;
	return false;
}


void Gardien::seekHunter(){
	
	
}

bool Gardien::targetHunter(){
	return true;
}
//#s
void Gardien::setAngle(int x, int y) {

	//https://en.wikipedia.org/wiki/Octant_(solid_geometry)
	vector<pair<int, pair<int, int>>> octants;
	octants.push_back(make_pair(1, make_pair(  0,  1)));
	octants.push_back(make_pair(2, make_pair( -1,  1)));
	octants.push_back(make_pair(3, make_pair( -1,  0)));
	octants.push_back(make_pair(4, make_pair( -1, -1)));
	octants.push_back(make_pair(5, make_pair(  0, -1)));
	octants.push_back(make_pair(6, make_pair(  1, -1)));
	octants.push_back(make_pair(7, make_pair(  1,  0)));
	octants.push_back(make_pair(8, make_pair(  1,  1)));
	//http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
	//
	random_device rd; 
	//
	mt19937 rng(rd());
	//	
	while (!octants.empty()){

		//
		uniform_int_distribution<> cell(0, octants.size()-1);
		//
		int index = cell(rng);	
		//
		int octant = octants[index].first;
		//
		int i = octants[index].second.first;
		int j = octants[index].second.second;
		//
		if (((Labyrinthe *)_l)->isAccessible(x + i, y + j)) {
			//
			uniform_int_distribution<> angle(((octant - 1) * 45) - 22.5, (octant * 45) - 22.5);
			//
			_angle = angle(rng);
			//
			if(_angle < 0) {
				_angle += 360;		
			}
			//
			break;
		} else {
			//
			octants.erase(octants.begin() + index);
		}
	}

}
//#e
