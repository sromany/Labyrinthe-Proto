#ifndef LABYRINTHE_H
	#define LABYRINTHE_H
	#include "Environnement.h"

	#include <stdlib.h>
	#include <stdio.h>
	#include <iostream>
	#include <fstream>
	#include <vector>
	#include <iomanip>
	#include <string>
	#include <algorithm>
	#include <regex>
	
	#define MUR 1
	#define CHASSEUR 2
	#define GARDIEN 3
	#define BOX 5	
	#define TRESOR 7
	
	using namespace std;

	typedef struct coord {
		int i, j;
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

        private:
            //~ Matrice du labyrinthe en ascii
            Mat<char> ascii;

            //~ Matrice des distance
            Mat<int> distances;

            //~ Dimension du labyrinthe
            int lab_width;
            int lab_height;                      

            //~ Table de correspondances et table contenant le nom des fichiers de texture pour les affiches
            //~ Exemple if('a' = texTab[i]) alors texFile[i] = fichier texture correspondant à 'a'
            vector<string> texFile;
            vector<char> texTab;
            
		public:
			// Point de vies des entité
			int * point_de_vie;
						
            //~ Matrice de colision
            Mat<int> density;
			
        public:
            Labyrinthe (char*);
            int width () { return lab_height;}	// retourne la largeur du labyrinthe.
            int height () { return lab_width;}	// retourne la longueur du labyrinthe.
            char data (int i, int j)
            {
                return density[i][j];
            }	// retourne la case (i, j).
			bool isAccessible(int x, int y);
			
        private:
            void readFile(string fname);		      // Lit un fichier .txt et créer le labyrinthe qu'il contient
            void findTex(char tmp[], const char c);   // Parcours les tables de correspondances et crée les affiches

            int  countWalls(Mat<char> A);
            void countAllData(Mat<char> A);
            void sortWallsAndPicts(Mat<char> A);
            void sortElements(Mat<char> A);
            void sortMovers(Mat<char> A);

            int distance(Mat<char>& A, int dNew, int i, int j, coord orig);

            void makeDensity(Mat<char> A, Mat<int>& B);
            //#s
            void makePCC(Mat<int> A, Mat<int>& B);
            //#e
            void printInFileMat(const Mat<int>& A, const char* fname);

	};
#endif
