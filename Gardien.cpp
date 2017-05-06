//#s
#include <vector>
#include <utility>  
#include <cmath>
#include <random>
#include <climits>
//~ #include <stdio.h>
//#e
#include "Gardien.h"
#include "Chasseur.h"
//#s
#include "Labyrinthe.h" 

#define OCTANT_0 make_pair(1, make_pair(  0,  1))
#define OCTANT_1 make_pair(2, make_pair( -1,  1))
#define OCTANT_2 make_pair(3, make_pair( -1,  0))
#define OCTANT_3 make_pair(4, make_pair( -1, -1))
#define OCTANT_4 make_pair(5, make_pair(  0, -1))
#define OCTANT_5 make_pair(6, make_pair(  1, -1))
#define OCTANT_6 make_pair(7, make_pair(  1,  0))
#define OCTANT_7 make_pair(8, make_pair(  1,  1))

using namespace std;
int Gardien::_distance_max = 0;
float Gardien::_potentiel_max = 0;

//~ void inline check_out_of_bounds(void)
//~ {
	//~ if(next_x >= _l->width()){
			//~ printf("1 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
		//~ }
		//~ if(next_x < 0){
			//~ printf("2 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
		//~ }
		//~ if(next_y < 0){
			//~ printf("3 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
		//~ }
		//~ if(next_y >= _l->height()){
			//~ printf("4 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
		//~ }
//~ }
        
//#e
//Constructor
Gardien::Gardien(Labyrinthe* l, int x, int y, const char* modele) : Mover (x, y, l, modele)
{
	_pv = 2;
	_behavior = ATTAQUE;
	_guard_fire = new Sound ("sons/guard_fire.wav");
	_guard_hit = new Sound ("sons/guard_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
	fm.hit = false;
	fm.trigger = false;
}


// Le gardien pense ici
void Gardien::update(void) {
	
    if (this == _l->_guards[1]) {
        _potentiel_max = 0;        
        _distance_max = 0;        
        for (int i = 1; i < _l->_nguards; i++) {
            
            Gardien* gardien = ((Gardien*) ((Labyrinthe*) _l)->_guards[i]);
            coord posGardien;
            posGardien.i = (int)(gardien->_x / Environnement::scale);
            posGardien.j = (int)(gardien->_y / Environnement::scale);		
            printf("A\n");
            int distance = ((Labyrinthe*) _l)->getDistance(posGardien.i, posGardien.j);
            if (distance * gardien->_pv) {
				printf("Aprime %d\n", distance);
				_distance_max = max(_distance_max, distance);
			}       
        }
        
    }
    
	coord posGardien;
	posGardien.i = (int)(_x / Environnement::scale);
    posGardien.j = (int)(_y / Environnement::scale);
    printf("B\n");
    int distance = ((Labyrinthe*) _l)->getDistance(posGardien.i, posGardien.j);  
    printf("C\n");
    if (distance * _pv) {
		printf("D %d\n", _distance_max);
        _potentiel = distance / _distance_max;
        
        _potentiel_max += _potentiel;
		
		printf("%lf , %d , %lf\n", _potentiel_max, _distance_max, _potentiel);
		
		if (_potentiel_max < _potentiel + 1) {
			_behavior = ATTAQUE;
		} else {
			_behavior = DEFENSE;
		}
    }
 
/*

    if (this == this->_l->_guards[1]) {
        this->_l->_totalPotentiel = 0;
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
	
	
	if(this->_pv > 0){
		if(_behavior == ATTAQUE){
			if(fm.trigger == false)
			{
				fire(0);
				fm.trigger = true;
				fm.hit = false;
			}else{
				int delay = 1000 * (fd.stop - fd.start)/CLOCKS_PER_SEC;
				if(fm.hit){
					if(delay >= 500){
						fm.trigger = false;
					}else{				
						fd.stop = clock();									
					}
				}
			}			
			//#s		
		}
		move(0.5, 0.5);
	}else{
		rester_au_sol();
	}
    //#e
	message ("PV : %d", ((Chasseur *) ((Labyrinthe *)_l)->_guards[0])->_pv);
}

//#s
bool Gardien::move (double dx, double dy) { 
	//~ vector<pair<int, pair<int, int>>> octants;
	//~ octants.push_back(OCTANT_0);
	//~ octants.push_back(OCTANT_1);
	//~ octants.push_back(OCTANT_2);
	//~ octants.push_back(OCTANT_3);
	//~ octants.push_back(OCTANT_4);
	//~ octants.push_back(OCTANT_5);
	//~ octants.push_back(OCTANT_6);
	//~ octants.push_back(OCTANT_7);
	//~ int index = round(_angle / 45);
	
	int next_x = (int)((_x + dx * cos(_angle)) / Environnement::scale);
	int next_y = (int)((_y + dy * sin(_angle)) / Environnement::scale);
	//~ int next_step = _l -> data (next_x,	next_y);
	if(next_x >= _l->width()){
		printf("1 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
	}
	if(next_x < 0){
		printf("2 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
	}
	if(next_y < 0){
		printf("3 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
	}
	if(next_y >= _l->height()){
		printf("4 P(%d : %d) N(%d : %d)\n",(int)_x / Environnement::scale, (int)_y / Environnement::scale, (int)next_x / Environnement::scale, (int)next_y / Environnement::scale);
	}
		

	if (((Labyrinthe *) _l)->isAccessible(next_x, next_y)){
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = EMPTY;
		_x += dx * cos(_angle);
        _y += dy * sin(_angle);
		((Labyrinthe *)_l)->density[(int)(_x / Environnement::scale)][(int)(_y / Environnement::scale)] = GARDIEN;
		return true;		
	}

    // Set Angle Patrouille
    setAngle();
    
    // Set Angle Defense
	return false;
}
//#e

// ne sait pas tirer sur un ennemi.
void Gardien::fire (int angle_vertical) {
	
    _guard_fire -> play ();
	_fb -> init (_x, _y, 10.,                /* position initiale de la boule xyz */ 
				 angle_vertical, _angle);    /* angle de visée vertical - horizontal */ 
	fd.start = clock();
}


// quand a faire bouger la boule de feu...
bool Gardien::process_fireball (float dx, float dy)
{
	Chasseur * chasseur = ((Chasseur *) ((Labyrinthe *)_l)->_guards[0]);

	// calculer la distance du sons entre le chasseur et le lieu de l'explosion.
	float	x = (chasseur->_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (chasseur->_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	
	int next_x = (int)((_fb -> get_x () + dx) / Environnement::scale);
	int next_y = (int)((_fb -> get_y () + dy) / Environnement::scale);
	
	if(next_x < 0){
		next_x = 0;
	}else if(next_x >= _l->width()){
		next_x = _l->width()-1;
	}
	if(next_y < 0){
		next_y = 0;
	}else if(next_y >= _l->height()){
		next_y = _l->height()-1;
	}
	//~ printf("G\n");
	//~ printf("%d , %d \n",(int)((_fb -> get_x () + dx) / Environnement::scale), 
						//~ (int)((_fb -> get_y () + dy) / Environnement::scale));

	
	// on bouge que dans le vide !
	if (((Labyrinthe *)_l)->isAccessible(next_x,next_y))
	{
		return true;
	}

	
	//~ // Sinon collision...
	//~ // Si la prochaine case contient du chasseur	
	if((int)(chasseur->_x / Environnement::scale) == next_x 
		&& (int)(chasseur->_y / Environnement::scale) == next_y){
		
		if(chasseur->_pv > 0){
		   chasseur->_pv--;
			chasseur->_hunter_hit->play();
		}else{
			partie_terminee(false);
		}
	}
	
	// Attention : Clock Varie en fonction de la configuration du pc 
	// La temporisation peut donc varier
	fd.stop = clock();
	
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());

	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	
	// Indique que la fireball a touche quelque chose
	fm.hit = true;	
	return false;
}


void Gardien::seekHunter(){
	
	
}

bool Gardien::targetHunter(){
	return true;
}
//#s

void Gardien::setAngle() {

    //https://en.wikipedia.org/wiki/Octant_(solid_geometry)
    vector<pair<int, pair<int, int>>> octants;
    octants.push_back(OCTANT_0);
    octants.push_back(OCTANT_1);
    octants.push_back(OCTANT_2);
    octants.push_back(OCTANT_3);
    octants.push_back(OCTANT_4);
    octants.push_back(OCTANT_5);
    octants.push_back(OCTANT_6);
    octants.push_back(OCTANT_7);
        
    // Guardian position
    int x = (int) (_x / Environnement::scale);
    int y = (int) (_y / Environnement::scale);

    // Octant index
    int index = round(_angle / 45.f);  
    
    // 8 to 0
    if (index == 8) {
        index = 0;
    }

    // Remove octant inaccessible
    octants.erase(octants.begin() + index);
        
    // Minimum distance 
    int distance_min = INT_MAX;
    
    //
    index = 0;
    
    // Loop all octants
    for (int i = 0; i < octants.size(); i++) {               

        // Retrieve octant position
        int dx = octants[i].second.first;
        int dy = octants[i].second.second;
        
        // Retrieve octant distance
        int distance = ((Labyrinthe*) _l)->getDistance(x + dx, y + dy);
        
        // Check if octant is accessible
        if (distance == INT_MAX) {
          
            // Remove octant inaccessible
            octants.erase(octants.begin() + i);
            
        } else {
            
            // Check if distance is the minimum
            if (distance < distance_min) {
            
                // Update minimum distance
                distance_min = distance;
                
                // Update index octant
                index = i;
                
            }
            
        }
            
    }
  
    
    //http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    //
    random_device rd;
    //
    mt19937 rng(rd());
    
    // if (_behavior == DEFENSE) 
    // Do nothing because the minimum octant is already known
    
    //  
    if (_behavior == PATROUILLE) {
                       
        //
        uniform_int_distribution<> direction(0, octants.size() - 1);
        
        //
        index = direction(rng);
        
    }
    
    // Retrieve octant
    int octant = octants[index].first;

    //
    uniform_int_distribution<> angle(((octant - 1) * 45) - 22.5, (octant * 45) - 21.5);
            
    //
    _angle = angle(rng);
            
    //
    if (_angle < 0) {
        _angle += 360;
    }
    
}
//#e

