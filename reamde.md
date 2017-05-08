/*
 * Reamde.md
 * 
 * Copyright 2017 LABYRINTHE-PROTOTYPE 
 * Stephane Romany,
 * Carlos Anderson Ferreira Da Silva
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should not have received a copy of the GNU General Public License
 * along with this program because we don't realy care at this moment...
 * 
 */


=====Implémentation du Labyrinthe=======================================\
	-Parsing d'un labyrinthe format txt:                                | 
		-Symbol lettre de 'a' à 'z' -> les Affiches                     #              
		-Symbol lettre 'x'          -> les Caisses                      |
		-Symbol lettre 'T'          -> le Trésor                        #
		-Symbol lettre 'G'          -> les Gardiens                     |                  
		-Symbol lettre 'C'          -> le Chasseur                      #
		-Symbol + - |               -> les Murs                         |               
	                                                                    #
                                                                        |
	-Création des matrices:                                             #
		-Density  -> La matrice pour la détection de colisions          |
		-Distance -> La matrice des distances de PCC                    #
		-Ascii    -> Une copie du labyrinthe format char en mémoire     |
========================================================================/



=====Implémentation des murs============================================\
	-Positionnement des murs                                            |
		-Verticaux et horizontaux                                       #
		-Remplissage de la matrice Density [i][j] = MURS                |
    -Positionnement des affiches                                        #
        -Verticales et horizontales                                     |  
        -Remplissage de la matrice Density [i][j] = MURS                #     
        -Ajout des textures via la fonction wall_texture                |  
========================================================================/



=====Implémentation des boites==========================================\
	-Positionnement des boites                                          |
		-Remplissage de la matrice Density [i][j] = BOX                 #
		-Ajout d'une fonction remove pour supprimer les caisses         |
========================================================================/



=====Implémentation du chasseur=========================================\
	-Déplacement du chasseur:                                           |
		-Dans sa propre Case == CHASSEUR                                #
		-Dans les Cases == GARDIEN, BOX, MURS -> Colision               |
		-Dans la Case == TRESOR               -> Colision mais Victoire #
																	    |
	-Mode "tir":                                                        #
		-Colisions de la boule de feu avec:                             |
			-Des boites    -> Certaines destructibles.                  #
			-Des Gardiens  -> Perdent des pv.                           |
				-Animations tomber s'il sont touchés                    #
				-Animations rester au sol s'il sont morts               |
			-Des murs.     -> Collisions                                #
========================================================================/


    
=====Implémentation des gardiens========================================\
	-Déplacement du chasseur:                                           |
		-Dans sa propre Case == GARDIEN             -> free             #
		-Dans les Case == EMPTY                     -> free             |
		-Dans les Cases == GARDIEN =/= de la sienne -> collision        # 
		-Dans                                                           |
		                                                                #
		                                                                |
	-Comportement:                                                      #
		-PATROUILLE -> Se déplace tout droit jusqu'a                    |
		rencontrer un obstable puis choisit un angle                    #
		aléatoire en vérifiant les cases autour de lui.                 | 
		                                                                #
		-DEFENSE -> Similaire au mode précédent mais                    |
		privilégie les angles qui le rapproche du trésor.               #  
	                                                                    |
	-Mode "tir":                                                        #
		-Colisions de la boule de feu avec:                             |
			-Des boites  -> destructible                                #  
			-Le Chasseur -> Perd des pv + Fin de partie si ses pv = 0   |   
			-Des murs.                                                  #  
                                                                        |
========================================================================/



=====Implémentation du sons=============================================\
    -Ajout des sons pour les gardiens et le chasseur:                   |    
        -Tir de la boule de feu.                                        #  
        -Impact de la boule de feu.                                     |       
        -Se fait toucher par une boule de feu.                          #
        -Tiennent compte de la distance.                                |
    -Ajout des sons d'ambiances                                         # 
        -Défaite                                                        |       
        -Victoire                                                       #   
        -Ambiance en début de partie                                    | 
    -Ne tiens pas encore compte du panoramique                          #
        -Par rapport à l'origine du son                                 |
========================================================================/
