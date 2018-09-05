#ifndef ENGINE_HPP
# define ENGINE_HPP
# define GLEW_STATIC

# include <GL/glew.h>
# include <GL/glut.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include <GLFW/glfw3.h>
# include <glm/glm.hpp>

# include <vector>
# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <iostream>

# include <ft2build.h>
# include <freetype/freetype.h>
# include <freetype/ftglyph.h>
# include <freetype/ftoutln.h>
# include <freetype/fttrigon.h>
# include FT_FREETYPE_H

# include <map>

# include "../../Sound/includes/Sound.hpp"
# include "../../Game/includes/enumControls.hpp"
# include "TextRenderer.hpp"
# include "Shader.hpp"
# include <string.h>
# include <array>

# include "../../Sound/includes/Sound.hpp"
# include "../../Game/includes/enumControls.hpp"
# include "structControls.hpp"
# include "../../../States/includes/enumState.hpp"

class	Engine {
	public:
		Engine();
		~Engine();

		void			engineInit( void );
		void			render( void );
		void			clear( void );

		/********************************************************************************************/
		/*	Text Rendering Functions 																*/
		/*	print2DText() Takes in the text and X & Y position to print out on the Window			*/
		/*	current actively pressed key that is set using the key_callback function				*/
		/********************************************************************************************/

		void			print2DText(std::string text, int pos_x, int pos_y, GLubyte red, GLubyte green, GLubyte blue);
		void			printMenu(std::array<std::string, 5> menuItems, int pos_x, int pos_y, int menuIndex, std::string backgroundPath);
		
		void			printMenu(std::array<std::string, 5> menuItems, int menuIndex, std::string backgroundPath);

		/********************************************************************************************/
		/*	Keyboard Input Functions 																*/
		/*	getkey( void ) gets the "_Active" private memeber of the Engine which containts the 	*/
		/*	current actively pressed key that is set using the key_callback function				*/
		/********************************************************************************************/
		eControls		getInput( void );
		static void processKeys(unsigned char key, int x, int y);

		/* Timer functions */
		void			FPSManager();

		/* Exception */
		class GLFWInitializationError: public std::exception {
			virtual const char* what() const throw();
		};

		class GLEWInitializationError: public std::exception {
			virtual const char* what() const throw();
		};

		eState			state;

	private:
		GLFWwindow				*_Window;
		Engine					*_MYWindow;
		int						_WindowHeight;
		int						_WindowWidth;
		eControls				_ActiveKey;
		Sound					_SoundEngine;
		Shader					_Shader;
		glfreetype::font_data	_Font;
		sControls				_sControls;
		bool					_getKey( int key );
};

#endif // !ENGINE_HPP