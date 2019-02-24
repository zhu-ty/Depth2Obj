/**
@brief ObjectGenerator.h
Generate Object data & output
@author zhu-ty
@date Oct 11, 2018
*/


#ifndef __GEN_OBJ_OBJECT_GENERATOR__
#define __GEN_OBJ_OBJECT_GENERATOR__



// include stl
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "SysUtil.hpp"
#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>

class ObjectGenerator
{
	/* index : col then row
	0--------->x
	| 0   h+0
	| 1   h+1
	| 2   h+2
	| 3   h+3
	| ..  ..
	vy
	*/
public:
	ObjectGenerator() {};
	~ObjectGenerator() {};
	int AddMesh(std::string colorFile, std::string depthFile,
		int pieceSize = 2, double depthSeg = 10.0, int minimumAreaHole = 2, int minimumAreaBlock = 2, double depthDiv = 1.0, double cameraK = 1e3);

	int AddMesh(cv::Mat color, cv::Mat depth,
		int pieceSize = 2, double depthSeg = 10.0, int minimumAreaHole = 2, int minimumAreaBlock = 2, double depthDiv = 1.0, double cameraK = 1e3);

	int OutputSingleObj(std::string dir, int meshID, std::string name = "SingleObject");

	int OutputMixedObj(std::string dir, std::string name = "MixedObject");

private:
	struct Pt
	{
		//x,y
		cv::Point imgPos;
		//col,row
		cv::Point idxPos;
		//Converted pos
		cv::Point3i renderPos;
		double depth;
		int globalIdx;
		double operator-(Pt &another) //TODO : change it to pos - pos
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
		int updown()
		{
			return (pts[1].idxPos.x == pts[0].idxPos.x) ? 1 : 0;
		}
		int c()
		{
			return pts[0].idxPos.x;
		}
		int r()
		{
			return pts[0].idxPos.y;
		}
		Pt pts[3];
		bool hole;
		int areaCalculated; 
		//0 -> not caculated, -num -> hole area size, -1 -> calculating hole size
		//+num -> block area size, +1 -> calculating block size
	};

	struct mesh
	{
		int w, h, pieceSize, minimumAreaHole, minimumAreaBlock;
		float depthSeg;
		double depthDiv, cameraK;
		int maxAreaHole;
		int maxAreaBlock;
		cv::Mat IC;
		cv::Mat ID;
		std::vector<Pt> meshPT;
		std::vector<Tri> meshTRI;

		//mesh()
		//{
		//	w = 0;
		//	h = 0;
		//	PieceSize = 2;
		//	MinmumArea = 2;
		//	DepthSeg = 10.0f;
		//}

		mesh(cv::Mat _IC, cv::Mat _ID, int _pieceSize, double _depthSeg, int _minimumAreaHole, int _minimumAreaBlock, double _depthDiv)
		{
			IC = _IC;
			ID = _ID;
			if (IC.cols != ID.cols && IC.rows != ID.rows)
			{
				SysUtil::errorOutput("mesh IC/ID row/col not match!");
			}
			w = ID.cols;
			h = ID.rows;
			pieceSize = _pieceSize;
			depthSeg = _depthSeg;
			minimumAreaHole = _minimumAreaHole;
			minimumAreaBlock = _minimumAreaBlock;
			maxAreaHole = 0;
			maxAreaBlock = 0;
			depthDiv = _depthDiv;
			cameraK = IC.cols / 2;
		}

		// row = h / PieceSize
		int row() { return h / pieceSize; }
		// col = w / PieceSize
		int col() { return w / pieceSize; }
		// return p * PieceSize
		int mapImage(int p) { return p * pieceSize; }
		// return p / PieceSize
		int mapMesh(int p) { return p / pieceSize; }
		// map c,r(col,row) of the left-up point of 2 triangles (a square), and updown will point out which one.
		// c,r    2
		// | \    ^
		// |  \ 0 |
		// |   \  |
		// | 1  \ |
		// v     >1
		// 1----->2
		//
		int mapTriangleXY(int triangleIndex, int &c,int &r, int &updown)
		{
			updown = triangleIndex % 2;
			int idx_pt = triangleIndex / 2;
			c = idx_pt / (row() - 1);
			r = idx_pt % (row() - 1);
			return 0;
		}
		// map Triangle with left-up point & updown
		// return the index of that triangle
		int mapTriangle(int c, int r, int updown)
		{
			return (c * (row() - 1) + r) * 2 + updown;
		}
	};

	int OutputSegMat(std::string file,int meshID, bool hole);

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
	//static int CalculateAreaStackOverFlow(int c, int r, int updown, std::vector<Tri*> &list_this, mesh &m);

	static int CalculateArea(int c, int r, int updown, std::vector<Tri*> &list_this, mesh &m, bool hole);

	static Tri* getTri(int c, int r, int updown, mesh &m, bool hole);

	

	std::vector<mesh> meshs;
	int global_id = 1;
};




#endif //!__GEN_OBJ_OBJECT_GENERATOR__