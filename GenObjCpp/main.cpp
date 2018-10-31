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
	std::string argv1("input.txt");

	if(argc < 2)
	{
		printf("Input Format:\nGenObjCpp input.txt");
		printf("input.txt:\n[num]\n[Color] [Depth] [PieceSize] [DepthSeg] [MinimumArea] [DepthDiv]");
		FILE *out = fopen("SampleInput.txt", "w");
		fprintf(out,"2\n");
		fprintf(out, "Icolor.jpg Idepth.jpg 1 5 3 1.0");
		fprintf(out, "Icolor2.jpg Idepth2.jpg 2 10.0 2 1.0");
		//return 0;
	}
	else
	{
		argv1 = argv[1];
	}
	FILE *in = fopen(argv1.c_str(), "r");
	int count;
	fscanf(in, "%d\n", &count);
	ObjectGenerator OG;
	for (int i = 0; i < count; i++)
	{
		char color[1000] = {0}, depth[1000] = { 0 };
		int p, m;
		double d, div;
		fscanf(in, "%s %s %d %lf %d %lf\n", color, depth, &p, &d, &m, &div);
		OG.AddMesh(color, depth, p, d, m, div);
	}

	
	//OG.AddMesh(argv[1], argv[2], PieceSize, DepthSeg, MinimumArea);
	//OG.OutputSingleObj(".", 0);
	OG.OutputMixedObj(".");
	return 0;
}