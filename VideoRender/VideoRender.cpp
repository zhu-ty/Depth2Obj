#include "VideoRender.h"

int ObjectVideoRender::Init(cv::Size finalSize, double previewRatio)
{
	SKOpenGL::window::WindowSetting set;
	set.width = finalSize.width * previewRatio;
	set.height = finalSize.height * previewRatio;
	set.renderMode = SKOpenGL::window::RenderMode::Window;
	SKOpenGL::window::InitGlfw(set, "VideoRenderPreview");

	_render_buffer.init(SKOpenGL::camera(), finalSize.width, finalSize.height);
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
