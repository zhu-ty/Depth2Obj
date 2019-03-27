/**
@brief SKOpenGL.hpp
ShadowK's OpenGl Common Usage functions & class
@author zhu-ty
@date May 12, 2019
*/




#ifndef __SHADOWK_OPENGL__
#define __SHADOWK_OPENGL__

#include <cstdlib>
#include <string>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// include GLFW
#include <glfw3.h>

#include <opencv2/opencv.hpp>

#include "SKCommon.hpp"

#define PI 3.1415926536

namespace SKOpenGL {

	static class shader
	{
	public:
		/**
		@brief function to load glsl shaders
		include vertex shader and fragment shader
		@param std::string vertex_file_path: vertex shader file name
		@param std::string fragment_file_path: fragment shader file name
		@return GLuint: shader program ID
		*/
		static GLuint LoadShaders(std::string vertex_file_path, std::string fragment_file_path);
		/**
		@brief function to create glsl shaders
		include vertex shader and fragment shader
		@param std::string vertex: vertex shader
		@param std::string fragment: fragment shader
		@return GLuint: shader program ID
		*/
		static GLuint CreateShaders(std::string vertex, std::string fragment);
	private:
		static void StringReplace(std::string &strBase, std::string strSrc, std::string strDes);
		static GLuint CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader);
	};
	
	static class data
	{
	public:
		static int GenerateVAO(GLuint & VAO, GLuint LVBO, GLuint UVBO, unsigned int * ebo_data, int pt_count, GLuint &EBO, int lvboSize = 3, int uvboSize = 2);
		static int GenerateVAO(GLuint & VAO, GLuint LVBO, GLuint UVBO, int lvboSize = 3, int uvboSize = 2);
		static int Upload3DPoints(GLuint & LVBO, GLuint & UVBO, float * pt_loc, float * pt_uv, int pt_size, int lvboSize = 3, int uvboSize = 2);
		static int DeleteBuffer(GLuint VBO);
		static int DeleteTexure(GLuint TEX);
		static int DeleteArrays(GLuint VAO);

		//with BGR order
		static int LoadTexture(std::string file_path, GLuint &texture_id);
		static int LoadTexture(cv::Mat file, GLuint &texture_id);
		static int LoadTextureMask(std::string file_path, std::string mask_path, GLuint &texture_id);
		static int LoadTextureMask(cv::Mat file,cv::Mat mask, GLuint &texture_id);

		static bool Mat_CV2GLM(const cv::Mat& cvmat, glm::mat4* glmmat);
		static bool Mat_GLM2CV(const glm::mat4& glmmat, cv::Mat* cvmat);
	};

	class callback {
	public:
		double pressX;
		double pressY;
		double releaseX;
		double releaseY;
		double currentX;
		double currentY;
		GLfloat xoffset;
		GLfloat yoffset;
		float scroll_yoffset;
		bool key_pressed[1024] = { 0 };
		bool mouse_key_pressed[1024] = { 0 };
		bool firstMove = true;
		double scroll_acc_val = 0;
		double deltaTime;

		callback(){}
		callback(const callback& C)
		{
			pressX = C.pressX;
			pressY = C.pressY;
			releaseX = C.releaseX;
			releaseY = C.releaseY;
			currentX = C.currentX;
			currentY = C.currentY;
			xoffset = C.xoffset;
			yoffset = C.yoffset;
			scroll_yoffset = C.scroll_yoffset;
			memcpy(key_pressed, C.key_pressed, 1024 * sizeof(bool));
			memcpy(mouse_key_pressed, C.mouse_key_pressed, 1024 * sizeof(bool));
			firstMove = C.firstMove;
			scroll_acc_val = C.scroll_acc_val;
			deltaTime = C.deltaTime;
		}

		//clear some int
		void clear_some();
	};

	class window
	{
	public:
		enum class RenderMode {
			Window, FullScreen
		};
		class WindowSetting
		{
		public:
			int width;
			int height;
			RenderMode renderMode;
			bool enableMouse;
			WindowSetting() :
				width(1280), height(720),
				renderMode(RenderMode::Window),
				enableMouse(true){}
		};
		static int InitGlfw(WindowSetting setting, std::string name = "SKOpenGL Window");
		static GLFWwindow* GetWindowPtr() { return windowPtr; }
		static int Render(GLuint textureID, callback &ret);
	private:
		static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static double last_currentTime;
		static GLuint glWindowVAOID, glWindowVertexBufferID, glWindowUvBufferID, WindowShaderID;
		static glm::mat4 glWindowMVP;
		static bool IsGLFWInited;
		static WindowSetting _setting;
		static GLFWwindow* windowPtr;
		static callback control_param;
	};

	class camera {
	private:
		// Calculates the front vector from the Camera's (updated) Eular Angles
		void updateCameraVectors();
	public:
		//direction
		enum Camera_Movement {
			FORWARD = 1,
			BACKWARD = 2,
			LEFT = 3,
			RIGHT = 4,
			UP = 5,
			DOWN = 6
		};
		typedef cv::Rect2f SphericalRect2f;

		// Projection
		float camera_near = 0.1;
		float camera_far = 10.0*15000.0;
		// Float fov;
		float aspect = 16.0 / 9.0;
		// Static mode
		bool __static__matrix_mode = false;
		glm::mat4 __static__view;
		glm::mat4 __static__project;
		// Camera Attributes
		glm::vec3 Position, Front, Up, Right, WorldUp;
		// Eular Angles
		float Yaw;
		float Pitch;
		// Camera options
		float MovementSpeed = 1.0f;
		float MouseSensitivity = 0.1f;
		float Zoom = 45.0f;
		// Whether camera can move in 3 dimension
		//0 : pos move (false)
		//1 : direction move (true)
		//2 : zoom move (true)
		bool control_lock[3];

		// Constructor with vectors
		camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = -90.0f, float pitch = 0.0f);
		// Constructor with scalar values
		camera(float posX, float posY, float posZ,
			float upX, float upY, float upZ,
			float yaw, float pitch);

		// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix();

		// Returns the projection matrix by camera fov, aspect, camera_near, camera_far
		glm::mat4 GetProjectionMatrix();

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);

		// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset);

		void ZoomReset();
		// Get Fov Spherical Rect
		SphericalRect2f GetFovRect();
		~camera() {}

	private:
		/**
		@brief Calc a single 3d point to its sherical corrdiantes
		@param glm::vec3 &pt: a 3d point (x,y,z)
		@return glm::vec3: spherical coordinate point (r,¦È,¦Õ)
		^y  -z
		|  /
		| /
		|/
		0----->x
		¦È is the counterclockwise angle in the x-z plane measured in radians from the positive x-axis. The value of the angle is in the range [0 2*pi].
		¦Õ is the elevation angle in radians from the x-z plane, in the range [-1/2pi 1/2pi]
		*/
		static glm::vec3 calc_spherical_coordinates(glm::vec3 &pt);

	};

	//Render some images in a 3D-world framebuffer and use the result as a 2D texture
	class framebuffer
	{
	public:
		//Camera
		camera Camera;
		//GLFrameBufferID
		GLuint glFboID;
		//GLDepthBufferID
		GLuint glDboID;
		//GLTextureID(2D texture)
		GLuint glTextureID;

		int width;
		int height;

		/* ==================================
		Version 2 Area
		==================================*/

		/**
		@brief the order enum for layers
		usually the Top will be rendered first
		then is the First , Second, Third...
		finally is the Last.
		Whatever option will not be	guaranteed at specific order.
		*/
		enum class RenderOrder
		{
			Whatever = -1,
			Top = 0,
			First = 1, 
			Second = 2, 
			Third = 3, 
			Fourth = 4, 
			Fifth = 5, 
			Last = 9999
		};

		/**
		@brief the depth option for layers
		you may control the depth buffer rendering options, including:
		ReadOnly : for transparent layer rendering, readonly allowed layer not writing the depth buffer
		NonTest : the layer's triangles will all be drawn (its depth data will also be drawn if ReadOnly is not set)
		ClearAfter : (warning!) the depth buffer is cleared after rending this layer
		*/
		enum class DepthOption
		{
			ReadOnly = 0x1,
			NonTest = 0x2,
			ClearAfter = 0x4
		};

		/**
		@brief the type of the rendered data of a layer
		Element:(default) EBO is used, glDrawElements is called when rendering some triangles
		Array: EBO is not used, rendering VAO triangles in LVBO & UVBO
		*/
		enum class RenderDataType
		{
			Element = 0,
			Array = 1,
		};

		/**
		@brief choose whether enable glBlend when rendering this layer
		*/
		enum class BlendOption
		{
			Enable = 0,
			Disable = 1,
		};

		/**
		@brief some buffer id for a rendering target
		*/
		struct GLBufferID
		{
			GLuint VaoID = 0, EboID = 0, LVBO = 0, UVBO = 0;
		};

		/**
		@brief a layer is a mesh with its texture, color mat, VAO, transparent setting, etc...
		*/
		struct Layer
		{
			struct
			{
				cv::Mat colorMat;
				cv::cuda::GpuMat maskMatGpu;
				glm::mat4 origin_model;
				std::string cam_sn;
				struct
				{
					struct cudaGraphicsResource * lvbo_data;
					struct cudaGraphicsResource * uvbo_data;
					GLBufferID exGlBufs;
				}exLayerData;
			} exData; //Extra data for other use, these data will NOT be used by renderer
			GLBufferID glBufs; //VAO used to rendering, others only for buffer deleting
			GLuint programID;
			int triCount;
			glm::mat4 model;
			GLuint texID;
			glm::mat4 colorCorre;
			glm::vec4 colorAppen;
			RenderOrder order;
			DepthOption depOption;
			RenderDataType renDataType = RenderDataType::Element;
			BlendOption blOption = BlendOption::Enable;
		};

	public:
		/**
		@brief init textures which need to be rendered
		@param PRCamera glcam: A Pano Render Camera
		@param int width: texture width
		@param int height: texture height
		@return int(0)
		*/
		int init(camera glcam_ = camera(), int width_ = 1024, int height_ = 768);

		int clearBuffer(glm::vec4 clearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		/**
		@brief version 2.0 draw some layers of the given layer list
		the rendering order will be sort by RenderOrder
		@param std::vector<Layer> layers: data pack used to draw triangles
		@return int(0)
		*/
		int drawLayers(std::vector<Layer> layers);

		/**
		@brief draw some simple line in the 3D space with a VAO & point count
		@return int(0)
		*/
		int drawLine(GLuint program_id, GLuint VAO, int count);

		int getTextureCPU_RGB(cv::Mat &rgb);

		/**
		@brief use other's drawing function inside this framebuffer
		@return int(0)
		*/
		template<class T>
		int external_draw(T *object, void (T::*draw_funtion)(void));

		framebuffer() {};
		~framebuffer();
	private:
		bool inited = false;
	private:
		inline std::vector<RenderOrder> getOrderVec()
		{
			std::vector<RenderOrder> ret;
			ret.push_back(RenderOrder::Top);
			ret.push_back(RenderOrder::First);
			ret.push_back(RenderOrder::Second);
			ret.push_back(RenderOrder::Third);
			ret.push_back(RenderOrder::Fourth);
			ret.push_back(RenderOrder::Fifth);
			ret.push_back(RenderOrder::Whatever);
			ret.push_back(RenderOrder::Last);
			return ret;
		}
	};
	template<class T>
	inline int framebuffer::external_draw(T * object, void(T::* draw_funtion)(void))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, glFboID);
		glBindRenderbuffer(GL_RENDERBUFFER, glDboID);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glViewport(0, 0, width, height);
		(object->*draw_funtion)();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return 0;
	}
	
}
#endif //__SHADOWK_OPENGL__
