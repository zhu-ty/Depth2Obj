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

std::vector<std::string> CollectFiles(std::string dir, std::vector<std::string> allowedExtensions)
{
	std::vector<std::string> ret;
	//std::vector<std::string> allowedExtensions = { ".jpg", ".png" ,".jpeg" };
	for (int i = 0; i < allowedExtensions.size(); i++) {
		std::vector<cv::String> imageNamesCurrentExtension;
		cv::glob(
			dir + "/*" + allowedExtensions[i],
			imageNamesCurrentExtension,
			true
		);
		ret.insert(
			ret.end(),
			imageNamesCurrentExtension.begin(),
			imageNamesCurrentExtension.end()
		);
	}
	return ret;
}


int main(int argc, char* argv[])
{
	INIReader reader("OVConfig.ini");
	
	std::string color_dir = reader.Get("All", "Color", "./color");
	std::string dis_dir = reader.Get("All", "Disparity", "./dis");
	std::string output_name = reader.Get("All", "Output", "Video.h265");

	std::vector<std::string> color_names, dis_names;
	color_names = CollectFiles(color_dir, std::vector<std::string>({ ".jpg", ".png" ,".jpeg" }));
	dis_names = CollectFiles(dis_dir, std::vector<std::string>({ ".tiff" }));

	if (dis_names.size() == 1)
	{
		color_names.resize(100, color_names[0]);
		dis_names.resize(100, dis_names[0]);
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
			cam.ProcessKeyboard(SKOpenGL::camera::FORWARD, 30);
		if ((((i / 7) % 4 + 1) / 2) % 2 == 0)
			cam.ProcessKeyboard(SKOpenGL::camera::RIGHT, 5.0f);
		else
			cam.ProcessKeyboard(SKOpenGL::camera::LEFT, 5.0f);
		SKCommon::infoOutput(SKCommon::format("%d", i));
	}
	//ovr.SaveImgs();
	ovr.SaveVideos(output_name);
	//system("pause");
	return 0;
}