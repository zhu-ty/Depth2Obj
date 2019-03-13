/**
@brief SKEncoder, Interface of the NvidiaEncoder
@author zhu-ty
@date Dec 29, 2017
*/

#ifndef __H265_ENCODER_SKENCODER_H__
#define __H265_ENCODER_SKENCODER_H__

// include std
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <thread>
#include <memory>

// opencv
#include <opencv2/opencv.hpp>

// cuda
#ifdef _WIN32
#include <windows.h>
#endif
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>


/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message(VAR_NAME_VALUE(CUDART_VERSION))
#if CUDART_VERSION < 10000
#include <dynlink_nvcuvid.h>
#include <dynlink_cuviddec.h>
#endif
#include <device_launch_parameters.h>

#include "NvEncoder/NvEncoderCuda.h"
#include "Utils/Logger.h"
#include "Utils/NvEncoderCLIOptions.h"
#include "Utils/NvCodecUtils.h"

#include "SKCommon.hpp"

#define IMG_MAX_SIDE_LEN 8192

class SKEncoder
{
public:
	enum class FrameType
	{
		//YUV420 Planar data, full size Y planar with half size U & half size V (use vector to input these data)
		IYUV = 0,
		//8 bit Packed A8B8G8R8.This is a word - ordered format where a pixel is represented by a 32 - bit word with R
		//in the lowest 8 bits, G in the next 8 bits, B in the 8 bits after that and A in the highest 8 bits.
		//(use single gpu pointer to input data)
		ABGR = 1,
		//8 bit Packed A8R8G8B8.This is a word - ordered format where a pixel is represented by a 32 - bit word with B
		//in the lowest 8 bits, G in the next 8 bits, R in the 8 bits after that and A in the highest 8 bits.
		//(use single gpu pointer to input data)
		ARGB = 2
	};
public:
	SKEncoder() {};
	~SKEncoder() {};
	/**
	@brief init SKEncoder
	@param int frameNum: total video frame number of the given img list
	@param cv::Size imgSize: size of the img
	@param std::string fileName: output file name (default: output.h265)
	@param FrameType type: See SKEncoder::FrameType (default: IYUV)
	@param int GpuID: usage of which GPU (default: 0)
	@return int
	*/
	int init(int frameNum, cv::Size imgSize, std::string fileName = "output.h265", FrameType type = FrameType::IYUV, int GpuID = 0);

	/**
	@brief encode 3 planar data (YUV420 use)
	@param std::vector<void*> gpu_YUVdata3: YUV data list (gpu ptr)
	@param std::vector<uint32_t> steps3: gpu pointer step of each planar
	@return int
	*/
	int encode(std::vector<void*> gpu_YUVdata3, std::vector<uint32_t> steps3);

	/**
	@brief encode single planar data (ABGR & ARGB)
	@param void *gpu_PackedData: data (gpu ptr)
	@param uint32_t step: gpu pointer step
	@return int
	*/
	int encode(void *gpu_PackedData, uint32_t step);

	int endEncode();

private:
	int _frameNum, _GpuID;
	int _encodedFrameNum = 0;
	std::string _fileName;
	cv::Size _imgSize;
	FrameType _type;
	std::ofstream _fpOut;
	std::shared_ptr<NvEncoderCuda> _enc;
	//NvEncoderCuda _enc = NvEncoderCuda(NULL, 0, 0, NV_ENC_BUFFER_FORMAT_UNDEFINED);;
	NV_ENC_INITIALIZE_PARAMS _initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	NV_ENC_CONFIG _encodeConfig = { NV_ENC_CONFIG_VER };
	CUcontext _cuContext = NULL;
	NV_ENC_BUFFER_FORMAT _eFormat;
	NvEncoderInitParam _encodeCLIOptions;
	int _nFrameSize, _nFrame;
	int _stat_step;
	uint64_t _stat_last_time;
private:
	int printStatInfo();
};



#endif //__H265_ENCODER_SKENCODER_H__