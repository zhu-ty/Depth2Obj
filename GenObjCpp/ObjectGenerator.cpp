#include "ObjectGenerator.h"

//int ObjectGenerator::CalculateAreaStackOverFlow(int c, int r, int updown, std::vector<Tri*>& list_this, mesh &m)
//{
//	if (c < 0 || r < 0 || c >= m.col() - 1 || r >= m.row() - 1)
//		return 0;
//	int ret = 0;
//	Tri *t;
//	t = &(m.meshTRI[m.mapTriangle(c, r, updown)]);
//	if (t->hole == true && t->areaCalculated == 0)
//	{
//		t->areaCalculated = -1;
//		list_this.push_back(t);
//		ret += 1;
//		if (updown == 0)
//		{
//			ret += CalculateAreaStackOverFlow(c, r, 1, list_this, m);
//			ret += CalculateAreaStackOverFlow(c, r - 1, 1, list_this, m);
//			ret += CalculateAreaStackOverFlow(c + 1, r, 1, list_this, m);
//		}
//		else
//		{
//			ret += CalculateAreaStackOverFlow(c, r, 0, list_this, m);
//			ret += CalculateAreaStackOverFlow(c - 1, r, 0, list_this, m);
//			ret += CalculateAreaStackOverFlow(c, r + 1, 0, list_this, m);
//		}
//	}
//	return ret;
//}

int ObjectGenerator::OutputSegMat(std::string file, int meshID, bool hole)
{
	cv::Mat segMat, segMatColor, segMatColor255;
	cv::Mat countMat;
	countMat.create(meshs[meshID].row() - 1, meshs[meshID].col() - 1, CV_16UC1);
	countMat.setTo(cv::Scalar(0));
	segMat.create(meshs[meshID].row() - 1, meshs[meshID].col() - 1, CV_32FC3);
	segMat.setTo(cv::Scalar(1.0f, 1.0f, 1.0f));
	for (int i = 0; i < meshs[meshID].row() - 1; i++)
		for (int j = 0; j < meshs[meshID].col() - 1; j++)
		{
			int t1 = meshs[meshID].mapTriangle(j, i, 0);
			int t2 = meshs[meshID].mapTriangle(j, i, 1);
			int AC1 = (!(hole ^ (meshs[meshID].meshTRI[t1].areaCalculated < 0))) ? abs(meshs[meshID].meshTRI[t1].areaCalculated) : 0;
			int AC2 = (!(hole ^ (meshs[meshID].meshTRI[t2].areaCalculated < 0))) ? abs(meshs[meshID].meshTRI[t2].areaCalculated) : 0;

			segMat.at<cv::Vec3f>(i, j)[0] = 240.0f - (AC1 + AC2) / (2.0f * (hole ? meshs[meshID].maxAreaHole : meshs[meshID].maxAreaBlock)) * 240.0f;
			countMat.at<unsigned short>(i, j) = (AC1 + AC2 > 65535) ? 65535 : (AC1 + AC2);
			//test_Mat.at<unsigned short>(i, j) = meshTRI[j * (h / PieceSize - 1) * 2 + i].areaCalculated;
		}
	cv::cvtColor(segMat, segMatColor, CV_HSV2BGR);
	segMatColor.convertTo(segMatColor255, CV_8UC3, 255.0f);
	cv::imwrite(file + ".jpg", segMatColor255);

	cv::imwrite(file + "CountMat.tiff", countMat);
	return 0;
}

int ObjectGenerator::CalculateArea(int c, int r, int updown, std::vector<Tri*>& list_this, mesh &m, bool hole)
{
	Tri *t;
	if ((t = getTri(c,r,updown,m, hole)) == nullptr)
		return 0;
	int ret = 0;
	std::vector<Tri*> list_stack;
	list_this.push_back(t);
	list_stack.push_back(t);
	while (list_stack.size() > 0)
	{
		Tri* tri_back = list_stack.back();
		tri_back->areaCalculated = hole ? -1 : 1;
		Tri* child;
		if ((child = getTri(tri_back->c(), tri_back->r(), 1 - tri_back->updown(), m, hole)) != nullptr)
		{
			list_stack.push_back(child);
			list_this.push_back(child);
			continue;
		}
		if ((child = getTri(tri_back->c() + ((tri_back->updown() == 0) ? 1 : -1), tri_back->r(), 1 - tri_back->updown(), m, hole)) != nullptr)
		{
			list_stack.push_back(child);
			list_this.push_back(child);
			continue;
		}
		if ((child = getTri(tri_back->c(), tri_back->r() - ((tri_back->updown() == 0) ? 1 : -1), 1 - tri_back->updown(), m, hole)) != nullptr)
		{
			list_stack.push_back(child);
			list_this.push_back(child);
			continue;
		}
		ret++;
		list_stack.pop_back();
	}
	return ret;
}

ObjectGenerator::Tri * ObjectGenerator::getTri(int c, int r, int updown, mesh & m, bool hole)
{
	if (c < 0 || r < 0 || c >= m.col() - 1 || r >= m.row() - 1)
		return nullptr;
	Tri *t;
	t = &(m.meshTRI[m.mapTriangle(c, r, updown)]);
	if (t->hole == hole && t->areaCalculated == 0)
		return t;
	return nullptr;
}


int ObjectGenerator::AddMesh(std::string colorFile, std::string depthFile, int pieceSize, double depthSeg, int minimumAreaHole, int minimumAreaBlock, double depthDiv)
{
	cv::Mat c, d;
	c = cv::imread(colorFile);
	d = cv::imread(depthFile, CV_LOAD_IMAGE_UNCHANGED);
	this->AddMesh(c, d, pieceSize, depthSeg, minimumAreaHole, minimumAreaBlock, depthDiv);
	return 0;
}

int ObjectGenerator::AddMesh(cv::Mat color, cv::Mat depth, int pieceSize, double depthSeg, int minimumAreaHole, int minimumAreaBlock, double depthDiv)
{
	if (depth.type() != CV_16UC1)
	{
		SysUtil::errorOutput("ObjectGenerator::AddMesh only depth = CV_16UC1 supported.");
		return -1;
	}
	mesh m(color, depth, pieceSize, depthSeg, minimumAreaHole, minimumAreaBlock, depthDiv);
	for (int i = 0; i < m.col(); i++)
	{
		for (int j = 0; j < m.row(); j++)
		{
			Pt t_pt;
			t_pt.imgPos = cv::Point(m.mapImage(i), m.mapImage(j));
			t_pt.idxPos = cv::Point(i, j);
			t_pt.globalIdx = global_id;
			t_pt.depth = (double)(depth.at<uint16_t>(m.mapImage(j), m.mapImage(i))) / m.depthDiv; //at(row,col) --so--> at(j,i)
			m.meshPT.push_back(t_pt);
			global_id++;
		}
	}


	for (int i = 0; i < m.col() - 1; i++)
	{
		for (int j = 0; j < m.row() - 1; j++)
		{
			Tri t_tri1, t_tri2;
			Pt t1, t2, t3, t4;
			t1 = m.meshPT[i * m.row() + j];
			t2 = m.meshPT[(i + 1) * m.row() + j];
			t3 = m.meshPT[i * m.row() + j + 1];
			t4 = m.meshPT[(i + 1) * m.row() + j + 1];
			t_tri1.pts[0] = t1;
			t_tri1.pts[1] = t4;
			t_tri1.pts[2] = t2;
			if (t1.absdiff(t4) > m.depthSeg || t1.absdiff(t2) > m.depthSeg || t4.absdiff(t2) > m.depthSeg
				|| t1.depth <= 0 || t2.depth <= 0 || t4.depth <= 0)
				t_tri1.hole = true;
			m.meshTRI.push_back(t_tri1);
			t_tri2.pts[0] = t1;
			t_tri2.pts[1] = t3;
			t_tri2.pts[2] = t4;
			if (t1.absdiff(t4) > m.depthSeg || t1.absdiff(t3) > m.depthSeg || t4.absdiff(t3) > m.depthSeg
				|| t1.depth <= 0 || t3.depth <= 0 || t4.depth <= 0)
				t_tri2.hole = true;
			m.meshTRI.push_back(t_tri2);
		}
	}

	//step 1: find holes and turn small holes into blocks
	for (int i = 0; i < m.meshTRI.size(); i++)
	{
		if (m.meshTRI[i].areaCalculated == 0)
		{
			int updown, c, r;
			m.mapTriangleXY(i, c, r, updown);
			if (m.meshTRI[i].hole)
			{
				std::vector<Tri*> list_this;
				int count = CalculateArea(c, r, updown, list_this, m, true);
				if (count < m.minimumAreaHole)
				{
					for (int j = 0; j < list_this.size(); j++)
					{
						//turn them into blocks
						list_this[j]->areaCalculated = 0;
						list_this[j]->hole = false;
					}
				}
				else
				{
					for (int j = 0; j < list_this.size(); j++)
						list_this[j]->areaCalculated = -count;
					if (count > m.maxAreaHole)
						m.maxAreaHole = count;
				}

			}
		}
	}
	//step 2: find blocks
	for (int i = 0; i < m.meshTRI.size(); i++)
	{
		if (m.meshTRI[i].areaCalculated == 0)
		{
			int updown, c, r;
			m.mapTriangleXY(i, c, r, updown);
			if (m.meshTRI[i].hole == false)
			{
				std::vector<Tri*> list_this;
				int count = CalculateArea(c, r, updown, list_this, m, false);
				for (int j = 0; j < list_this.size(); j++)
					list_this[j]->areaCalculated = count;
				if (count > m.maxAreaBlock)
					m.maxAreaBlock = count;
			}	
		}
	}
	meshs.push_back(m);
	return 0;
}

int ObjectGenerator::OutputSingleObj(std::string dir, int meshID, std::string name)
{
	std::string dirname = dir + "/" + name;
	SysUtil::mkdir(dirname);

	FILE *param = fopen((dirname +
		SysUtil::format("/%s_%d_%.1f_%d_%d_%.1f.txt",
			name.c_str(),
			meshs[meshID].pieceSize,
			meshs[meshID].depthSeg,
			meshs[meshID].minimumAreaHole,
			meshs[meshID].minimumAreaBlock,
			meshs[meshID].depthDiv)).c_str(), "w");
	fprintf(param, "PieceSize = %d\nDepthSeg = %f\nMinimumAreaHole = %d\nMinimumAreaBlock = %d\nDepthDiv = %f\n",
		meshs[meshID].pieceSize, meshs[meshID].depthSeg, meshs[meshID].minimumAreaHole, meshs[meshID].minimumAreaBlock, meshs[meshID].depthDiv);
	fclose(param);

	FILE *obj = fopen((dirname + "/object.obj").c_str(), "w");
	fprintf(obj, "mtllib texture.mtl\nusemtl 01___Default\n");
	for (int i = 0; i < meshs[meshID].meshPT.size(); i++)
	{
		fprintf(obj, "v %f %f %f\n", (double)meshs[meshID].meshPT[i].imgPos.x, -(double)meshs[meshID].meshPT[i].imgPos.y, -meshs[meshID].meshPT[i].depth);
		fprintf(obj, "vt %f %f %f\n", meshs[meshID].meshPT[i].imgPos.x / (double)meshs[meshID].w, 1.0 - meshs[meshID].meshPT[i].imgPos.y / (double)meshs[meshID].h, 0.0);
	}
	int global_id_diff = meshs[meshID].meshPT[0].globalIdx - 1;
	for (int i = 0; i < meshs[meshID].meshTRI.size(); i++)
	{
		if ((meshs[meshID].meshTRI[i].hole == false && meshs[meshID].meshTRI[i].areaCalculated > meshs[meshID].minimumAreaBlock) ||
			(meshs[meshID].meshTRI[i].hole == true && abs(meshs[meshID].meshTRI[i].areaCalculated) < meshs[meshID].minimumAreaHole))
		{
			fprintf(obj, "f %d/%d %d/%d %d/%d\n",
				meshs[meshID].meshTRI[i].pts[0].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[0].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[1].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[1].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[2].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[2].globalIdx - global_id_diff);
		}
			
	}
	fclose(obj);

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

	cv::imwrite(dirname + "/texture.jpg", meshs[meshID].IC);
	cv::imwrite(dirname + "/depth.png", meshs[meshID].ID);

	OutputSegMat(dirname + "/seg_block", meshID, false);
	OutputSegMat(dirname + "/seg_hole", meshID, true);
	return 0;
}

int ObjectGenerator::OutputMixedObj(std::string dir, std::string name)
{
	std::string dirname = dir + "/" + name;
	SysUtil::mkdir(dirname);
	FILE *obj = fopen((dirname + "/object.obj").c_str(), "w");
	fprintf(obj, "mtllib texture.mtl\n");
	for (int meshID = 0; meshID < meshs.size(); meshID++)
	{
		for (int i = 0; i < meshs[meshID].meshPT.size(); i++)
		{
			fprintf(obj, "v %f %f %f\n", (double)meshs[meshID].meshPT[i].imgPos.x, -(double)meshs[meshID].meshPT[i].imgPos.y, -meshs[meshID].meshPT[i].depth);
			fprintf(obj, "vt %f %f %f\n", meshs[meshID].meshPT[i].imgPos.x / (double)meshs[meshID].w, 1.0 - meshs[meshID].meshPT[i].imgPos.y / (double)meshs[meshID].h, 0.0);
		}
	}
	for (int meshID = 0; meshID < meshs.size(); meshID++)
	{
		fprintf(obj, "usemtl %d__mtl\n", meshID);
		for (int i = 0; i < meshs[meshID].meshTRI.size(); i++)
		{
			if ((meshs[meshID].meshTRI[i].hole == false && meshs[meshID].meshTRI[i].areaCalculated > meshs[meshID].minimumAreaBlock) ||
				(meshs[meshID].meshTRI[i].hole == true && abs(meshs[meshID].meshTRI[i].areaCalculated) < meshs[meshID].minimumAreaHole))
			{
				fprintf(obj, "f %d/%d %d/%d %d/%d\n",
					meshs[meshID].meshTRI[i].pts[0].globalIdx,
					meshs[meshID].meshTRI[i].pts[0].globalIdx,
					meshs[meshID].meshTRI[i].pts[1].globalIdx,
					meshs[meshID].meshTRI[i].pts[1].globalIdx,
					meshs[meshID].meshTRI[i].pts[2].globalIdx,
					meshs[meshID].meshTRI[i].pts[2].globalIdx);
			}
		}
	}
	fclose(obj);

	FILE *tex = fopen((dirname + "/texture.mtl").c_str(), "w");
	for (int meshID = 0; meshID < meshs.size(); meshID++)
	{
		fprintf(tex, "newmtl %d__mtl \n\
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
			map_Ka texture%d.jpg\n\
			map_Kd texture%d.jpg\n", meshID, meshID, meshID);
		cv::imwrite(
			cv::format("%s/texture%d.jpg", dirname.c_str(), meshID),
			meshs[meshID].IC);
		cv::imwrite(
			cv::format("%s/depth%d.png", dirname.c_str(), meshID),
			meshs[meshID].ID);
	}
	fclose(tex);

	for (int meshID = 0; meshID < meshs.size(); meshID++)
	{
		this->OutputSingleObj(dirname, meshID, SysUtil::format("Single%d", meshID));
	}

	return 0;
}
