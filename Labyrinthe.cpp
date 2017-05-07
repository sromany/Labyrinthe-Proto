#define VECTOR_VERSION
#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include <random>
#include <climits>
#include <cmath>

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Gardien::_guard_fire;	// bruit de l'arme du gardien.
Sound*	Gardien::_guard_hit;	// cri du gardien touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.
Sound*	Gardien::_wall_hit;	    // un gardien a tapé un mur.

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename){
	printf("A\n");
	readFile(string(filename));
	
	printf("B\n");
	cout << "Nombre de murs: " << _nwall << endl;
	cout << "Nombre d'affiches: " << _npicts << endl;
	cout << "Nombre de boites: " << _nboxes << endl;
	cout << "Nombre de movers: " << _nguards << endl;
	printMat(ascii, lab_width, lab_height);
	
	printf("C\n");
	makeDensity(ascii, density);

	// On stock les murs
	_walls = new Wall[_nwall];

	// On stock les affiches.
	//string texfiles[_npict];
	_picts = new Wall[_npicts];	
	_teleporters._t = new Wall* [_nteleporter];

	printf("D\n");
	sortWallsAndPicts(ascii);
	// On stock les boxes.
	printf("Dprime %d\n", _nboxes);
	_boxes = new Box[_nboxes];

    // On tous les elements
    printf("E\n");
	sortElements(ascii);

	// On stock les gardiens et le chasseur
	_guards = new Mover* [_nguards];

	// On stock chasseur et les gardiens.
	printf("F\n");
	sortMovers(ascii);

	//#s
	printf("G\n");
	makePCC(density, distance);
	//#e

	printf("%d  x %d\n", width(), height());
}

void Labyrinthe::readFile(string fname){
	ifstream in(fname);
	string line;
	bool first = true;
	int lab_start = 0;
	int width =  0;
	int height = 0;
	char c = ' ';
	
	regex comment("^#");
	regex rtex("[[:alpha:]]+[[:digit:]]*[[:alpha:]]*\\.jpg");
	regex letter_Not_x("^(\\s*)([a-wy-z]|@)");
	regex spaceOrWall("^(\\s*)((\\+)+|(\\|))");
	smatch tex_fname;
	smatch tex_letter;
	printf("START\n");
	if(in){
		while(getline(in,line)){
			if (regex_search(line, comment)) {
				// Ici on parse les lignes commençant par un commentaire en les ignorants
				printf("COMMENT\n");
			}
			else if(regex_search(line, tex_letter, letter_Not_x) && regex_search(line, tex_fname, rtex)){
				printf("AFFICHE\n");
				// C'est ici qu'on enregistre les textures
				cout << "Affiche: ["<< tex_letter[0].str()[0] << "]" << "\nFichier: ["<< tex_fname[0].str() << "]" << endl;
				texTab.push_back(tex_letter[0].str().c_str()[0]);
				texFile.push_back(tex_fname[0].str());
			}
			else if(regex_search(line, spaceOrWall) || regex_search(line, letter_Not_x)){
				printf("COUNT SIZE\n");
                // Ici on commence à compter la taille du labyrinthe
				width = std::max(width, int(line.length()));
				height++;
				// Et on retient la position ou le labyrinthe débute dans le fichier
				if(first){
					lab_start = int(in.tellg()) - int(line.length()) - 1;
					cout << "offset: " << lab_start << endl;
					first = false;
				}
			}
		}
		
		printf("END %d , %d\n",width, height);
		cout << "Taille du labyrinthe calculer dans le fichier " <<  width << " x " << height << endl;
		lab_width = width;
		lab_height = height;
		createMat(ascii, lab_width, lab_height);
		fillMat(ascii, c, lab_width, lab_height);
		in.ignore();
		in.clear();
		in.seekg(lab_start);
		int i = 0;
		printf("START AGAIN\n");
		while(getline(in,line)){
			printf("EACH LINE\n");
			for(int j = 0; j < int(line.length()); j++){
				ascii[i][j] = line[j];
				printf("%d , %d \n", i, j);
			}
			i++;
		}

	}else{
		cout << "ERREUR: Impossible d'ouvrir le fichier en lecture." << endl;
	}
	printf("END AGAIN\n");
	countAllData(ascii);
	printf("COUNT DATA\n");
}

void Labyrinthe::printInFileMat(const Mat<int>& A, const char* fname){
	ofstream out(fname);
	if(out)  //On teste si tout est OK
	{
		for (int i = 0; i < lab_height; i++) {
			for (int j = 0; j < lab_width; j++) {
				out << setfill('0') << setw(1) << A[i][j] << " ";
			}
			out << endl;
		}
	}
	else
	{
		cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	}
}

void Labyrinthe::makeDensity(Mat<char> A, Mat<int>& B){

	createMat(B, lab_width, lab_height);
	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if (A[i][j] == '+'
			|| A[i][j] == '-'
			|| A[i][j] == '|'
			|| A[i][j] == 'a'
			|| A[i][j] == 'b') {  // regex match cij ([a-w-yz] ou + ou - ou | ou x ou G ou T
				B[i][j] = MUR;
			}else if(A[i][j] == 'C'){
				B[i][j] = CHASSEUR;
			}else if(A[i][j] == 'G'){
				B[i][j] = GARDIEN;
			}else if(A[i][j] == 'T'){
				B[i][j] = TRESOR;
			}else if(A[i][j] == '@'){
				B[i][j] = PORTAL;
			}else if(A[i][j] == 'x'){
				B[i][j] = BOX;
			}else{
				B[i][j] = EMPTY;
			}
		}
	}
	printInFileMat(B, "density");
}

int Labyrinthe::countWalls(Mat<char> A){

	int k = 0;
	bool close = true;
	
	// Murs Horizontaux
	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){

				}else if(close){
					k++;
					if(j < lab_width - 1){
						if(A[i][j + 1] == '-' || A[i][j + 1] == '+' || A[i][j + 1] == '@' 
						|| ((A[i][j + 1] >= 'a' && A[i][j + 1] <= 'z') && A[i][j + 1] != 'x')){
							close = !close;
						}
					}
				}
			}
		}
	}

	// Murs Verticaux
 	for(int j = 0; j < lab_width; j++){
		for(int i = 0; i < lab_height; i++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){
					
				}else if(close){
					k++;
					if(i < lab_height - 1){
						if(A[i + 1][j] == '|' || A[i + 1][j] == '+' || A[i + 1][j] == '@' 
						|| ((A[i + 1][j] >= 'a' && A[i + 1][j] <= 'z') && A[i + 1][j]!= 'x')){
							close = !close;
						}
					}
				}
			}
		}
	}
	return k;
}

void Labyrinthe::countAllData(Mat<char> A){
	int k = 0, g = 0, m = 0;
	int t = 0;
	_nwall = countWalls(A);

	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if(A[i][j] == 'x'){
				k++;
			}
			if(A[i][j] == 'G' || A[i][j] == 'C'){
				g++;
			}
			if(A[i][j] >= 'a' && A[i][j] <= 'z'){
                if(A[i][j] != 'x'){
                    m++;
                }
			}
			if(A[i][j] == '@'){
				t++;
			}
		}
	}
	_nboxes = k;
	_nguards = g;
	_npicts = m;
	_nteleporter = t;
}

void Labyrinthe::findTex(char tmp[], const char c){
	int i = 0;
	while(i < int(texTab.size())){
		if(c == texTab[i])
			break;
		i++;
	}
	if(!texFile.empty()){
        sprintf (tmp, "%s/%s", texture_dir, texFile[i].c_str());
	}else{
        sprintf (tmp, "%s/%s", texture_dir, "brickwall.jpg");
	}
}

void Labyrinthe::sortWallsAndPicts(Mat<char> A){
	int k = 0, g = 0, t = 0;
	int h = 0, v = 0;
	bool close = true;
	cout << "Taille envoyer a sortWall " << lab_width << " x " << lab_height << endl;

	printf("Dprime1\n");
	// Murs Horizontaux
	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){
					_walls[k]._x1 = i;
					_walls[k]._y1 = j;

				}else if(close){
					_walls[k]._x2 = i;
					_walls[k]._y2 = j;
					_walls[k]._ntex = 0;
					h++;
					k++;
					if(j < lab_width - 1){
						if(A[i][j + 1] == '-' || A[i][j + 1] == '+'){
							_walls[k]._x1 = i;
							_walls[k]._y1 = j;
							close = !close;
						}
					}
				}
			}
			if(!close){
                if(((A[i][j] >= 'a' && A[i][j] <= 'z') && A[i][j]!= 'x')|| A[i][j] == '@'){
                    _picts[g]._x1 = i;
                    _picts[g]._y1 = j-1;
                    _picts[g]._x2 = i;
                    _picts[g]._y2 = j+1;

					char tmp[128];
					if(A[i][j] == '@'){
						sprintf (tmp, "%s/%s", texture_dir, "portal.jpg");
						_picts[g]._ntex = wall_texture (tmp);
						_teleporters._t[t] = &_picts[g];						
						t++;
					}else{
						findTex(tmp, A[i][j]);
						_picts[g]._ntex = wall_texture (tmp);              
					}
					g++;
				}
			}
		}
	}
	printf("Dprime2\n");
	// Murs Verticaux
 	for(int j = 0; j < lab_width; j++){
		for(int i = 0; i < lab_height; i++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){
					_walls[k]._x1 = i;
					_walls[k]._y1 = j;
				}
				if(close){
					_walls[k]._x2 = i;
					_walls[k]._y2 = j;
					_walls[k]._ntex = 0;
					v++;
					k++;
					if(i < lab_height - 1){
						if(A[i+1][j] == '|' || A[i+1][j] == '+'){
							_walls[k]._x1 = i;
                            _walls[k]._y1 = j;
							close = !close;
						}
					}
				}
			}
			if(!close){
                if((A[i][j] >= 'a' && A[i][j] <= 'z') && A[i][j]!= 'x'){
                    _picts[g]._x1 = i-1;
                    _picts[g]._y1 = j;
                    _picts[g]._x2 = i+1;
                    _picts[g]._y2 = j;
                     
                    char tmp[128];                 
					if(A[i][j] == '@'){
						sprintf (tmp, "%s/%s", texture_dir, "portal.jpg");
						_picts[g]._ntex = wall_texture (tmp);
						_teleporters._t[t] = &_picts[g];						
						t++;
					}else{						
						findTex(tmp, A[i][j]);
						_picts[g]._ntex = wall_texture (tmp);                    
					}
					g++;
				}
            }
		}
	}
	printf("Dprime3\n");
	cout <<"Nb de mur H:" << h << endl;
	cout <<"Nb de mur V:" << v << endl;
}

void Labyrinthe::sortElements(Mat<char> A){

	int k = 0;
	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if(A[i][j] == 'x'){
				_boxes[k]._x = i;
				_boxes[k]._y = j;
				k++;
			}
			if(A[i][j] == 'T'){
				_treasor._x = i;
				_treasor._y = j;
			}
		}
	}
}

void Labyrinthe::sortMovers(Mat<char> A){
	int k = 1;
	vector<string> guardianName({"drfreak","garde","Marvin","Potator"});
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> g(0, guardianName.size()-1);

	for(int i = 0; i < lab_height; i++){
		for(int j = 0; j < lab_width; j++){
			if(A[i][j] == 'C'){
				_guards[0] = new Chasseur(this, i*Environnement::scale, j*Environnement::scale);
			}
			if(A[i][j] == 'G'){
				_guards[k] = new Gardien (this, i*Environnement::scale, j*Environnement::scale, guardianName[g(gen)].c_str()/* Random value i in Guardian Name*/);
				k++;
			}
		}
	}
}
/**
 ** Ajouter Dijkstra ici !!!
 **/
// Mercredi
// jeudi 11 et vendredi 12 Soutenance

//#s
void Labyrinthe::makePCC(Mat<int> A, Mat<int>& B){
	
	//
	createMat(B, lab_width, lab_height);

    // Position tresor
    coord p;
    p.i = _treasor._x;
	p.j = _treasor._y;
	//
	int i_x = p.i;
    	int j_y = p.j;
	
    	Mat<bool> visited;
    	createMat(visited, lab_width, lab_height);
	//
	for (int i = 0; i < lab_height; i++) {
		for (int j = 0; j < lab_width; j++) {
			visited[i][j] = !isAccessible(i,j);
	            	distance[i][j] = INT_MAX;
		}
	}
	//
	distance[i_x][j_y] = 0;
	//
	vector<coord>  unexplored;
	unexplored.push_back(p);
	//
    	while (!unexplored.empty()) {
        	//
        	p = unexplored.back();
		//
        	i_x = p.i;
	        j_y = p.j;
		//
	        unexplored.pop_back();
		//
		for (int k = -1; k <= 1; k++) {
			for (int l = -1; l <= 1; l++) {
				if (isAccessible(i_x + k, j_y + l) && !visited[i_x + k][j_y + l]) {
					p.i = i_x + k;
					p.j = j_y + l;
					unexplored.push_back(p);
				}
			}
		}
		//
        	visited[i_x][j_y] = true;
		//
		for (int k = -1; k <= 1; k++) {
			//
			for (int l = -1; l <= 1; l++) {
				if (isAccessible(i_x + k, j_y + l) && (distance[i_x + k][j_y + l] > (distance[i_x][j_y] + 1))) {
					distance[i_x + k][j_y + l] = distance[i_x][j_y] + 1;
				}
			}		
		}

    }
	printInFileMat(B, "distance");
}

//
bool Labyrinthe::isAccessible(int x, int y) {
	return (x >= 0 && x < lab_height) && (y >= 0 && y < lab_width) && (density[x][y] != MUR) && (density[x][y] != TRESOR) && (density[x][y] != BOX ) && (density[x][y] != CHASSEUR);
}

//
int Labyrinthe::getDistance(int x, int y) {
	if (isAccessible(x, y)){
            return distance[x][y];
        }
	return INT_MAX;
}

//
bool Labyrinthe::removeBox(int x, int y) {
	// Vérification : si c'est bien une caisse et qu'on se trouve dans le Lab..
	if ((x >= 0 && x < lab_height) && (y >= 0 && y < lab_width) && (density[x][y] == BOX)){

		for (int index = 0; index < _nboxes; index++){
			
			if((_boxes[index]._x == x) && (_boxes[index]._y == y)){
				density[x][y] = EMPTY;
                                updateDistance(x, y);
				memmove(_boxes + index, _boxes + index + 1, (_nboxes - index - 1) * sizeof(Box));
				_nboxes--; 
				reconfigure();
				return true;
		   }
		}
	}
	return false;
}

//
int Labyrinthe::updateDistance(int x, int y) {
    for (int i = -1; i <= 1; i++) {
	for (int j = -1; j <= 1; j++) {
            distance[x][y] = min(distance[x][y], getDistance(x + i, y + j));
	}
    }
    return distance[x][y]++;    
}//#e
