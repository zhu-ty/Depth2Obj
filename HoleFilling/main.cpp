#include<iostream>
#include<vector>

#include"SysUtil.hpp"
#include"INIReader.h"
#include<opencv2\opencv.hpp>



int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		SysUtil::errorOutput("Input Sample: img_file.png mask.png output.png");
		system("pause");
		return 0;
	}
	std::string img_p(argv[1]), mask_p(argv[2]), out_p;

	if (argc < 4)
		out_p = "output.png";
	else
	{
		out_p = argv[3];
	}

	cv::Mat I,IF, R,Rgray,OPT,OPT_FLOAT;
	I = cv::imread(img_p, cv::IMREAD_UNCHANGED);

	R = cv::imread(mask_p, cv::IMREAD_UNCHANGED);
	cv::inpaint(I, R, OPT, 5, cv::INPAINT_NS);

	//I.convertTo(IF, CV_32FC1);
	//cv::log(IF, IF);
	//OPT.convertTo(OPT_FLOAT, CV_32FC1);
	//cv::log(OPT_FLOAT, OPT_FLOAT);
	//cv::blur(OPT, OPT, cv::Size(11, 11));

	cv::imwrite(out_p, OPT);
	//system("pause");
	return 0;
}