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
	
	if (argc < 2 && !SysUtil::existFile("./input.txt"))
	{
		printf("Input Format:\nGenObjCpp input.txt\n");
		printf("input.txt:\n[num]\n[Color] [Depth] [PieceSize] [DepthSegCos] [MinimumAreaHole] [MinimumAreaBlock] [DepthDiv]\n");
		printf("������ͣ����ģ���\n����һ���ı��ļ����ļ���һ��Ϊ��Ҫ�ϲ���depth/color ͼ��������������Ľ���K���˺�ÿ�У�\n");
		printf("[����ͼ]\n[���ͼ]������Ϊ16bit��\n[�зִ�С]��ÿ��������һ�������Σ�\n");
		printf("[�����ֵCos]��С�����εķ�������(0,0)�����ĵ����ߵļнǵ�cosֵ�ľ���ֵ��ֵ��С����ֵ���нǹ�С����Ϊhole�������Ϊblock��\n");
		printf("[��Сhole���]���������������hole��������Ⱦ��\n");
		printf("[��Сblock���]��С�����������block��������Ⱦ��\n");
		printf("[��ȱ任]��ʵ�ʵ�zֵ�������������Ϊ��Ⱦ��ȣ�Ŀǰ����ȡΪ1��\n");
		FILE *out = fopen("SampleInput.txt", "w");
		fprintf(out,"2 1672.1\n");
		fprintf(out, "Icolor.jpg Idepth.jpg 1 0.2 3 5 1.0\n");
		fprintf(out, "Icolor2.jpg Idepth2.jpg 2 0.1 2 4 1.0\n");
		return 0;
	}
	else
	{
		argv1 = argv[1];
	}
	FILE *in = fopen(argv1.c_str(), "r");
	int count;
	double K;
	fscanf(in, "%d %lf\n", &count, &K);
	ObjectGenerator OG;
	for (int i = 0; i < count; i++)
	{
		char color[1000] = {0}, depth[1000] = { 0 };
		int p, mh, mb;
		double d, div;
		fscanf(in, "%s %s %d %lf %d %d %lf\n", color, depth, &p, &d, &mh, &mb, &div);
		OG.AddMesh(color, depth, p, d, mh, mb, div, K);
	}
	fclose(in);
	
	//OG.AddMesh(argv[1], argv[2], PieceSize, DepthSeg, MinimumArea);
	//OG.OutputSingleObj(".", 0);
	OG.OutputMixedObj(".", "MixedObject");
	SysUtil::copyFile(argv1, "MixedObject/" + argv1);
	return 0;
}