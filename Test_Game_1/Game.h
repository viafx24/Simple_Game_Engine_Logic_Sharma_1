#pragma once

#include<iostream>
#include<vector>
#include<ctime>
#include<sstream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

/*
	Class that acts as the game engine.
	Wrapper class.
*/

class Game
{

// le decoupage est vraiment intéressant; déja toute la partie private avec:
// les variables de fenètre, les ressources, les textes, la logic du jeu, les objets du jeu, puis les fonctions privées.
// on sent l'experience du programmeur qui sépare bien les différents elements en classe pour faire du code réutilisable et facile à lire
// dans l'hypothèse où le jeu a une taille minimum (dans de nombreux cas, tout foutre dans le main est une alternative intéressante mais
// que pour les petits jeux sinon rapidement on se fait chier à monter et à descendre dans le code et on n'y pige plus rien.

// ensuite on a la partie public dans laquelle on retrouve:
// constructeur et destructeur, accesseur (on notera la présence du const devant qui indique que rien ne sera modifier avec cette méthode)
// et ensuite les fonctions. on notera:
// une fonction de création d'ennemi, une fonction de gestion d'evenement de base
//4 fonctions d'update (concernant la logique du jeu) et 3 fonctions de rende (gestion de l'affichage).

private:
	//Variables
	//Window
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event ev;

	//Mouse positions
	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosView;

	//Resources
	sf::Font font;

	//Text
	sf::Text uiText;

	//Game logic
	bool endGame;
	unsigned points; // si aucun type n'est spécifié, int est assumé
	int health;
	float enemySpawnTimer;
	float enemySpawnTimerMax;
	int maxEnemies;
	bool mouseHeld;

	//Game objects
	std::vector<sf::RectangleShape> enemies;
	sf::RectangleShape enemy;

	//Private functions
	void initVariables();
	void initWindow();
	void initFonts();
	void initText();
	void initEnemies();

public:
	//Constructors / Destructors
	Game();

	// je ne sais pas pourquoi le destructeur est virtuel (sans doute car il ne sera jamais appelé)
	// l'auteur conseille d'éviter d'utiliser les destructeurs (mauvaise pratique) . intuitivement,j'aurais 
	// pensé à l'inverse qu'il s'agissait d'une bonne pratique à la base de la poo.

	virtual ~Game();

	//Accessors
	
	// renvoie 1 si la fenètre est ouverte; const permet de dire qu'on interdit à la méthode de changer quoi que ce soit (variables privé)
	const bool running() const;
	const bool getEndGame() const;

	//Functions
	void spawnEnemy();


	void pollEvents();
	void updateMousePositions();
	void updateText();
	void updateEnemies();
	void update();

	void renderText(sf::RenderTarget& target);
	void renderEnemies(sf::RenderTarget& target);
	void render();
};
