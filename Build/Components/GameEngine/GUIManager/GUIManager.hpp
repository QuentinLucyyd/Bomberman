# ifndef GUIMANAGER_HPP
# define GUIMANAGER_HPP

# include "../../Engine/includes/Engine.hpp"
# include "../GameObjects/includes/GameObject.hpp"
# include <ctime>

class GUIManager {
	public:
		GUIManager( Engine & engine);
		GUIManager( void );
		GUIManager(GUIManager const & src);
		GUIManager	&operator=(GUIManager const &rhs);
		~GUIManager( void );
	
	void			update( GameObject *player, int	elapsedSec, int playerScore, int level);
	void			render( void );

	Engine			*engine;
	std::string		_lives;
	std::string		_level;
	std::string		_timeElapsed;
	std::string		_points;
};

#endif // !GUIMANAGER_HPP