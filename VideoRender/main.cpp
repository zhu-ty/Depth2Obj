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
#include "INIReader.h"

int main(int argc, char* argv[])
{
	INIReader reader("OVConfig.ini");
	
	std::string color_dir = reader.Get("All", "Color", "./color");
	std::string dis_dir = reader.Get("All", "Disparity", "./dis");

	std::vector<std::string> color_names, dis_names;
	{
		std::vector<std::string> allowedExtensions = { ".jpg", ".png" ,".jpeg" };
		for (int i = 0; i < allowedExtensions.size(); i++) {
			std::vector<cv::String> imageNamesCurrentExtension;
			cv::glob(
				color_dir + "/*" + allowedExtensions[i],
				imageNamesCurrentExtension,
				true
			);
			color_names.insert(
				color_names.end(),
				imageNamesCurrentExtension.begin(),
				imageNamesCurrentExtension.end()
			);
		}
	}
	{
		std::vector<std::string> allowedExtensions = { ".tiff" };
		for (int i = 0; i < allowedExtensions.size(); i++) {
			std::vector<cv::String> imageNamesCurrentExtension;
			cv::glob(
				dis_dir + "/*" + allowedExtensions[i],
				imageNamesCurrentExtension,
				true
			);
			dis_names.insert(
				dis_names.end(),
				imageNamesCurrentExtension.begin(),
				imageNamesCurrentExtension.end()
			);
		}
	}

	ObjectVideoRender ovr;
	ovr.Init(cv::Size(1920, 1080));
	
	SKOpenGL::camera cam;
	SKOpenGL::callback recall;
	
	cv::Mat c, dis;
	//c = cv::imread("./color.png");
	//dis = cv::imread("./disparity.tiff", cv::IMREAD_UNCHANGED);

	for (int i = 0; i < dis_names.size(); i++)
	{
		c = cv::imread(color_names[i]);
		dis = cv::imread(dis_names[i], cv::IMREAD_UNCHANGED);
		ovr.RenderFrame(c, dis, cam, recall);
		//cam.ProcessMouseMovement(-2, 0);
		if(i < 4)
			cam.ProcessKeyboard(SKOpenGL::camera::FORWARD, 35);
		if ((i / 10) % 2 == 0)
			cam.ProcessKeyboard(SKOpenGL::camera::LEFT, 3.5f);
		else
			cam.ProcessKeyboard(SKOpenGL::camera::RIGHT, 3.5f);
		SKCommon::infoOutput(SKCommon::format("%d", i));
	}
	//ovr.SaveImgs();
	ovr.SaveVideos();
	//system("pause");

	return 0;
}