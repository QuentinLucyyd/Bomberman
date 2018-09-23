#ifndef OBJECTMANAGER_HPP
# define OBJECTMANAGER_HPP

# include <vector>

# include "ObjectManager.hpp"
# include "../GameObjects/includes/GameObject.hpp"
# include "../enums/enumGameObjectType.hpp"
# include "../GameObjects/includes/Bomb.hpp"
# include "../../Game/includes/enumControls.hpp"
# include "../LevelManager/LevelManager.hpp"
# include "../../Engine/includes/Engine.hpp"

class ObjectManager {
public:
	ObjectManager( Engine & engine ); // may need reference to playstate
	ObjectManager( void ); // may need reference to playstate
	~ObjectManager( void );

	void	update(eControls key, double deltaTime);
	void	render(void);
	void	requestMove(GameObject *actor, eControls key); // returns direction // or void and calls move directly
	int		canMove(int x, int y);
	void	move( GameObject *actor, int vectorDifference );
	void	checkCollision( void );
	int		getVectorDifference(GameObject *actor);

	Engine						*engine;
	std::vector<GameObject *>	map;
	GameObject					*player;
	Bomb						*bomb;

protected:

private:

};

#endif