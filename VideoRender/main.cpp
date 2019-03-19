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

	int count = reader.GetInteger("All", "Count", 1);
	std::string output_name = reader.Get("All", "Output", "Video.h265");

	std::vector<std::vector<std::string>> colorName(count), disName(count);
	int mins = 999999;

	for (int i = 0; i < count; i++)
	{
		std::string cDir = reader.Get(SKCommon::format("Pair_%d", i), "Color", "./color");
		std::string dDir = reader.Get(SKCommon::format("Pair_%d", i), "Disparity", "./color");
		colorName[i] = CollectFiles(cDir, std::vector<std::string>({ ".jpg", ".png" ,".jpeg" }));
		disName[i] = CollectFiles(dDir, std::vector<std::string>({ ".tiff" }));
		if (colorName[i].size() != 1 && colorName[i].size() < mins)
			mins = colorName[i].size();
		if (disName[i].size() != 1 && disName[i].size() < mins)
			mins = disName[i].size();
	}
	mins = (mins == 999999) ? 100 : mins;

	for (int i = 0; i < count; i++)
	{
		if (colorName[i].size() == 1)
			colorName[i].resize(mins, colorName[i][0]);
		if (disName[i].size() == 1)
			disName[i].resize(mins, disName[i][0]);
	}

	ObjectVideoRender ovr;
	ovr.Init(cv::Size(1920, 1080));
	
	SKOpenGL::camera cam;
	SKOpenGL::callback recall;


	for (int i = 0; i < mins; i++)
	{
		std::vector<cv::Mat> cs(count), diss(count);
		for (int j = 0; j < count; j++)
		{
			cs[j] = cv::imread(colorName[j][i]);
			diss[j] = cv::imread(disName[j][i], cv::IMREAD_UNCHANGED);
		}

		ovr.RenderFrame(cs, diss, cam, recall);
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