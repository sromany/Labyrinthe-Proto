#ifndef LABYRINTHE_H
	#define LABYRINTHE_H
	#include "Environnement.h"

	#include <stdlib.h>
	#include <stdio.h>
	#include <iostream>
	#include <fstream>
	#include <vector>
        #include <map>
	#include <iomanip>
	#include <string>
	#include <algorithm>
	#include <regex>

	#define MUR 1
	#define CHASSEUR 2
	#define GARDIEN 3
	#define TRESOR 7
	#define PORTAL 4
	#define BOX 5	

	using namespace std;

	typedef struct coord {
		int i, j;
		int x, y;
	}coord;

	/**
	 ** Template for std::vector<std::vector<T>> as Mat<T>
	 **/
	template<typename T>
	struct Mat : public std::vector< std::vector<T> > {};

	template<typename T>
	void createMat(Mat<T>& A, int width, int height){
		for (int i = 0; i < height; i++) {
			A.push_back(vector<T>(width));
		}
	}

	template<typename T>
	void fillMat(Mat<T>& A, T value, int width, int height) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				 A[i][j] = value;
			}
		}
	}

	template<typename T>
	void printMat(const Mat<T> A, int width, int height) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				cout << A[i][j];
			}
			cout << endl;
		}
	}

class Labyrinthe : public Environnement {
	// Public Methodes
	public:
		Labyrinthe (char*);
		int width () { return _height;}	// retourne la largeur du labyrinthe.
		int height () { return _width;}	// retourne la longueur du labyrinthe.
		char data (int i, int j)
		{
			return _density[i][j];
		}	// retourne la case (i, j).
		// Permet d'écrire la matrice A dans un fichier txt (format int, char)
		void printInFileMat(const Mat<int>& A, const char* fname);
		void printInFileMat(const Mat<char>& A, const char* fname);
	// Private Methodes
	private:
		void readFile(string fname);		      // Lit un fichier .txt et créer le labyrinthe qu'il contient
		void findTex(char tmp[], const char c);   // Parcours les tables de correspondances et crée les affiches
		int  countWalls(Mat<char> A);
		void countAllData(Mat<char> A);
		void sortWallsAndPicts(Mat<char> A);

    //#s
    // Public attributes
    public:
    	static Sound*	_win;
		static Sound*	_lose;
		static Sound*	_ambiance;
		static Sound*	_save;
		//~ Matrice du labyrinthe en ascii
        Mat<char> _ascii;

    // Private attributes
    private:
        // Dimensions
        int _width;
        int _height; 
        //
        vector<vector<int>> _density;
        // Distance matrix
        vector<vector<int>> _distances;
        // Objects
        map<int*, int> _objects;
        
		//~ Table de correspondances et table contenant le nom des fichiers de texture pour les affiches
		//~ Exemple if('a' = texTab[i]) alors texFile[i] = fichier texture correspondant à 'a'
		vector<string> texFile;
		vector<char> texTab;
		 
    // Public methods
    public:
        bool isValid(int x, int y);
        bool isAccessible(int x, int y);
        bool isFree(int x, int y);
        bool isFree(double, double, Mover*);
        int getDistance(int x, int y);
        bool update(double, double, Mover*);
        void free(int x, int y);
        void free(Mover*);
        bool updateBox(int x, int y);
    // Private methods
    private:
        void computeDensity(vector<vector<char>>);
        void computeDistances();
        //
        int updateDistance(int x, int y);            
    //#e    
		
};
#endif
