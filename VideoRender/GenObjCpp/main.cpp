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
		printf("input.txt:\n[num] [PieceSize] [DepthSegCos] [K] [E]\n[Color] [Disparity]\n");
		printf("������ͣ����ģ���\n����һ���ı��ļ����ļ���һ��Ϊ��Ҫ�ϲ���Disparity/color ͼ���������зִ�С��ÿ��������һ�������Σ��������ֵCos������Ľ���K����ȱ任��E���˺�ÿ�У�\n");
		printf("[����ͼ]\n[�Ӳ�ͼ]������Ϊ32bit float��");
		FILE *out = fopen("SampleInput.txt", "w");
		fprintf(out, "2 2 0.05 1672.1 27000\n");
		fprintf(out, "Icolor.jpg Idisparity.tiff\n");
		fprintf(out, "Icolor2.jpg Idisparity2.tiff\n");
		return 0;
	}
	else if (argc >= 2)
	{
		argv1 = argv[1];
	}
	FILE *in = fopen(argv1.c_str(), "r");
	int count, PieS;
	double DepSeg, K, E;
	fscanf(in, "%d %d %lf %lf %lf\n", &count,&PieS, &DepSeg, &K, &E);
	ObjectGenerator OG;
	for (int i = 0; i < count; i++)
	{
		char color[1000] = { 0 }, dis_[1000] = { 0 };
		fscanf(in, "%s %s\n", color, dis_);

		cv::Mat c, dis;
		c = cv::imread(color);
		dis = cv::imread(dis_, cv::IMREAD_UNCHANGED);
		OG.AddMeshDisparity(c, dis, E, K, PieS, DepSeg);
	}
	fclose(in);
	OG.OutputMixedObj(".", "MixedObject");
	SysUtil::copyFile(argv1, "MixedObject/" + argv1);
	return 0;
}
