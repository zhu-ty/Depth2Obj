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
		printf("Input Format:\nGenObjCpp input.txt\n");
		printf("input.txt:\n[num]\n[Color] [Depth] [PieceSize] [DepthSeg] [MinimumAreaHole] [MinimumAreaBlock] [DepthDiv]\n");
		printf("输入解释（中文）：\n输入一个文本文件，文件第一行为需要合并的depth/color 图像数量，此后每行：\n");
		printf("[纹理图]\n[深度图]（必须为16bit）\n[切分大小]（每几个点切一个三角形）\n");
		printf("[深度阈值]（超过阈值认为属于深度差异较大区域，记为hole，否则记为block）\n");
		printf("[最小hole面积]（超过面积的连续hole将不被渲染）\n");
		printf("[最小block面积]（小于面积的连续block将不被渲染）\n");
		printf("[深度变换]（实际深度将除以这个数作为渲染深度）\n");
		FILE *out = fopen("SampleInput.txt", "w");
		fprintf(out,"2\n");
		fprintf(out, "Icolor.jpg Idepth.jpg 1 5 3 5 1.0\n");
		fprintf(out, "Icolor2.jpg Idepth2.jpg 2 10.0 2 4 1.0\n");
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
		int p, mh, mb;
		double d, div;
		fscanf(in, "%s %s %d %lf %d %d %lf\n", color, depth, &p, &d, &mh, &mb, &div);
		OG.AddMesh(color, depth, p, d, mh, mb, div);
	}

	
	//OG.AddMesh(argv[1], argv[2], PieceSize, DepthSeg, MinimumArea);
	//OG.OutputSingleObj(".", 0);
	OG.OutputMixedObj(".");
	return 0;
}