/**
@brief VideoRender.h
Generate 3D Object Video Frames data
@author zhu-ty
@date Oct 11, 2018
*/


#ifndef __VIDEO_RENDER_VIDEO_RENDER__
#define __VIDEO_RENDER_VIDEO_RENDER__

#include "SKCommon.hpp"
#include "SKOpenGL.hpp"
#include "ObjectGenerator.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>


class ObjectVideoRender
{
public:
	ObjectVideoRender() {};
	~ObjectVideoRender() {};

	int Init(cv::Size finalSize,double cameraK = 1e3,double E = 2.7e4, double previewRatio = 0.5);
	int RenderFrame(cv::Mat color, cv::Mat disparity, SKOpenGL::camera cam, SKOpenGL::callback &recall);
	int SaveImgs(std::string dir = "./VideoFrames");
	int SaveVideos() { SKCommon::warningOutput("Havent implemented"); };
	int GetNewestFrame(cv::Mat &ret);
private:
	double _cameraK, _stereoE;
	SKOpenGL::framebuffer _render_buffer;
	std::vector<cv::Mat> _buffered_frame;
	cv::Size _size;
	GLuint _shaderID;
};



#endif //!__VIDEO_RENDER_VIDEO_RENDER__