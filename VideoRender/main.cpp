#include <iostream>
#include <vector>
#include "SysUtil.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "ObjectGenerator.h"
// include GLEW
#include <GL/glew.h>
// include GLFW
#include <glfw3.h>
// include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SKOpenGL.hpp"
#include "VideoRender.h"

int main(int argc, char* argv[])
{
	ObjectVideoRender ovr;
	ovr.Init(cv::Size(1920, 1080));
	
	SKOpenGL::camera cam;
	SKOpenGL::callback recall;
	
	cv::Mat c, dis;
	c = cv::imread("./color.png");
	dis = cv::imread("./disparity.tiff", cv::IMREAD_UNCHANGED);

	for (int i = 0; i < 70; i++)
	{
		ovr.RenderFrame(c, dis, cam, recall);
		cam.ProcessMouseMovement(-2, 0);
		cam.ProcessKeyboard(SKOpenGL::camera::FORWARD, 35);
		SKCommon::infoOutput(SKCommon::format("%d", i));
	}
	ovr.SaveImgs();
	system("pause");

	return 0;
}