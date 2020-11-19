#include "Game.h"

//Private functions
void Game::initVariables()
{
	// puisque 'on utilise new, this->windows est un pointeur qu'on initialise à zero avec nullptr ce qui est une meilleur pratique
	// que d'utiliser NULL car NULL est un int fixé à zero alors que nullptr est forcement un pointeur initilialisé à zero.
	this->window = nullptr;
	
	//Game logic
	this->endGame = false;
	this->points = 0;
	this->health = 20;
	this->enemySpawnTimerMax = 20.f;// 20 pour un int; 20.0 si on veut un double, 20.f si on veut un float. C++ (pas SFML)
	this->enemySpawnTimer = this->enemySpawnTimerMax;
	this->maxEnemies = 5;
	this->mouseHeld = false;
}

void Game::initWindow()
{
	this->videoMode.height = 600;
	this->videoMode.width = 800;
	
	// l'utilisation du new/delete est en général à proscrire. Je ne sais pas pourquoi ici, le choix a été fait de travailler
	// avec un pointeur pour windows plutôt que l'objet directement. peut-être pour anticiper le fait que certaine méthode passeront
	// la fenètre en argument et pour eviter de copier? pas clair encore pour moi. J'aurais voulu faire un test sans new/delete
	// mais cela necessite de changer tous les -> par des . aprés window. un peu lourd. on verra plus tard. Ce qui compte c'est que
	// ca marche et que je comprends les deux écritures même si je sais pas pourquoi l'utilisation du new ici a été preferé.

	this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close);

	this->window->setFramerateLimit(60);
}

void Game::initFonts()
{
	if (this->font.loadFromFile("Fonts/Dosis-Light.ttf"))
	{
		std::cout << "ERROR::GAME::INITFONTS::Failed to load font!" << "\n";
	}
}

void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(24);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("NONE");
}

void Game::initEnemies()
{
	this->enemy.setPosition(10.f, 10.f);
	this->enemy.setSize(sf::Vector2f(100.f, 100.f));
	this->enemy.setFillColor(sf::Color::Cyan);
	//this->enemy.setOutlineColor(sf::Color::Green);
	//this->enemy.setOutlineThickness(1.f);
}

//Constructors / Destructors
Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initFonts();
	this->initText();
	this->initEnemies();
}

Game::~Game()
{

	// ce code sera executé même si le destructeur est viruel car il n'est "pure virtuel" (cas d'une classe abstraite)
	delete this->window;
}

//Accessors
const bool Game::running() const
{
	return this->window->isOpen();
}

const bool Game::getEndGame() const
{
	return this->endGame;
}

//Functions
void Game::spawnEnemy()
{
	/*
		@return void

		Spawns enemies and sets their types and colors. Spawns them at random positions.
		-Sets a random type (diff).
		-Sets a random position.
		-Sets a random color.
		-Adds enemy to the vector.
	*/

	this->enemy.setPosition(
		static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - this->enemy.getSize().x)),
		0.f
	);

	//Randomize enemy type
	int type = rand() % 5;

	switch (type)
	{
	case 0:
		this->enemy.setSize(sf::Vector2f(10.f, 10.f));
		this->enemy.setFillColor(sf::Color::Magenta);
		break;
	case 1:
		this->enemy.setSize(sf::Vector2f(30.f, 30.f));
		this->enemy.setFillColor(sf::Color::Blue);
		break;
	case 2:
		this->enemy.setSize(sf::Vector2f(50.f, 50.f));
		this->enemy.setFillColor(sf::Color::Cyan);
		break;
	case 3:
		this->enemy.setSize(sf::Vector2f(70.f, 70.f));
		this->enemy.setFillColor(sf::Color::Red);
		break;
	case 4:
		this->enemy.setSize(sf::Vector2f(100.f, 100.f));
		this->enemy.setFillColor(sf::Color::Green);
		break;
	default:
		this->enemy.setSize(sf::Vector2f(100.f, 100.f));
		this->enemy.setFillColor(sf::Color::Yellow);
		break;
	}

	//Spawn the enemy
	this->enemies.push_back(this->enemy);
}

void Game::pollEvents()
{
	//Event polling
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->ev.key.code == sf::Keyboard::Escape)
				this->window->close();
			break;
		}
	}
}

void Game::updateMousePositions()
{
	/**
		@ return void

		Updates the mouse positions:
		- Mouse position relative to window (Vector2i)
	*/

	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::updateText()
{
	// on nage ici en plein C++ intéressant à retenir.
	// on forme déja un streamstring (librairie standard)
	// dans lequel on met ce qu'on veut (text, float etc...)
	// comme on ferait dans un flux cout

	std::stringstream ss;

	ss << "Points: " << this->points << "\n"
		<< "Health: " << this->health << "\n";

	// puis on revient dans notre librairie sfml pour faire un setString sur le uiText.
	// pour récupérer le contenu du string, on utilise str() de la librairie standard (ss.str())

	this->uiText.setString(ss.str());
}

void Game::updateEnemies()
{
	/**
		@return void

		Updates the enemy spawn timer and spawns enemies
		When the total amount of enemies is smaller than the maximum.
		Moves the enemies downwards.
		Removes the enemies at the edge of the screen. //TODO
	*/

	//Updating the timer for enemy spawning
	if (this->enemies.size() < this->maxEnemies)
	{
		if (this->enemySpawnTimer >= this->enemySpawnTimerMax)
		{
			//Spawn the enemy and reset the timer
			this->spawnEnemy();
			this->enemySpawnTimer = 0.f;
		}
		else
			this->enemySpawnTimer += 1.f;
	}

	//Moving and updating enemies
	for (int i = 0; i < this->enemies.size(); i++)
	{
		bool deleted = false;

		// on deplace les ennemis à chaque iteration vers le bas
		this->enemies[i].move(0.f, 5.f);

		// si l'ennemi depasse l'écran en bas, on le delete
		if (this->enemies[i].getPosition().y > this->window->getSize().y)
		{
			// si je comprends bien le this->enemies.begin() + i est un iterateur. il parcourt
			// tout jusquà s'arrêter sur notre element. pourquoi est ce necessaire à bas niveau?
			// pas encor clair. 

			// le point étrange c'est qu'il n'y a pas d'initilisation de l'iterateur ni utilisation
			// du mot clés auto pour  générer une initialisation automatique.

			this->enemies.erase(this->enemies.begin() + i);
			// on perd un point de vie.
			this->health -= 1;
			std::cout << "Health: " << this->health << "\n";
		}
	}

	//Check if clicked upon
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// interdit de maintenir le bouton gauche appuyé.
		if (this->mouseHeld == false)
		{
			this->mouseHeld = true;
			bool deleted = false;

			// size_t indique que c'est le resultat d'une taille (on pourrait dire que cela remplace int)
			// et donc faire comme si c'était un int.

			for (size_t i = 0; i < this->enemies.size() && deleted == false; i++)
			{

				// Un point central de programmation: on regarde déja si le buton est appuyé
				// si oui, on regarde si la position de la souris est inclus dans les limites de 
				// notre objet ennemi. 
				// pas evident de comprendre pourquoi on a besoin à la fois du getglobalbounds et du contains
				// intuitivement, je n'aurai utilisé que contain.

				if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
				{
					//Gain points
					if(this->enemies[i].getFillColor() == sf::Color::Magenta)
						this->points += 10;
					else if (this->enemies[i].getFillColor() == sf::Color::Blue)
						this->points += 7;
					else if(this->enemies[i].getFillColor() == sf::Color::Cyan)
						this->points += 5;
					else if (this->enemies[i].getFillColor() == sf::Color::Red)
						this->points += 3;
					else if (this->enemies[i].getFillColor() == sf::Color::Green)
						this->points += 1;

					std::cout << "Points: " << this->points << "\n";

					//Delete the enemy

					// le fait de passer deleted à true fait qu'on sort de la boucle et qu'on ne pourra
					// pas deleter plusieurs objets en maintenant appuyer le bouton gauche
					deleted = true;
					this->enemies.erase(this->enemies.begin() + i);
				}
			}
		}
	}
	else
	{
		this->mouseHeld = false;
	}
}

void Game::update()
{
	this->pollEvents();

	if (this->endGame == false)
	{
		this->updateMousePositions();

		this->updateText();

		this->updateEnemies();
	}

	//End game condition
	if (this->health <= 0)
		this->endGame = true;
}

// je crois que l'utilisation de RenderTarget sert si on utilisait plusieurs fenètre (c'est la class mère de renderwindows)
// j'imagine et ce sont les points merdiques du C++ qu'on met le & pour indiquer qu'on veut une reference sur la fenètre
// afin de ne pas la copier? La fenètre envoyé sera simplement this->windows ou plus exactement *this->windows: cette écriture
// est mysterieuse pour moi car la fleche sert déja à dereferencer et l'étoile aussi. donc il y a redondance et je ne pige pas.
// a moins que écrit comme cela, on indique qu'on passe un pointeur en argument . Tout cela sera testé en debuggage.

void Game::renderText(sf::RenderTarget& target)
{
	target.draw(this->uiText);
}

void Game::renderEnemies(sf::RenderTarget& target)
{
	//Rendering all the enemies
	// ici encore je crois que l'utilsiation d'une réference sur e evite la copie de l'objet mais c'est pas clair et
	// je ne suis pas sûr.

	// auto signifie : detection automatique de type.
	// les : correspondent à une boucle de type range, comme dans matlab, trés pratique.

	// je ne comprends toujours pas cette écriture (voir note)
	// celle la (sans le eperluette) fonctionne tout aussi bien:  for (auto e : this->enemies)

	// ici le range ne fonctionne pas exactement comme dans matlab: la deuxième partie (this->ennemies) n'a pas besoin
	// d'être la fin (une sorte de limite, de end comme dans matlab) mais simplement le vecteur, la map etc.. a parcourir.

	for (auto &e : this->enemies)


	{
		// écriture trés peu user friendly; target est la fenètre cible
		// e correspond à l'ennemi mais pourquoi e et pas &e; c'est la caractéristique du C++ d'être peu lisible et peu clair.
		// que fait le &e dans la boucle, declare il une réference sur e qu'on peut ensuite appeler e? vraiment merdique.
		target.draw(e);
	}
}

void Game::render()
{
	/**
		@return void

		- clear old frame
		- render objects
		- display frame in window

		Renders the game objects.
	*/

	this->window->clear();

	//Draw game objects

	// l'utilisation du * est pas clair du tout. 
	// si il s'agit simplement du dereferencement du pointeur this->windows car renderennmies attend l'objet en argument et non pas l
// le pointeur.

	this->renderEnemies(*this->window);
	
	this->renderText(*this->window);

	this->window->display();
}
