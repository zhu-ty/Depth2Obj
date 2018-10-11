#include "ObjectGenerator.h"

int ObjectGenerator::CalculateArea(int i, int j, int updown, int width_p, int height_p, std::vector<Tri>& list_all, std::vector<Tri*>& list_this)
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

int ObjectGenerator::CalculateArea(int c, int r, int updown, std::vector<Tri*>& list_this, mesh &m)
{
	if (c < 0 || r < 0 || c >= m.col() - 1 || r >= m.row() - 1)
		return 0;
	int ret = 0;
	Tri *t;
	t = &(m.meshTRI[m.mapTriangle(c, r, updown)]);
	if (t->hole == true && t->areaCalculated == 0)
	{
		t->areaCalculated = -1;
		list_this.push_back(t);
		ret += 1;
		if (updown == 0)
		{
			ret += CalculateArea(c, r, 1, list_this,m);
			ret += CalculateArea(c, r - 1, 1, list_this, m);
			ret += CalculateArea(c + 1, r, 1, list_this, m);
		}
		else
		{
			ret += CalculateArea(c, r, 0, list_this, m);
			ret += CalculateArea(c - 1, r, 0, list_this, m);
			ret += CalculateArea(c, r + 1, 0, list_this, m);
		}
	}
	return ret;
}

int ObjectGenerator::AddMesh(std::string colorFile, std::string depthFile, int pieceSize, double depthSeg, int minimumArea)
{
	cv::Mat c, d;
	c = cv::imread(colorFile);
	d = cv::imread(depthFile, CV_LOAD_IMAGE_UNCHANGED);
	this->AddMesh(c, d, pieceSize, depthSeg, minimumArea);
	return 0;
}

int ObjectGenerator::AddMesh(cv::Mat color, cv::Mat depth, int pieceSize, double depthSeg, int minimumArea)
{
	if (depth.type() != CV_16UC1)
	{
		SysUtil::errorOutput("ObjectGenerator::AddMesh only depth = CV_16UC1 supported.");
		return -1;
	}
	mesh m(color, depth, pieceSize, depthSeg, minimumArea);
	for (int i = 0; i < m.col(); i++)
	{
		for (int j = 0; j < m.row(); j++)
		{
			Pt t_pt;
			t_pt.imgPos = cv::Point(m.mapImage(i), m.mapImage(j));
			t_pt.idxPos = cv::Point(i, j);
			t_pt.globalIdx = global_id;
			t_pt.depth = (double)(depth.at<uint16_t>(m.mapImage(j), m.mapImage(i))); //at(row,col) --so--> at(j,i)
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

	for (int i = 0; i < m.meshTRI.size(); i++)
	{
		if (m.meshTRI[i].hole == true && m.meshTRI[i].areaCalculated == 0)
		{
			int updown, c, r;
			m.mapTriangleXY(i, c, r, updown);
			std::vector<Tri*> list_this;
			int count = CalculateArea(c, r, updown, list_this, m);
			for (int j = 0; j < list_this.size(); j++)
				list_this[j]->areaCalculated = count;
			if (count > m.maxArea)
				m.maxArea = count;
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
		SysUtil::format("/%s_%d_%.1f_%d.txt",
			name.c_str(),
			meshs[meshID].pieceSize,
			meshs[meshID].depthSeg,
			meshs[meshID].minmumArea)).c_str(), "w");
	fprintf(param, "PieceSize = %d\nDepthSeg = %f\nMinmumArea = %d\n",
		meshs[meshID].pieceSize, meshs[meshID].depthSeg, meshs[meshID].minmumArea);
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
		if (meshs[meshID].meshTRI[i].hole == false || meshs[meshID].meshTRI[i].areaCalculated < meshs[meshID].minmumArea)
			fprintf(obj, "f %d/%d %d/%d %d/%d\n",
				meshs[meshID].meshTRI[i].pts[0].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[0].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[1].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[1].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[2].globalIdx - global_id_diff,
				meshs[meshID].meshTRI[i].pts[2].globalIdx - global_id_diff);
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

	cv::Mat segMat,segMatColor, segMatColor255;
	segMat.create(meshs[meshID].row() - 1, meshs[meshID].col() - 1, CV_32FC3);
	segMat.setTo(cv::Scalar(1.0f, 1.0f, 1.0f));
	for (int i = 0; i < meshs[meshID].row() - 1; i++)
		for (int j = 0; j < meshs[meshID].col() - 1; j++)
		{
			int t1 = meshs[meshID].mapTriangle(j, i, 0);
			int t2 = meshs[meshID].mapTriangle(j, i, 1);
			segMat.at<cv::Vec3f>(i, j)[0] = 240.0f - (meshs[meshID].meshTRI[t1].areaCalculated + meshs[meshID].meshTRI[t2].areaCalculated) / (2.0f * meshs[meshID].maxArea) * 240.0f;
			//test_Mat.at<unsigned short>(i, j) = meshTRI[j * (h / PieceSize - 1) * 2 + i].areaCalculated;
		}
	cv::cvtColor(segMat, segMatColor, CV_HSV2BGR);
	segMatColor.convertTo(segMatColor255, CV_8UC3, 255.0f);
	cv::imwrite(dirname + "/seg.jpg", segMatColor255);
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
			if (meshs[meshID].meshTRI[i].hole == false || meshs[meshID].meshTRI[i].areaCalculated < meshs[meshID].minmumArea)
				fprintf(obj, "f %d/%d %d/%d %d/%d\n",
					meshs[meshID].meshTRI[i].pts[0].globalIdx,
					meshs[meshID].meshTRI[i].pts[0].globalIdx,
					meshs[meshID].meshTRI[i].pts[1].globalIdx,
					meshs[meshID].meshTRI[i].pts[1].globalIdx,
					meshs[meshID].meshTRI[i].pts[2].globalIdx,
					meshs[meshID].meshTRI[i].pts[2].globalIdx);
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
