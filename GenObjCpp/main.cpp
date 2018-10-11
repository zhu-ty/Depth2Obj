#include <iostream>
#include <vector>

#include "SysUtil.hpp"
#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "ObjectGenerator.h"


/* index : col then row
0--------->x
| 0   h+0
| 1   h+1
| 2   h+2
| 3   h+3
| ..  ..
vy
*/



int main(int argc, char* argv[])
{
	//if (argc != 3 && argc != 6)
	//{
	//	printf("Input Format:\nGenObkCpp Icolor Idepth [PieceSize=2 DepthSeg=10.0 MinimumArea=2]\n");
	//	printf("Input Sample 1:\nGenObjCpp Icolor.jpg Idepth.jpg 1 5 3\n");
	//	printf("Input Sample 2:\nGenObjCpp Icolor.jpg Idepth.jpg\n");
	//	return 0;
	//}

	//int PieceSize = 2, MinimumArea = 2;
	//float DepthSeg = 10.0f;
	//if (argc == 6)
	//{
	//	PieceSize = std::atoi(argv[3]);
	//	DepthSeg = std::atof(argv[4]);
	//	MinimumArea = std::atoi(argv[5]);
	//}

	if(argc < 2)
	{
		printf("Input Format:\nGenObjCpp input.txt");
		FILE *out = fopen("SampleInput.txt", "w");
		fprintf(out,"2\n");
		fprintf(out, "Icolor.jpg Idepth.jpg 1 5 3");
		fprintf(out, "Icolor2.jpg Idepth2.jpg 2 10.0 2");
		return 0;
	}

	FILE *in = fopen(argv[1], "r");
	int count;
	fscanf(in, "%d\n", &count);
	ObjectGenerator OG;
	for (int i = 0; i < count; i++)
	{
		char color[1000] = {0}, depth[1000] = { 0 };
		int p, m;
		double d;
		fscanf(in, "%s %s %d %lf %d\n", color, depth, &p, &d, &m);
		OG.AddMesh(color, depth, p, d, m);
	}

	
	//OG.AddMesh(argv[1], argv[2], PieceSize, DepthSeg, MinimumArea);
	//OG.OutputSingleObj(".", 0);
	OG.OutputMixedObj(".");
	return 0;
}