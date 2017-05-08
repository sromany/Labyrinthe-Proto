#include <climits>
#include <cmath>
#include <random>
#include <utility>
#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Gardien::_guard_fire;	// bruit de l'arme du gardien.
Sound*	Gardien::_guard_hit;	// cri du gardien touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.
Sound*	Gardien::_wall_hit;	    // un gardien a tapé un mur.
Sound*  Labyrinthe::_win;
Sound*  Labyrinthe::_lose;
Sound*  Labyrinthe::_ambiance;

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename){
	_win = new Sound ("sons/win.wav");
	_lose = new Sound ("sons/lose.wav");
	_ambiance = new Sound ("sons/ambiance.wav");
	printf("A\n");
	readFile(string(filename));

	printf("B\n");
	cout << "Nombre de murs: " << _nwall << endl;
	cout << "Nombre d'affiches: " << _npicts << endl;
	cout << "Nombre de boites: " << _nboxes << endl;
	cout << "Nombre de movers: " << _nguards << endl;

	printMat(ascii, _width, _height);

	printf("C\n");
	computeDensity(ascii);

	// On stock les murs
	_walls = new Wall[_nwall];

	// On stock les affiches.
	//string texfiles[_npict];
	_picts = new Wall[_npicts];

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
	computeDistances();
	//#e

	printf("%d  x %d\n", width(), height());
	_ambiance->play(.5);
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

		printf("END %d , %d\n", width, height);
		cout << "Taille du labyrinthe calculer dans le fichier " <<  width << " x " << height << endl;
		_width = width;
		_height = height;
		createMat(ascii, _width, _height);
		fillMat(ascii, c, _width, _height);
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
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
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

int Labyrinthe::countWalls(Mat<char> A){

	int k = 0;
	bool close = true;

	// Murs Horizontaux
	for(int i = 0; i < _height; i++){
		for(int j = 0; j < _width; j++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){

				}else if(close){
					k++;
					if(j < _width - 1){
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
 	for(int j = 0; j < _width; j++){
		for(int i = 0; i < _height; i++){
			if(A[i][j] == '+'){
				close = !close;
				if(!close){

				}else if(close){
					k++;
					if(i < _height - 1){
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
	_nwall = countWalls(A);

	for(int i = 0; i < _height; i++){
		for(int j = 0; j < _width; j++){
                        //#s
                        if(A[i][j] == 'x' || A[i][j] == '%'){
				k++;
			}
                        //#e
			if(A[i][j] == 'G' || A[i][j] == 'C'){
				g++;
			}
			if(A[i][j] >= 'a' && A[i][j] <= 'z'){
                            if(A[i][j] != 'x'){
                                m++;
                            }
			}
                        //#s
			if(A[i][j] == '%'){
                            //??
			}
                        //#e
		}
	}
	_nboxes = k;
	_nguards = g;
	_npicts = m;
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
	int k = 0, g = 0; /** * //#s , t = 0; * //#e */
	int h = 0, v = 0;
	bool close = true;
	cout << "Taille envoyer a sortWall " << _width << " x " << _height << endl;

	printf("Dprime1\n");
	// Murs Horizontaux
	for(int i = 0; i < _height; i++){
		for(int j = 0; j < _width; j++){
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
					if(j < _width - 1){
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
                        /**
                         * //#s
                        _teleporters._t[t] = &_picts[g];
						t++;
                         * //#e
                         */
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
 	for(int j = 0; j < _width; j++){
		for(int i = 0; i < _height; i++){
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
					if(i < _height - 1){
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
						/**
                                                 * //#s
                                                _teleporters._t[t] = &_picts[g];
						t++;
                                                 * //#e
                                                 */
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
	for(int i = 0; i < _height; i++){
		for(int j = 0; j < _width; j++){
			if((A[i][j] == 'x') || (A[i][j] == '%')) {
				_boxes[k]._x = i;
				_boxes[k]._y = j;
				k++;
			}
			if(A[i][j] == 'T'){
				_treasor._x = i;
				_treasor._y = j;
			}
                //#s
            if(A[i][j] == '%'){
			}
                //#e
		}
	}
}

void Labyrinthe::sortMovers(Mat<char> A){
	int k = 1;
	vector<string> guardianName({"drfreak","garde","Marvin","Potator"});
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> g(0, guardianName.size()-1);

	for(int i = 0; i < _height; i++){
		for(int j = 0; j < _width; j++){
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

//#s
// Public methods

//
bool Labyrinthe::isValid(int x, int y) {
    return (x >= 0 && x < _height) && (y >= 0 && y < _width);
}

//
bool Labyrinthe::isAccessible(int x, int y) {
    return isValid(x, y) && _density[x][y] != MUR && _density[x][y] != TRESOR && _density[x][y] != BOX;
}

//
bool Labyrinthe::isFree(int x, int y) {
    return isValid(x, y) && _density[x][y] == EMPTY;
}

//
bool Labyrinthe::isFree(double x, double y, Mover* mover) {

    int _x = (mover->_x + x) / Environnement::scale;
    int _y = (mover->_y + y) / Environnement::scale;

    return ((_x == ((int) mover->_x / Environnement::scale)) && (_y == ((int) mover->_y / Environnement::scale))) || isFree(_x, _y);

}

//
int Labyrinthe::getDistance(int x, int y) {
    if (isAccessible(x, y)){
        return _distances[x][y];
    }
    return INT_MAX;
}

//
bool Labyrinthe::update(int x, int y) {
    if (!isAccessible(x, y) && updateBox(x, y)) {
         reconfigure();
         return true;
    }
    return false;
}

//
bool Labyrinthe::update(double x, double y, Mover* mover) {

    if(isFree(x, y, mover)) {

        int i = mover->_x / Environnement::scale;
        int j = mover->_y / Environnement::scale;

        int code = _density[i][j];

        _density[i][j] = EMPTY;

        mover->_x += x;
        mover->_y += y;

        int k = mover->_x / Environnement::scale;
        int l = mover->_y / Environnement::scale;

        _density[k][l] = code;

        return true;

    }

    return false;

}

//
void Labyrinthe::free(int x, int y) {
    if (isValid(x, y)) {
        _density[x][y] = EMPTY;
    }
}

//
void Labyrinthe::free(Mover* mover) {

    _density[((int) mover->_x / Environnement::scale)][((int) mover->_y / Environnement::scale)] = EMPTY;

}

// Private methods

//
void Labyrinthe::computeDensity(vector<vector<char>> codes) {

    for(int i = 0; i < _height; i++) {
	for(int j = 0; j < _width; j++) {

            _density.push_back(vector<int>(_width));

            if (codes[i][j] == '+' || codes[i][j] == '-' || codes[i][j] == '|' || codes[i][j] == 'a' || codes[i][j] == 'b')
                _density[i][j] = MUR;
            else if(codes[i][j] == 'C')
		_density[i][j] = CHASSEUR;
            else if(codes[i][j] == 'G')
		_density[i][j] = GARDIEN;
            else if(codes[i][j] == 'T')
		_density[i][j] = TRESOR;
            else if(codes[i][j] == '%')
		_density[i][j] = PORTAL;
            else if(codes[i][j] == 'x')
		_density[i][j] = BOX;
            else
		_density[i][j] = EMPTY;

        }
    }

}

//
void Labyrinthe::computeDistances(){

    vector<vector<bool>> visited;

    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {

            visited.push_back(vector<bool>(_width));
            _distances.push_back(vector<int>(_width));

            visited[i][j] = !isAccessible(i,j);
            _distances[i][j] = INT_MAX;

        }
    }

    // Treasor distance
    _distances[_treasor._x][_treasor._y] = 0;

    // Unexplored cells
    vector<pair<int, int>>  unexplored;
    unexplored.push_back(make_pair(_treasor._x, _treasor._y));

    //
    while (!unexplored.empty()) {

        //
        pair<int, int> cell = unexplored.back();
        unexplored.pop_back();

        //
	for (int k = -1; k <= 1; k++) {
            //
            for (int l = -1; l <= 1; l++) {
                int i = cell.first + k;
                int j = cell.second + l;
                //
		if (isAccessible(i, j) && !visited[i][j]) {
                    unexplored.push_back(make_pair(i, j));
                    //
                    if (_distances[i][j] > (_distances[cell.first][cell.second] + 1)) {
			_distances[i][j] = _distances[cell.first][cell.second] + 1;
                    }
		}
            }
	}

        //
        visited[cell.first][cell.second] = true;

    }

}

//
bool Labyrinthe::updateBox(int x, int y) {
    if (_density[x][y] == BOX){
        for (int index = 0; index < _nboxes; index++) {
            // Remove box
            if((_boxes[index]._x == x) && (_boxes[index]._y == y)){
                _density[x][y] = EMPTY;
                updateDistance(x, y);
                memmove(_boxes + index, _boxes + index + 1, (_nboxes - index - 1) * sizeof(Box));
                _nboxes--;
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
            _distances[x][y] = min(_distances[x][y], getDistance(x + i, y + j));
	}
    }
    return _distances[x][y]++;
}
//#e
