
#include "SKOpenGL.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

GLuint SKOpenGL::shader::LoadShaders(std::string vertex_file_path, std::string fragment_file_path){

#ifdef _WIN32
	StringReplace(vertex_file_path, "/", "\\");
	StringReplace(fragment_file_path, "/", "\\");
#endif
	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}
	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
	if(FragmentShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fragment_file_path);
		getchar();
		return 0;
	}
	return CreateShaders(VertexShaderCode, FragmentShaderCode);
}

GLuint SKOpenGL::shader::CreateShaders(std::string vertex, std::string fragment)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	//printf("Compiling shader : %s\n", vertex_file_path.c_str());
	char const * VertexSourcePointer = vertex.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	//printf("Compiling shader : %s\n", fragment_file_path.c_str());
	char const * FragmentSourcePointer = fragment.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	// printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void SKOpenGL::shader::StringReplace(std::string & strBase, std::string strSrc, std::string strDes)
{
	std::string::size_type pos = 0;
	std::string::size_type srcLen = strSrc.size();
	std::string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != std::string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
}

GLuint SKOpenGL::shader::CompileGLShader(const char * pchShaderName, const char * pchVertexShader, const char * pchFragmentShader)
{
	GLuint unProgramID = glCreateProgram();

	GLuint nSceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(nSceneVertexShader, 1, &pchVertexShader, NULL);
	glCompileShader(nSceneVertexShader);

	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(nSceneVertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile vertex shader %d!\n", pchShaderName, nSceneVertexShader);
		glDeleteProgram(unProgramID);
		glDeleteShader(nSceneVertexShader);
		return 0;
	}
	glAttachShader(unProgramID, nSceneVertexShader);
	glDeleteShader(nSceneVertexShader); // the program hangs onto this once it's attached

	GLuint  nSceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(nSceneFragmentShader, 1, &pchFragmentShader, NULL);
	glCompileShader(nSceneFragmentShader);

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(nSceneFragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		printf("%s - Unable to compile fragment shader %d!\n", pchShaderName, nSceneFragmentShader);
		glDeleteProgram(unProgramID);
		glDeleteShader(nSceneFragmentShader);
		return 0;
	}

	glAttachShader(unProgramID, nSceneFragmentShader);
	glDeleteShader(nSceneFragmentShader); // the program hangs onto this once it's attached

	glLinkProgram(unProgramID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("%s - Error linking program %d!\n", pchShaderName, unProgramID);
		glDeleteProgram(unProgramID);
		return 0;
	}

	glUseProgram(unProgramID);
	glUseProgram(0);

	return unProgramID;
}

int SKOpenGL::data::GenerateVAO(GLuint & VAO, GLuint LVBO, GLuint UVBO, unsigned int * ebo_data, int pt_count, GLuint & EBO, int lvboSize, int uvboSize)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pt_count * sizeof(unsigned int), ebo_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glVertexAttribPointer(0, lvboSize, GL_FLOAT, GL_FALSE, lvboSize * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glVertexAttribPointer(1, uvboSize, GL_FLOAT, GL_FALSE, uvboSize * sizeof(GLfloat), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return 0;
}

int SKOpenGL::data::GenerateVAO(GLuint & VAO, GLuint LVBO, GLuint UVBO, int lvboSize, int uvboSize)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glVertexAttribPointer(0, lvboSize, GL_FLOAT, GL_FALSE, lvboSize * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glVertexAttribPointer(1, uvboSize, GL_FLOAT, GL_FALSE, uvboSize * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 0;
}

int SKOpenGL::data::Upload3DPoints(GLuint & LVBO, GLuint & UVBO, float * pt_loc, float * pt_uv, int pt_size, int lvboSize, int uvboSize)
{
	glGenBuffers(1, &LVBO);
	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glBufferData(GL_ARRAY_BUFFER, pt_size * lvboSize * sizeof(float), pt_loc, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, pt_size * uvboSize * sizeof(float), pt_uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return 0;
}

int SKOpenGL::data::DeleteBuffer(GLuint VBO)
{
	glDeleteBuffers(1, &VBO);
	return 0;
}

int SKOpenGL::data::DeleteTexure(GLuint TEX)
{
	glDeleteTextures(1, &TEX);
	return 0;
}

int SKOpenGL::data::DeleteArrays(GLuint VAO)
{
	glDeleteVertexArrays(1, &VAO);
	return 0;
}

int SKOpenGL::data::LoadTexture(std::string file_path, GLuint &texture_id)
{
	cv::Mat img = cv::imread(file_path);
	return LoadTexture(img, texture_id);
}

int SKOpenGL::data::LoadTexture(cv::Mat file, GLuint & texture_id)
{
	if (file.channels() != 3 || ((file.type() & CV_MAT_DEPTH_MASK) != CV_8U))
	{
		SKCommon::errorOutput(DEBUG_STRING + "Only 3 channel uchar BGR image is now supported.");
		return -1;
	}
	cv::Mat img;
	cv::cvtColor(file, img, cv::COLOR_BGR2BGRA); //To make sure we upload data with 4x
	// More info : https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
	//cv::resize(img, img, cv::Size(img.cols / 4 * 4, img.rows / 4 * 4));

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// everything that is out of texture will be set to alpha = 0.0f
	float color_[] = { 0.0f,0.0f,0.0f,0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//int w, h;
	//int miplevel = 0;
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);

	glBindTexture(GL_TEXTURE_2D, 0);
	return 0;
}

int SKOpenGL::data::LoadTextureMask(std::string file_path, std::string mask_path, GLuint &texture_id)
{
	if (mask_path == "" || !SKCommon::existFile(mask_path))
	{
		SKCommon::warningOutput("LoadTextureMask mask file do not exist, ignored.");
		LoadTexture(file_path, texture_id);
		return 1;
	}
	cv::Mat img = cv::imread(file_path, cv::IMREAD_UNCHANGED);
	cv::Mat mask = cv::imread(mask_path, cv::IMREAD_UNCHANGED);
	return LoadTextureMask(img, mask, texture_id);
}

int SKOpenGL::data::LoadTextureMask(cv::Mat file, cv::Mat mask, GLuint & texture_id)
{
	cv::Mat img = file;
	if (img.channels() != 3 || mask.channels() != 1 || ((img.type() & CV_MAT_DEPTH_MASK) != CV_8U) || ((mask.type() & CV_MAT_DEPTH_MASK) != CV_8U))
	{
		SKCommon::errorOutput(DEBUG_STRING + "Only 3 channel uchar texture & 1 channel uchar mask is now supported.");
		return -1;
	}
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// everything that is out of texture will be set to alpha = 0.0f
	float color_[] = { 0.0f,0.0f,0.0f,0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char * img_full_data = new unsigned char[img.cols * img.rows * 4];
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			memcpy(img_full_data + i * img.cols * 4 + j * 4, img.data + i * img.cols * 3 + j * 3, 3 * sizeof(unsigned char));
			if (i < mask.rows && j < mask.cols)
				img_full_data[i * img.cols * 4 + j * 4 + 3] = mask.data[i * mask.cols + j];
			else
				img_full_data[i * img.cols * 4 + j * 4 + 3] = 255;
		}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.cols, img.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, img_full_data);
	delete[] img_full_data;
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return 0;
}

bool SKOpenGL::data::Mat_CV2GLM(const cv::Mat & cvmat, glm::mat4 * glmmat)
{
	if (cvmat.cols != 4 || cvmat.rows != 4 || cvmat.type() != CV_32FC1) {
		SKCommon::errorOutput("PanoDataGenerator::Mat_CV2GLM Matrix conversion error!");
		return false;
	}
	memcpy(glm::value_ptr(*glmmat), cvmat.data, 16 * sizeof(float));
	*glmmat = glm::transpose(*glmmat);
	return true;
}

bool SKOpenGL::data::Mat_GLM2CV(const glm::mat4 & glmmat, cv::Mat * cvmat)
{
	if (cvmat->cols != 4 || cvmat->rows != 4) {
		(*cvmat) = cv::Mat(4, 4, CV_32F);
	}
	memcpy(cvmat->data, glm::value_ptr(glmmat), 16 * sizeof(float));
	*cvmat = cvmat->t();
	return true;
}

glm::vec3 SKOpenGL::data::getRandomColor()
{
	glm::vec3 ret;
	ret.r = b() / 1000000.0f;
	ret.g = b() / 1000000.0f;
	ret.b = b() / 1000000.0f;
	return ret;
}
std::random_device SKOpenGL::data::rd;
std::default_random_engine SKOpenGL::data::g = std::default_random_engine(rd());
std::_Binder<std::_Unforced, std::uniform_int_distribution<int>, std::default_random_engine&> SKOpenGL::data::b = 
	std::bind(std::uniform_int_distribution<int>(0, 1000000), g);

inline void SKOpenGL::callback::clear_some()
{
	//pressX = 0;
	//pressY = 0;
	//releaseX = 0;
	//releaseY = 0;
	//currentX = 0;
	//currentY = 0;

	xoffset = 0;
	yoffset = 0;
	scroll_yoffset = 0;
	deltaTime = 0;
}

// window static member
SKOpenGL::callback SKOpenGL::window::control_param; 
GLFWwindow* SKOpenGL::window::windowPtr;
SKOpenGL::window::WindowSetting SKOpenGL::window::_setting;
GLuint SKOpenGL::window::glWindowVAOID, SKOpenGL::window::glWindowVertexBufferID,
	   SKOpenGL::window::glWindowUvBufferID, SKOpenGL::window::WindowShaderID;
glm::mat4 SKOpenGL::window::glWindowMVP;
bool SKOpenGL::window::IsGLFWInited = false;
double SKOpenGL::window::last_currentTime = 0;

int SKOpenGL::window::InitGlfw(WindowSetting setting, std::string name)
{
	_setting = setting;
	if (!glfwInit()) {
		SKCommon::errorOutput(DEBUG_STRING + "Failed to initialize GLFW");
		return -1;
	}
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int monitor_count;
	cv::Size WindowSize, ScreenSize;
	auto monitors = glfwGetMonitors(&monitor_count);
	GLFWmonitor* special_monitor = glfwGetPrimaryMonitor();
	if (_setting.renderMode == RenderMode::FullScreen)
	{
		for (int i = 0; i < monitor_count; i++)
		{
			const GLFWvidmode * _mode = glfwGetVideoMode(monitors[i]);
			if (_mode->width == _setting.width)
			{
				special_monitor = monitors[i];
				break;
			}
		}
		const GLFWvidmode * _mode = glfwGetVideoMode(special_monitor);
		_setting.width = _mode->width;
		_setting.height = _mode->height;
	}
	WindowSize = cv::Size(_setting.width, _setting.height);

	// get screen resolution
	const GLFWvidmode * mode = glfwGetVideoMode(special_monitor);
	ScreenSize.width = mode->width;
	ScreenSize.height = mode->height;
	SKCommon::infoOutput(SKCommon::format("Screen resolution: <%d, %d>, window resolution: <%d, %d>, render mode: %s\n",
		ScreenSize.width, ScreenSize.height, WindowSize.width, WindowSize.height,
		_setting.renderMode == RenderMode::Window ? "Window" : "Full Screen"));
	// Open a window and create its OpenGL context
	if (_setting.renderMode == RenderMode::Window)
		windowPtr = glfwCreateWindow(WindowSize.width, WindowSize.height, name.c_str(), NULL, NULL);
	else windowPtr = glfwCreateWindow(ScreenSize.width, ScreenSize.height, name.c_str(), special_monitor, NULL);
	if (windowPtr == nullptr)
	{
		SKCommon::errorOutput(DEBUG_STRING + " Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.");
		glfwTerminate();
		return -2;
	}
	glfwMakeContextCurrent(windowPtr);

	if(setting.enableMouse == false)
		glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//// set control callback function
	glfwSetMouseButtonCallback(windowPtr, mouse_button_callback);
	glfwSetScrollCallback(windowPtr, mouse_scroll_callback);
	glfwSetCursorPosCallback(windowPtr, mouse_cursor_callback);
	glfwSetKeyCallback(windowPtr, keyboard_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		SKCommon::errorOutput(DEBUG_STRING + " Failed to initialize GLEW.");
		return -3;
	}

	static const GLfloat g_quad_vertex_buffer_data[] = {
		0.0f, 0.0f, 0.0f,
		WindowSize.width, 0.0f, 0.0f,
		WindowSize.width, WindowSize.height, 0.0f,
		WindowSize.width, WindowSize.height, 0.0f,
		0.0f, WindowSize.height, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	// The fullscreen quad's FBO
	static const GLfloat g_quad_uv_buffer_data[] = {
		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f
	};
	// bind
	glGenVertexArrays(1, &glWindowVAOID);
	glBindVertexArray(glWindowVAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &glWindowVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, glWindowVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &glWindowUvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, glWindowUvBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_uv_buffer_data), g_quad_uv_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 View = glm::lookAt(
		glm::vec3(WindowSize.width / 2, WindowSize.height / 2, WindowSize.height / 2), // camera position
		glm::vec3(WindowSize.width / 2, WindowSize.height / 2, 0), // look at position
		glm::vec3(0, 1, 0) // head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0);
	glm::mat4 Projection2 = 
		glm::perspective(glm::radians(90.0f),
		(float)WindowSize.width / (float)WindowSize.height,
		0.1f, static_cast<float>(WindowSize.height));
	glWindowMVP = Projection2 * View * Model;

	std::string verShader = 
"\
#version 450 core \n\
layout(location = 0) in vec3 vertexPosition;\n\
layout(location = 1) in vec2 vertexUV;\n\
out vec2 UV;\n\
uniform mat4 MVP;\n\
void main() {\n\
	gl_Position = MVP * vec4(vertexPosition, 1);\n\
	UV = vertexUV;\n\
}\n\
";
	std::string fraShader =
"\
#version 450 core\n\
in vec2 UV;\n\
out vec4 colorRGBA;\n\
uniform sampler2D myTextureSampler;\n\
void main() {\n\
	colorRGBA = texture(myTextureSampler, UV).rgba;\n\
}\n\
";
	WindowShaderID = shader::CreateShaders(verShader, fraShader);

	IsGLFWInited = true;
	return 0;
}

int SKOpenGL::window::Render(GLuint textureID, callback &ret)
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// RGBA texture blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	double currentTime = glfwGetTime();
	double deltaTime = currentTime - last_currentTime;
	last_currentTime = currentTime;

	GLuint MatrixID = glGetUniformLocation(WindowShaderID, "MVP");
	GLuint TextureID = glGetUniformLocation(WindowShaderID, "myTextureSampler");
	// Render to the screen
	glViewport(0, 0, _setting.width, _setting.height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// Clear the screen
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(WindowShaderID);
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &glWindowMVP[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(TextureID, 0);
	glBindVertexArray(glWindowVAOID);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3); 

	glfwSwapBuffers(windowPtr);
	control_param.clear_some();
	glfwPollEvents();
	ret = control_param;
	ret.deltaTime = deltaTime;
	return 0;
}

void SKOpenGL::window::mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {

	if (control_param.firstMove) // 这个bool变量一开始是设定为true的
	{
		control_param.currentX = xpos;
		control_param.currentY = ypos;
		control_param.firstMove = false;
	}

	control_param.xoffset = xpos - control_param.currentX;
	control_param.yoffset = control_param.currentY - ypos; // 注意这里是相反的，因为y坐标的范围是从下往上的
	control_param.currentX = xpos;
	control_param.currentY = ypos;

}

void SKOpenGL::window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		control_param.mouse_key_pressed[button] = true;
	else if (action == GLFW_RELEASE)
		control_param.mouse_key_pressed[button] = false;
	return;
}

void SKOpenGL::window::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
	control_param.scroll_yoffset = yoffset;
}

void SKOpenGL::window::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		control_param.key_pressed[key] = true;
	else if (action == GLFW_RELEASE)
		control_param.key_pressed[key] = false;
}

SKOpenGL::camera::camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	control_lock[0] = false;
	control_lock[1] = false;
	control_lock[2] = false;
	updateCameraVectors();
}

SKOpenGL::camera::camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	control_lock[0] = false;
	control_lock[1] = false;
	control_lock[2] = false;
	updateCameraVectors();
}

glm::mat4 SKOpenGL::camera::GetViewMatrix()
{
	if (__static__matrix_mode == false)
		return glm::lookAt(Position, Position + Front, Up);
	else
		return __static__view;
}

glm::mat4 SKOpenGL::camera::GetProjectionMatrix()
{
	if (__static__matrix_mode == false)
		return glm::perspective(glm::radians(Zoom), aspect,
			camera_near, camera_far);
	else
		return __static__project;
}

void SKOpenGL::camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	if (control_lock[0] == false)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position += WorldUp * velocity;
		if (direction == DOWN)
			Position -= WorldUp * velocity;
	}
}

void SKOpenGL::camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (control_lock[1] == false)
	{
		xoffset *= MouseSensitivity * (Zoom / 45.0f);
		yoffset *= MouseSensitivity * (Zoom / 45.0f);

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		updateCameraVectors();
	}
}

void SKOpenGL::camera::ProcessMouseScroll(float yoffset)
{
	if (control_lock[2] == false)
	{
		const float max_zoom = 120.0f;
		const float min_zoom = 0.1f;
		if (Zoom >= min_zoom && Zoom <= max_zoom)
			Zoom -= yoffset * ((Zoom - min_zoom) / (max_zoom - min_zoom)) * 2;
		if (Zoom <= min_zoom)
			Zoom = min_zoom;
		if (Zoom >= max_zoom)
			Zoom = max_zoom;
	}
}

void SKOpenGL::camera::ZoomReset()
{
	if (control_lock[2] == false)
	{
		Zoom = 45.0f;
	}
}

SKOpenGL::camera::SphericalRect2f SKOpenGL::camera::GetFovRect()
{
	SphericalRect2f ret;
	glm::vec3 look_at_sp = calc_spherical_coordinates(Front);
	ret.height = glm::radians(Zoom);
	ret.width = ret.height * aspect;
	ret.x = (2 * PI - look_at_sp.y) - ret.width / 2.0f + (((2 * PI - look_at_sp.y) - ret.width / 2.0f < 0) ? 2 * PI : 0.0f);
	ret.y = -look_at_sp.z - ret.height / 2.0f;
	return ret;
}

void SKOpenGL::camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, 
	// because their length gets closer to 0 the more you look up or down which results in slower movement.
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

glm::vec3 SKOpenGL::camera::calc_spherical_coordinates(glm::vec3 & pt)
{
	glm::vec3 ret;
	float x = pt.x;
	float y = pt.y;
	float z = pt.z;
	ret.x = sqrt(x*x + y*y + z*z);
	if (ret.x <= 1e-6)
	{
		SKCommon::warningOutput("PanoDataGenerator::calc_spherical_coordinates r too small!");
		ret.y = 0.0f;
		ret.z = 0.0f;
	}
	else
	{
		ret.z = (float)asin(y / ret.x);
		float len_xz = ret.x * (float)cos(ret.z);
		// found a problem here! this range is not [0 2pi)
		// (ERROR version )ret.y = (float)acos(x / len_xz);
		if (z > 0.f)
			ret.y = 2.0f * PI - (float)acos(x / len_xz);
		else
			ret.y = (float)acos(x / len_xz);
	}
	return ret;
}

int SKOpenGL::framebuffer::init(camera glcam_, int width_, int height_)
{
	width = width_;
	height = height_;
	Camera = glcam_;

	// generate FBO and bind texture
	// the framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &glFboID);
	glBindFramebuffer(GL_FRAMEBUFFER, glFboID);

	glGenRenderbuffers(1, &glDboID);
	glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glDboID);

	// generate texture
	glGenTextures(1, &glTextureID);
	glBindTexture(GL_TEXTURE_2D, glTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glTextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		SKCommon::errorOutput(DEBUG_STRING + "Framebuffer is not complete!");
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	inited = true;
	return 0;
}

int SKOpenGL::framebuffer::clearBuffer(glm::vec4 clearColor)
{
	glBindFramebuffer(GL_FRAMEBUFFER, glFboID);
	glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return 0;
}

int SKOpenGL::framebuffer::drawLayers(std::vector<Layer> layers)
{
	glBindFramebuffer(GL_FRAMEBUFFER, glFboID);
	glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);

	//drawing
	auto orderVec = getOrderVec();
	for (int i = 0; i < orderVec.size(); i++)
	{
		for (int j = 0; j < layers.size(); j++)
		{
			if (layers[j].order != orderVec[i] || layers[j].glBufs.VaoID == 0)
				continue;
			glUseProgram(layers[j].programID);
			GLuint MatrixLoc = glGetUniformLocation(layers[j].programID, "MVP");
			GLuint TextureLoc = glGetUniformLocation(layers[j].programID, "myTextureSampler");
			GLuint ColorCorrectionID = glGetUniformLocation(layers[j].programID, "colorCorrection");
			GLuint ColorAppendID = glGetUniformLocation(layers[j].programID, "colorAppend");
			glUniformMatrix4fv(ColorCorrectionID, 1, GL_FALSE, &((glm::mat4()[0][0])));
			glUniform4fv(ColorAppendID, 1, &((glm::vec4()[0])));
			if (static_cast<int>(layers[j].depOption) & static_cast<int>(DepthOption::ReadOnly))
				glDepthMask(GL_FALSE);
			else
				glDepthMask(GL_TRUE);
			if (static_cast<int>(layers[j].depOption) & static_cast<int>(DepthOption::NonTest))
				glDepthFunc(GL_ALWAYS);
			else
				glDepthFunc(GL_LEQUAL);

			if (layers[j].blOption == BlendOption::Enable)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//glEnable(GL_BLEND);
			else
				glBlendFunc(GL_ONE, GL_ZERO);//glDisable(GL_BLEND);

			glm::mat4 MVP = Camera.GetProjectionMatrix() * Camera.GetViewMatrix() * layers[j].model;
			glUniformMatrix4fv(MatrixLoc, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ColorCorrectionID, 1, GL_FALSE, &(layers[j].colorCorre[0][0]));
			glUniform4fv(ColorAppendID, 1, &(layers[j].colorAppen[0]));
			glBindVertexArray(layers[j].glBufs.VaoID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, layers[j].texID);
			glUniform1i(TextureLoc, 0);
			if (layers[j].renDataType == RenderDataType::Element)
				glDrawElements(GL_TRIANGLES, layers[j].triCount * 3, GL_UNSIGNED_INT, 0);
			else if (layers[j].renDataType == RenderDataType::Array)
				glDrawArrays(GL_TRIANGLES, 0, layers[j].triCount * 3);

			glBindVertexArray(0);

			if (static_cast<int>(layers[j].depOption) & static_cast<int>(DepthOption::ClearAfter))
			{
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			glUniformMatrix4fv(ColorCorrectionID, 1, GL_FALSE, &((glm::mat4()[0][0])));
			glUniform4fv(ColorAppendID, 1, &((glm::vec4()[0])));
		}
	}

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return 0;
}

int SKOpenGL::framebuffer::drawLine(GLuint program_id, GLuint VAO, int count)
{
	glBindFramebuffer(GL_FRAMEBUFFER, glFboID);
	glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glUseProgram(program_id);
	glBindVertexArray(VAO);
	glLineWidth(5.0f);
	glDrawArrays(GL_LINES, 0, count);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return 0;
}

int SKOpenGL::framebuffer::getTextureCPU_RGB(cv::Mat & rgb)
{
	glBindFramebuffer(GL_FRAMEBUFFER, glFboID);
	glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
	char* pixels = new char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels); // Remeber that this is fliped img
	cv::Mat img(cv::Size(width, height), CV_8UC3, pixels);
	//rgb = img.clone();
	cv::flip(img, rgb, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return 0;
}

SKOpenGL::framebuffer::~framebuffer()
{
	if (inited)
	{
		glDeleteFramebuffers(1, &glFboID);
		glDeleteTextures(1, &glTextureID);
	}
}

