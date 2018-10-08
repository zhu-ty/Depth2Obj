#include<iostream>
#include<vector>

#include"SysUtil.hpp"
#include<opencv2\opencv.hpp>
#include<opencv2/imgcodecs.hpp>


/* index : col then row
0--------->x
| 0   h+0
| 1   h+1
| 2   h+2
| 3   h+3
| ..  ..
vy
*/


struct Pt
{
	cv::Point imgPos;
	cv::Point idxPos;
	double depth;
	int globalIdx;


	double operator-(Pt &another)
	{
		return this->depth - another.depth;
	}

	double absdiff(Pt &another)
	{
		return abs(*this - another);
	}
};

struct Tri
{
	Tri()
	{
		hole = false;
		areaCalculated = false;
	}
	Pt pts[3];
	bool hole;
	int areaCalculated; //0 -> not caculated, num -> hole area size, -1 -> calculating
};

//critical: [i,j] may mark 2 triangles, so we use updown = 0/1 to switch each of them
/*

00     2
| \    ^
|  \ 0 |
|   \  |
| 1  \ |
v     >1
1----->2
*/
int CalculateArea(int i, int j, int updown, int width_p, int height_p, std::vector<Tri> &list_all, std::vector<Tri*> &list_this)
{
	if (i < 0 || j < 0 || i >= width_p - 1 || j >= height_p - 1)
		return 0;
	int ret = 0;
	Tri *t;
	if (updown == 0)
		t = &list_all[(i * (height_p - 1) + j) * 2];
	else
		t = &list_all[(i * (height_p - 1) + j) * 2 + 1];
	if (t->hole == true && t->areaCalculated == 0)
	{
		t->areaCalculated = -1;
		list_this.push_back(t);
		ret += 1;
		if (updown == 0)
		{
			ret += CalculateArea(i, j, 1, width_p, height_p, list_all, list_this);
			ret += CalculateArea(i, j - 1, 1, width_p, height_p, list_all, list_this);
			ret += CalculateArea(i + 1, j, 1, width_p, height_p, list_all, list_this);
		}
		else
		{
			ret += CalculateArea(i, j, 0, width_p, height_p, list_all, list_this);
			ret += CalculateArea(i - 1, j, 0, width_p, height_p, list_all, list_this);
			ret += CalculateArea(i, j + 1, 0, width_p, height_p, list_all, list_this);
		}
	}
	return ret;
}


int main(int argc, char* argv[])
{
	if (argc != 3 && argc != 6)
	{
		printf("Input Format:\nGenObkCpp Icolor Idepth [PieceSize=2 DepthSeg=10.0 MinimumArea=2]\n");
		printf("Input Sample 1:\nGenObjCpp Icolor.jpg Idepth.jpg 1 5 3\n");
		printf("Input Sample 2:\nGenObjCpp Icolor.jpg Idepth.jpg\n");
		return 0;
	}

	int PieceSize = 2, MinmumArea = 2;
	float DepthSeg = 10.0f;
	if (argc == 6)
	{
		PieceSize = std::atoi(argv[3]);
		DepthSeg = std::atof(argv[4]);
		MinmumArea = std::atoi(argv[5]);
	}

	cv::Mat IC = cv::imread(argv[1]);
	cv::Mat ID = cv::imread(argv[2], CV_LOAD_IMAGE_UNCHANGED);
	std::vector<Pt> meshPT;
	std::vector<Tri> meshTRI;

	int w = ID.cols; //width
	int h = ID.rows; //height

	int gloid = 1;

	for (int i = 0; i < w / PieceSize; i++)
	{
		for (int j = 0; j < h / PieceSize; j++)
		{
			Pt t_pt;
			t_pt.imgPos = cv::Point(i * PieceSize, j * PieceSize);
			t_pt.idxPos = cv::Point(i, j);
			t_pt.globalIdx = gloid;
			t_pt.depth = (double)(ID.at<uint16_t>(j * PieceSize, i * PieceSize));
			meshPT.push_back(t_pt);
			gloid++;
		}
	}

	for (int i = 0; i < w / PieceSize - 1; i++)
	{
		for (int j = 0; j < h / PieceSize - 1; j++)
		{
			Tri t_tri1, t_tri2;
			Pt t1, t2, t3, t4;
			t1 = meshPT[i * h / PieceSize + j];
			t2 = meshPT[(i + 1) * h / PieceSize + j];
			t3 = meshPT[i * h / PieceSize + j + 1];
			t4 = meshPT[(i + 1) * h / PieceSize + j + 1];
			t_tri1.pts[0] = t1;
			t_tri1.pts[1] = t4;
			t_tri1.pts[2] = t2;
			if (t1.absdiff(t4) > DepthSeg || t1.absdiff(t2) > DepthSeg || t4.absdiff(t2) > DepthSeg
				|| t1.depth <= 0 || t2.depth <= 0 || t4.depth <= 0)
				t_tri1.hole = true;
			meshTRI.push_back(t_tri1);
			t_tri2.pts[0] = t1;
			t_tri2.pts[1] = t3;
			t_tri2.pts[2] = t4;
			if (t1.absdiff(t4) > DepthSeg || t1.absdiff(t3) > DepthSeg || t4.absdiff(t3) > DepthSeg
				|| t1.depth <= 0 || t3.depth <= 0 || t4.depth <= 0)
				t_tri2.hole = true;
			meshTRI.push_back(t_tri2);
		}
	}
	
	for (int i = 0; i < meshTRI.size(); i++)
	{
		if (meshTRI[i].hole == true && meshTRI[i].areaCalculated == 0)
		{
			int updown = i % 2;
			int idx_pt = i / 2;
			int i_p = idx_pt / ((h / PieceSize) - 1);
			int j_p = idx_pt % ((h / PieceSize) - 1);
			std::vector<Tri*> list_this;
			int count = CalculateArea(i_p, j_p, updown, w / PieceSize, h / PieceSize, meshTRI, list_this);
			for (int j = 0; j < list_this.size(); j++)
				list_this[j]->areaCalculated = count;
		}
	}



	//output
	std::string dirname = SysUtil::format("%s_%d_%.1f_%d", argv[1], PieceSize, DepthSeg, MinmumArea);
	SysUtil::mkdir(dirname);

	FILE *obj = fopen((dirname + "/object.obj").c_str(), "w");
	fprintf(obj, "mtllib texture.mtl\nusemtl 01___Default\n");
	for (int i = 0; i < meshPT.size(); i++)
	{
		fprintf(obj, "v %f %f %f\n", (double)meshPT[i].imgPos.x, -(double)meshPT[i].imgPos.y, -meshPT[i].depth);
		fprintf(obj, "vt %f %f %f\n", meshPT[i].imgPos.x / (double)w, 1.0 - meshPT[i].imgPos.y / (double)h, 0.0);
	}
	for (int i = 0; i < meshTRI.size(); i++)
	{
		if (meshTRI[i].hole == false || meshTRI[i].areaCalculated < MinmumArea)
			fprintf(obj, "f %d/%d %d/%d %d/%d\n",
				meshTRI[i].pts[0].globalIdx, meshTRI[i].pts[0].globalIdx,
				meshTRI[i].pts[1].globalIdx, meshTRI[i].pts[1].globalIdx,
				meshTRI[i].pts[2].globalIdx, meshTRI[i].pts[2].globalIdx);
	}
	fclose(obj);

	cv::Mat test_Mat, _8bitMat;
	test_Mat.create((h / PieceSize - 1) * 2, w / PieceSize - 1, CV_16UC1);
	for(int i = 0 ;i < (h / PieceSize - 1) * 2;i++)
		for (int j = 0; j < w / PieceSize - 1; j++)
		{
			test_Mat.at<unsigned short>(i, j) = meshTRI[j * (h / PieceSize - 1) * 2 + i].areaCalculated;
		}
	test_Mat.convertTo(_8bitMat, CV_8U);
	cv::equalizeHist(_8bitMat, _8bitMat);
	cv::imwrite(dirname + "/seg.jpg", _8bitMat);



	FILE *tex = fopen((dirname + "/texture.mtl").c_str(), "w");
	fprintf(tex, "newmtl 01___Default \n\
		Ns 10.0000\n\
		Ni 1.5000\n\
		d 1.0000\n\
		Tr 0.0000\n\
		Tf 1.0000 1.0000 1.0000 \n\
		illum 2\n\
		Ka 0.5882 0.5882 0.5882\n\
		Kd 0.5882 0.5882 0.5882\n\
		Ks 0.0000 0.0000 0.0000\n\
		Ke 0.0000 0.0000 0.0000\n\
		map_Ka texture.jpg\n\
		map_Kd texture.jpg\n");
	fclose(tex);


	//CopyFile(argv[1], (dirname + "texture.jpg").c_str(), false);
	cv::imwrite(dirname + "/texture.jpg", IC);
	return 0;
}