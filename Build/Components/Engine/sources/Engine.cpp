# define STB_IMAGE_IMPLEMENTATION
# include "../includes/Engine.hpp"

/********************************************************************************/
/*	Required functions															*/
/********************************************************************************/
unsigned int VBO, VAO, EBO;
unsigned int texture1, texture2;

unsigned int BVBO, BVAO, BEBO;
unsigned int Btexture1, Btexture2;

glm::vec3 cubePositions[] = {
  glm::vec3( 0.48f, -0.48f,  -0.1f), // same as camera offset
};

int		Engine::held = 1;

Engine::Engine(): _deltaTime(0.0f), _WindowHeight(600), _WindowWidth(800), bombAnim(0), bombMove(0.005f), explodeAnim(0), explodeMove(0.2f), _Fullscreen(true){
	std::cout << "Engine constructed" << std::endl;
	/* GLFW Initialization */
	if (!glfwInit()) {
		std::string errMsg = "[Error: Code 00 - (Render Engine)] : GLFW Failed to Initialize, Please Check your Includes";
		throw Engine::EngineErr(errMsg);
	}

	// GLFW Window Hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	this->_Monitor = glfwGetPrimaryMonitor();
	this->_Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	this->_Window = glfwCreateWindow(this->_WindowWidth, this->_WindowHeight, "Bomberman", NULL, NULL);
	if( this->_Window == NULL ){
		std::string errMsg = "[Error: Code 01 - (Render Engine)] : Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version";
		throw Engine::EngineErr(errMsg);
	}
	glfwMakeContextCurrent(this->_Window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::string errMsg = "[Error: Code 01 - (Render Engine)] : GLFW Failed to Initialize, Please Check your Includes";
		throw Engine::EngineErr(errMsg);
	}

	/** Set OpenGL & GLFW options **/
	glfwSetInputMode(this->_Window, GLFW_STICKY_KEYS, GL_TRUE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Initialize Sound, Text & Camera Engine */
	this->_TextEngine.init("Assets/Fonts/neon_pixel.ttf", 30, this->_WindowWidth, this->_WindowHeight);
	this->_SoundEngine.init();
	this->_SoundEngine._volume = 0.5f;
	//this->setFullScreen();
	this->engineInit();
	this->_Camera.init(glm::vec3(0.48f, -1.1f, 2.7f)); // use position of player in future

	// initialising controls struct
	this->_sControls.LEFT_KEY = GLFW_KEY_LEFT;//263;
	this->_sControls.UP_KEY = GLFW_KEY_UP;//265;
	this->_sControls.RIGHT_KEY = GLFW_KEY_RIGHT;//262;
	this->_sControls.DOWN_KEY = GLFW_KEY_DOWN;//264;
	this->_sControls.ENTER_KEY = GLFW_KEY_ENTER;//257;
	this->_sControls.ESCAPE_KEY = GLFW_KEY_ESCAPE;//256;
	this->_sControls.FIRE_KEY = GLFW_KEY_Z;//90; defaults to z
	this->_sControls.ACTION_KEY = GLFW_KEY_X;//88; defaults to x
	return ;
}

Engine::Engine(Engine const & src) {
	this->_Monitor = src._Monitor;
}

Engine	&Engine::operator=(Engine const &rhs) {
	return *(new Engine(rhs));
}

Engine::~Engine() {
	std::cout << "Engine destructed" << std::endl;
	glfwTerminate();
}

void	Engine::BigTexture( std::string path ) {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   0.5f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &BVAO);
	glGenBuffers(1, &BVBO);
	glGenBuffers(1, &BEBO);

	glBindVertexArray(BVAO);

	glBindBuffer(GL_ARRAY_BUFFER, BVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// load and create a texture 
	// -------------------------
	// texture 1
	// ---------
	glGenTextures(1, &Btexture1);
	glBindTexture(GL_TEXTURE_2D, Btexture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	this->_BackgroundShader.use();
	this->_BackgroundShader.setInt("texture1", 0);
	this->_BackgroundShader.setInt("texture2", 1);
}

void	Engine::backgroundTexture( std::string path ) {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   0.5f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// load and create a texture 
	// -------------------------
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	this->_Shader.use();
	this->_Shader.setInt("texture1", 0);
	this->_Shader.setInt("texture2", 1);
}

void	Engine::drawBigBackground( void ) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Btexture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Btexture2);
	this->_BackgroundShader.use();

	// create transformations
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)this->_WindowWidth / (float)this->_WindowHeight, 0.1f, 100.0f);

	view = this->_Camera.GetViewMatrix();
	unsigned int modelLoc = glGetUniformLocation(this->_BackgroundShader.ID, "model");
	unsigned int viewLoc  = glGetUniformLocation(this->_BackgroundShader.ID, "view");
	
	// pass them to the shaders (3 different ways)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	this->_BackgroundShader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(2.06f, 2.06f, -1.0f));
	this->_BackgroundShader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void	Engine::drawBackground( void ) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	this->_Shader.use();

	// create transformations
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)this->_WindowWidth / (float)this->_WindowHeight, 0.1f, 100.0f);

	view = this->_Camera.GetViewMatrix();
	unsigned int modelLoc = glGetUniformLocation(this->_Shader.ID, "model");
	unsigned int viewLoc  = glGetUniformLocation(this->_Shader.ID, "view");
	
	// pass them to the shaders (3 different ways)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	this->_Shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	for(unsigned int i = 0; i < 1; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		model = glm::scale(model, glm::vec3(1.06f, 1.06f, 0.0f));
		this->_Shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

/********************************************************************************/
/*	Engine specific functions													*/
/********************************************************************************/

void	Engine::engineInit( void ) {
	/* Shader Initialization */
	this->_Shader.init("Assets/Shaders/Textures/shader.vs", "Assets/Shaders/Textures/shader.fs");
	this->_BackgroundShader.init("Assets/Shaders/Textures/shader.vs", "Assets/Shaders/Textures/shader.fs");
	this->_ModelShader.init("Assets/Shaders/Model/shader.vs", "Assets/Shaders/Model/shader.fs");
	this->_Lighting.init("Assets/Shaders/Lighting/shader.vs", "Assets/Shaders/Lighting/shader.fs");

	/* Model Initialization */
	this->_SolidWall.init("Assets/Models/Crate/89e64c1cd44944659f70b75891693405.blend.obj");
	this->_BreakableWall.init("Assets/Models/Crate-Break/89e64c1cd44944659f70b75891693405.blend.obj");
	this->_Bomb.init("Assets/Models/rusty-bomb/source/Bomb.obj");
	this->_Player.init("Assets/Models/Slime/MC-Slime.obj");
	this->_Enemy.init("Assets/Models/Enemy/89e64c1cd44944659f70b75891693405.blend.obj");
	this->_Door.init("Assets/Models/Door/89e64c1cd44944659f70b75891693405.blend.obj");
	this->_Explosion.init("Assets/Models/Explosion/89e64c1cd44944659f70b75891693405.blend.obj");
	this->_PowerUp.init("Assets/Models/PowerUp/89e64c1cd44944659f70b75891693405.blend.obj");
}

void	Engine::clear( void ) {
	glClearColor(0.0, 0.65, 0.88, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void	Engine::drawModel( eGameObjectType type, float transX, float transY, float transZ ) {
	transX = (transX -1) * 0.08f; //scaling
	transY = (transY -1) * 0.08f; //scaling
	transY = -transY; // flip about y axis
	this->_ModelShader.use();
	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(this->_Camera.Zoom), (float)this->_WindowWidth / (float)this->_WindowHeight, 0.1f, 100.0f);
	glm::mat4 view = this->_Camera.GetViewMatrix();
	this->_ModelShader.setMat4("projection", projection);
	this->_ModelShader.setMat4("view", view);

	static float rot = 0.0f;
	static float val = 0.0f;

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	//										x		y		z
	if ( type == PLAYER ) {
		model = glm::translate(model, glm::vec3(transX, transY - 0.01f, transZ - 0.01f));
		model = glm::scale(model, glm::vec3(0.0007f, 0.0007f, 0.0007f));
		model = glm::rotate(model, 1.5f, glm::vec3(0.6f, 0.0f, 0.0f));

	
		if (this->_getKey(this->_sControls.LEFT_KEY)) {
			rot = 1.6f;
			val = -0.6f;
		} else if (this->_getKey(this->_sControls.RIGHT_KEY)){
			rot = 1.6f;
			val = 0.6f;
		} else if (this->_getKey(this->_sControls.UP_KEY)) {
			rot = 3.2f;
			val = 0.6f;
		} else if (this->_getKey(this->_sControls.DOWN_KEY)) {
			val = 0.6f;
			rot = 0.0f;
		}

		if (rot != 0.0f && val != 0.0f)
			model = glm::rotate(model, rot, glm::vec3(0.0f, val, 0.0f));
	} else if (type == ENEMY) {
		model = glm::translate(model, glm::vec3(transX, transY, transZ));
		model = glm::scale(model, glm::vec3(0.010f, 0.010f, 0.010f));
	} else if (type == BOMB) {
		this->bombAnim++;
		model = glm::translate(model, glm::vec3(transX, transY + 0.009f, transZ));
		if (this->bombAnim++ < 5) {
			model = glm::scale(model, glm::vec3(this->bombMove, this->bombMove, this->bombMove));
			this->bombMove += 0.00002f;
		} else
			model = glm::scale(model, glm::vec3(this->bombMove, this->bombMove, this->bombMove));
		model = glm::rotate(model, 2.0f, glm::vec3(1.5f, 0.0f, 0.0f));
	} else if ( type == EXPLOSION ){
		this->explodeAnim++;
		model = glm::translate(model, glm::vec3(transX, transY + 0.009f, transZ));
		if (this->explodeAnim++ < 5) {
			model = glm::scale(model, glm::vec3(this->explodeMove, this->explodeMove, this->explodeMove));
			this->explodeMove -= 0.008f;
		} else {
			model = glm::scale(model, glm::vec3(this->explodeMove, this->explodeMove, this->explodeMove));
		}
	} else if ( type == DOOR || type == POWERUP ) {
		model = glm::translate(model, glm::vec3(transX, transY, transZ));
		model = glm::scale(model, glm::vec3(0.016f, 0.016f, 0.016f));
	} else {
		model = glm::translate(model, glm::vec3(transX, transY, transZ));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
	}
	this->_ModelShader.setMat4("model", model);

	switch (type) {
		case ( EXPLOSION ):
			this->_Explosion.Draw(_ModelShader);
			break;
		case ( POWERUP ):
			this->_PowerUp.Draw(_ModelShader);
			break;
		case ( DOOR ):
			this->_Door.Draw(_ModelShader);
			break;
		case ( ENEMY ):
			this->_Enemy.Draw(_ModelShader);
			break;
		case ( BOMB ):
			this->_Bomb.Draw(_ModelShader);
			break;
		case( SOLIDWALL ):
			this->_SolidWall.Draw(_ModelShader);
			break;
		case( WALL ):
			this->_BreakableWall.Draw(_ModelShader);
			break;
		case( PLAYER ):
			this->_Player.Draw(_ModelShader);
			break;
	}
	this->bombAnim = 0;
}


void	Engine::render( void ) {
	glfwSwapBuffers(this->_Window);
	glfwPollEvents();
}

void		Engine::keyBindings( int bindID ) {
	if (bindID == 1) {
		this->_sControls.LEFT_KEY = GLFW_KEY_LEFT;//263;
		this->_sControls.UP_KEY = GLFW_KEY_UP;//265;
		this->_sControls.RIGHT_KEY = GLFW_KEY_RIGHT;//262;
		this->_sControls.DOWN_KEY = GLFW_KEY_DOWN;//264;
		this->_sControls.ENTER_KEY = GLFW_KEY_ENTER;//257;
		this->_sControls.ESCAPE_KEY = GLFW_KEY_ESCAPE;//256;
		this->_sControls.FIRE_KEY = GLFW_KEY_Z;//90; defaults to z
		this->_sControls.ACTION_KEY = GLFW_KEY_X;//88; defaults to x
	} else if (bindID == 2) {
		this->_sControls.LEFT_KEY = GLFW_KEY_A;//263;
		this->_sControls.UP_KEY = GLFW_KEY_W;//265;
		this->_sControls.RIGHT_KEY = GLFW_KEY_D;//262;
		this->_sControls.DOWN_KEY = GLFW_KEY_S;//264;
		this->_sControls.ENTER_KEY = GLFW_KEY_ENTER;//257;
		this->_sControls.ESCAPE_KEY = GLFW_KEY_ESCAPE;//256;
		this->_sControls.FIRE_KEY = GLFW_KEY_K;//90; defaults to z
		this->_sControls.ACTION_KEY = GLFW_KEY_L;//88; defaults to x
	} else if (bindID == 3) {
		this->_sControls.LEFT_KEY = GLFW_KEY_LEFT;//263;
		this->_sControls.UP_KEY = GLFW_KEY_UP;//265;
		this->_sControls.RIGHT_KEY = GLFW_KEY_RIGHT;//262;
		this->_sControls.DOWN_KEY = GLFW_KEY_DOWN;//264;
		this->_sControls.ENTER_KEY = GLFW_KEY_ENTER;//257;
		this->_sControls.ESCAPE_KEY = GLFW_KEY_ESCAPE;//256;
		this->_sControls.FIRE_KEY = GLFW_KEY_Q;//90; defaults to z
		this->_sControls.ACTION_KEY = GLFW_KEY_W;//88; defaults to x
	}
}

void		Engine::saveGame( void ) {
	std::string fileName = "save.save";
	std::string filePath = ".save/" + fileName;
	std::ofstream saveFile;
	saveFile.open(filePath);
	std::string data = std::to_string(this->_Save.health) + "," + std::to_string(this->_Save.level)
	+ ',' + std::to_string(this->_Save.points) + ',' + std::to_string(this->_Save.remainingTime);
	saveFile << data;
	std::cout << "DATA: " << data << std::endl; 
	saveFile.close();
}

template <class Container>
void split2(const std::string& str, Container& cont, char delim = ' ')
{
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}

void		Engine::loadGame( void ) {
	std::ifstream saveFile (".save/save.save");
	std::string line;
	if (saveFile.is_open()) {
		getline (saveFile,line);
	}
	saveFile.close();

	std::vector<std::string> words;
	split2(line, words, ',');
	std::copy(words.begin(), words.end(),
	std::ostream_iterator<std::string>(std::cout, "\n"));
}

void		Engine::playSound( std::string soundPath, bool loop) {
	if (!this->_Mute)
		this->_SoundEngine.play2DSound(soundPath, loop);
}

void		Engine::stopSound( void ) {
	this->_SoundEngine.stopSound();
}

void		Engine::muteSound( void ) {
	this->_Mute = !_Mute;
}

void		Engine::setVolume(float level) {
	this->_SoundEngine._volume = level;
}
/********************************************************************************/
/*	Text and Menu Rendering Function											*/
/********************************************************************************/

void		Engine::print2DText(std::string text, float pos_x, float pos_y, float red, float green, float blue, float scale) {
	this->_TextEngine.RenderText(text, pos_x, pos_y, scale , glm::vec3(red, green, blue));
}

void		Engine::printMenu(std::vector<std::string> menuItems, float pos_x, float pos_y, int menuIndex, std::string backgroundPath) {
	// pos_x = 0;
	// pos_y = 0;
	// menuIndex = 0;
	// backgroundPath = " ";
	static_cast<void>(pos_x);
	static_cast<void>(pos_y);
	static_cast<void>(menuIndex);
	static_cast<void>(pos_x);
	static_cast<void>(backgroundPath);
	float	x = 20;
	float 	y = 20;
	for (int i = menuItems.size() - 1;i >= 0; i--) {
		this->print2DText(menuItems[i], x, y, 0.3, 0.7f, 0.9f, 1.0f);
		y += 50;
	}
}

void		Engine::printIntro() {
	std::string introtext = "Welcome to Bomberman";
	/* Render Menu Header Text */
	this->print2DText(introtext, 20,20, 0.5, 0.8f, 0.2f, 1.0f);
}

void		Engine::printMenu(std::vector<std::string> menuItems, std::string menuHeading, int menuIndex, std::string backgroundPath) {
	float		y = (this->_WindowHeight / 2) - (menuItems.size() * 32);
	backgroundPath = "";
	
	/* Render Menu Header Text */
	float headingLength = menuHeading.length() * 1.5;
	this->print2DText(menuHeading, (this->_WindowWidth / 2) - ((headingLength / 2) * 25), y + (50 * menuItems.size() + 20), 0.5, 0.8f, 0.2f, 1.5f);

	for (int i = menuItems.size() - 1;i >= 0; i--) {
		float length = menuItems[i].length();
		if (i == menuIndex)
			this->print2DText(menuItems[i], (this->_WindowWidth / 2) - ((length / 2) * 25), y, 0.5, 0.8f, 0.2f, 1.0f);
		else
			this->print2DText(menuItems[i], (this->_WindowWidth / 2) - ((length / 2) * 25), y, 1.0f, 1.0f, 1.0f, 1.0f);
		y += 50;
	}
}

int			Engine::menuHandler( eControls key, int & menuIndex, int lastIndex ){
	switch (key){
			case UP:
			if (!(this->held)) {
				menuIndex == 0 ? menuIndex = 0 : menuIndex--;
				this->_SoundEngine.playSoundSource(this->_SoundEngine._Selection, false);
			}
			this->held = 1;
			break;
		case DOWN:
			if (!(this->held)) {
				menuIndex == lastIndex ? menuIndex = lastIndex : menuIndex++;
				this->_SoundEngine.playSoundSource(this->_SoundEngine._Selection, false);
			}
			this->held = 1;
			break;
		case ENTER:
			if (!(this->held)) {
				this->_SoundEngine.playSoundSource(this->_SoundEngine._Selection, false);
				return (1);
			}
			this->held = 1;
			break;
		case IDLEKEY:
			this->held = 0;
			break;
		case ESCAPE:
			if (!(this->held)) {
				this->state = BACK;
				this->_SoundEngine.playSoundSource(this->_SoundEngine._Selection, false);
			}
			this->held = 1;
			break;
		default:
			break;
	};
	return (0);
}

/********************************************************************************/
/*	Keyboard input functions													*/
/********************************************************************************/

void		Engine::controlChange(int i)
{
	if (i == 1)
	{
		this->_sControls.LEFT_KEY = GLFW_KEY_A;
		this->_sControls.UP_KEY = GLFW_KEY_W;
		this->_sControls.RIGHT_KEY = GLFW_KEY_D;
		this->_sControls.DOWN_KEY = GLFW_KEY_S;
		this->_sControls.FIRE_KEY = GLFW_KEY_SLASH;
		this->_sControls.ACTION_KEY = GLFW_KEY_PERIOD;
	}
	else if (i == 0)
	{
		this->_sControls.LEFT_KEY = GLFW_KEY_LEFT;
		this->_sControls.UP_KEY = GLFW_KEY_UP;
		this->_sControls.RIGHT_KEY = GLFW_KEY_RIGHT;
		this->_sControls.DOWN_KEY = GLFW_KEY_DOWN;
		this->_sControls.FIRE_KEY = GLFW_KEY_Z;
		this->_sControls.ACTION_KEY = GLFW_KEY_X;
	}
}

eControls	Engine::getInput(){
	// run through array or struct of values. struct most likely
	if (this->_getKey( this->_sControls.LEFT_KEY ))
		return (LEFT);
	if (this->_getKey( this->_sControls.UP_KEY ))
		return (UP);
	if (this->_getKey( this->_sControls.RIGHT_KEY ))
		return (RIGHT);
	if (this->_getKey( this->_sControls.DOWN_KEY ))
		return (DOWN);
	if (this->_getKey( this->_sControls.ENTER_KEY ))
		return (ENTER);
	if (this->_getKey( this->_sControls.ESCAPE_KEY ))
		return (ESCAPE);
	if (this->_getKey( this->_sControls.ACTION_KEY ))
		return (ACTION);
	if (this->_getKey( this->_sControls.FIRE_KEY ))
		return (FIRE);
	return (IDLEKEY);
}

bool		Engine::_getKey( int key ) {
	int state = glfwGetKey(this->_Window, key);
	if (state == GLFW_PRESS)
		return (true);
	return (false);
}

/********************************************************************************************/
/*	Display Monitor Functions 																*/
/* */
/********************************************************************************************/

void		Engine::setFullScreen( void ) {
	glfwSetWindowMonitor( this->_Window, this->_Monitor, 0, 0, this->_WindowWidth, this->_WindowHeight, 0 );
	this->_Fullscreen = true;
}

void		Engine::setWindowed( void ) {
	glfwSetWindowMonitor( this->_Window, NULL , 0, 0, this->_WindowWidth, this->_WindowHeight, 0 );
	this->_Fullscreen = false;
}

void		Engine::setResolution( int width, int height) {
	this->_WindowHeight = height;
	this->_WindowWidth = width;
	if (!this->_Fullscreen)
		glfwSetWindowMonitor( this->_Window, NULL, 0, 0, this->_WindowWidth, this->_WindowHeight, 0 );
	else
		glfwSetWindowMonitor( this->_Window, this->_Monitor, 0, 0, this->_WindowWidth, this->_WindowHeight, 0 );
	this->_TextEngine.init("Assets/Fonts/neon_pixel.ttf", 30, this->_WindowWidth, this->_WindowHeight);
}
/********************************************************************************/
/*	FPS management functions													*/
/********************************************************************************/

void		Engine::FPSManager( void ){
	static int				index = 0;
	static double			frames[100]; // set samples
	double					averageTime = 0;
	static int				samplesFull = 0;
	static double			prevTime = 0;
	double					currentTime = 0;

	currentTime = glfwGetTime();
	this->_deltaTime = currentTime - prevTime;
	frames[index++] = this->_deltaTime;

	if (index == 100) {
		samplesFull = 1;
		index = 0;
	}

	for (int i = 0; i < 100; i++) {
		averageTime += frames[i];
	}

	if (samplesFull)
		averageTime /= 100;
	else
		averageTime /= index;

	std::cout << "Average FPS: " << 1.0f / averageTime << std::endl;
	prevTime = currentTime;

	// delay here
}

/********************************************************************************/
/*	Exception handling															*/
/********************************************************************************/

Engine::EngineErr::EngineErr( std::string errMsg ) {
	_errMsg = errMsg;
}

Engine::EngineErr::~EngineErr() throw() { return ;}

const char*Engine::EngineErr::what() const throw() { 
	return _errMsg.c_str();
}