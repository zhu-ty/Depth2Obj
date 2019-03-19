#include "VideoRender.h"
#include "ObjectGenerator.h"
#include "SKEncoder.h"

int ObjectVideoRender::Init(cv::Size finalSize, double cameraK, double E, double previewRatio)
{
	_cameraK = cameraK;
	_stereoE = E;
	_size = finalSize;
	SKOpenGL::window::WindowSetting set;
	set.width = _size.width * previewRatio;
	set.height = _size.height * previewRatio;
	set.renderMode = SKOpenGL::window::RenderMode::Window;
	SKOpenGL::window::InitGlfw(set, "VideoRenderPreview");

	_render_buffer.init(SKOpenGL::camera(), _size.width, _size.height);
	std::string verShader =
		"\
#version 450 core \n\
layout(location = 0) in vec3 vertexPosition;\n\
layout(location = 1) in vec2 vertexUV;\n\
out vec2 UV;\n\
uniform mat4 MVP;\n\
void main() {\n\
	gl_Position = MVP * vec4(vertexPosition, 1);\n\
	UV = vertexUV;\n\
}\n\
";
	std::string fraShader =
		"\
#version 450 core\n\
in vec2 UV;\n\
out vec4 colorRGBA;\n\
uniform sampler2D myTextureSampler;\n\
uniform mat4 colorCorrection;\n\
uniform vec4 colorAppend;\n\
void main() {\n\
	colorRGBA = colorCorrection * texture(myTextureSampler, UV).rgba + colorAppend.rgba;\n\
}\n\
";
	_shaderID = SKOpenGL::shader::CreateShaders(verShader, fraShader);
	return 0;
}

int ObjectVideoRender::RenderFrame(cv::Mat color, cv::Mat disparity, SKOpenGL::camera cam, SKOpenGL::callback & recall)
{
	_render_buffer.clearBuffer();
	this->_draw_layer(color, disparity, cam);
	cv::Mat drawed;
	_render_buffer.getTextureCPU_RGB(drawed);
	_buffered_frame.push_back(drawed);
	SKOpenGL::window::Render(_render_buffer.glTextureID, recall);
	return 0;
}

int ObjectVideoRender::RenderFrame(std::vector<cv::Mat> colors, std::vector<cv::Mat> disparities, SKOpenGL::camera cam, SKOpenGL::callback & recall)
{
	_render_buffer.clearBuffer();
	for (int i = 0; i < colors.size(); i++)
		this->_draw_layer(colors[i], disparities[i], cam);
	cv::Mat drawed;
	_render_buffer.getTextureCPU_RGB(drawed);
	_buffered_frame.push_back(drawed);
	SKOpenGL::window::Render(_render_buffer.glTextureID, recall);
	return 0;
}

int ObjectVideoRender::SaveImgs(std::string dir)
{
	SKCommon::mkdir(dir);
	for (int i = 0; i < _buffered_frame.size(); i++)
	{
		cv::Mat bgr;
		cv::cvtColor(_buffered_frame[i], bgr, cv::COLOR_RGB2BGR);
		cv::imwrite(SKCommon::format("%s/%5d.png", dir.c_str(), i), bgr);
	}
	_buffered_frame.clear();
	return 0;
}

int ObjectVideoRender::SaveVideos(std::string file_name)
{
	if (_buffered_frame.size() <= 0)
		return -1;
	SKEncoder ske;
	ske.init(_buffered_frame.size(), _size, file_name, SKEncoder::FrameType::ABGR);
	for (int i = 0; i < _buffered_frame.size(); i++)
	{
		cv::cuda::GpuMat rgb(_buffered_frame[i]), rgba;
		cv::cuda::cvtColor(rgb,rgba, cv::COLOR_RGB2RGBA);
		ske.encode(rgba.data, rgba.step);
	}
	ske.endEncode();
	_buffered_frame.clear();
	return 0;
}

int ObjectVideoRender::GetNewestFrame(cv::Mat & ret)
{
	if (_buffered_frame.size() <= 0)
		return -1;
	ret = _buffered_frame[_buffered_frame.size() - 1];
	return 0;
}

int ObjectVideoRender::_draw_layer(cv::Mat color, cv::Mat disparity, SKOpenGL::camera cam)
{
	std::vector<float> ldata, udata;
	std::vector<int> edata;
	GLuint VAO, LVBO, UVBO, TEX, EBO;
	ObjectGenerator og;
	og.AddMeshDisparity(color, disparity, _stereoE, _cameraK);
	og.OutputMixedData(ldata, udata, edata);
	//og.OutputMixedObj("VideoFrames");

	SKOpenGL::data::LoadTexture(color, TEX);
	SKOpenGL::data::Upload3DPoints(LVBO, UVBO, ldata.data(), udata.data(), ldata.size() / 3);
	SKOpenGL::data::GenerateVAO(VAO, LVBO, UVBO, (unsigned int*)edata.data(), edata.size(), EBO);

	SKOpenGL::framebuffer::Layer lay;
	lay.programID = _shaderID;
	lay.colorAppen = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	lay.colorCorre = glm::mat4();
	lay.depOption = static_cast<SKOpenGL::framebuffer::DepthOption>(0);
	lay.order = SKOpenGL::framebuffer::RenderOrder::Whatever;
	lay.model = glm::mat4();
	lay.texID = TEX;
	lay.triCount = edata.size() / 3;
	lay.glBufs.VaoID = VAO;
	lay.glBufs.LVBO = LVBO;
	lay.glBufs.UVBO = UVBO;
	lay.glBufs.EboID = EBO;
	lay.renDataType = SKOpenGL::framebuffer::RenderDataType::Element;

	_render_buffer.Camera = cam;
	_render_buffer.drawLayers(std::vector<SKOpenGL::framebuffer::Layer>({ lay }));

	SKOpenGL::data::DeleteTexure(TEX);
	SKOpenGL::data::DeleteArrays(VAO);
	SKOpenGL::data::DeleteBuffer(EBO);
	SKOpenGL::data::DeleteBuffer(LVBO);
	SKOpenGL::data::DeleteBuffer(UVBO);
	return 0;
}
